// src/main.cpp

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

#include "config.h"
#include "wifi_manager.h"
#include "thingsboard_manager.h"
#include "neopixel_manager.h"
#include "servo_manager.h"
#include "settings_manager.h" // <-- THÊM DÒNG NÀY

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
DHT dht(DHTPIN, DHTTYPE);

long lastSend = 0;

// Cache giá trị cũ để dùng khi bị nhiễu
float lastValidTemp = 25.0;
float lastValidHumid = 60.0;
float lastValidDistance = 10.0;

// Thời điểm relay thay đổi trạng thái gần nhất
unsigned long lastRelayChangeTime = 0;
const unsigned long RELAY_SETTLE_TIME = 500; // Chờ 500ms sau khi relay đổi trạng thái

// Hàm điều khiển relay với tracking thời gian
void setRelay(int pin, bool state)
{
  static bool pumpState = false;
  static bool heaterState = false;
  
  bool *currentState = (pin == PUMP_PIN) ? &pumpState : &heaterState;
  
  if (*currentState != state)
  {
    digitalWrite(pin, state ? HIGH : LOW);
    *currentState = state;
    lastRelayChangeTime = millis(); // Ghi nhận thời điểm đổi trạng thái
  }
}

// Kiểm tra xem có an toàn để đọc cảm biến không (relay đã ổn định)
bool isSafeToReadSensors()
{
  return (millis() - lastRelayChangeTime) > RELAY_SETTLE_TIME;
}

// Đọc DHT11 với retry (chống nhiễu)
bool readDHT_withRetry(float &temp, float &humid, int maxRetry = 3)
{
  // Chờ relay ổn định trước khi đọc
  if (!isSafeToReadSensors())
  {
    Serial.println("⏳ Chờ relay ổn định...");
    delay(RELAY_SETTLE_TIME);
  }
  
  for (int i = 0; i < maxRetry; i++)
  {
    humid = dht.readHumidity();
    temp = dht.readTemperature();
    
    if (!isnan(temp) && !isnan(humid))
    {
      // Đọc thành công, cập nhật cache
      lastValidTemp = temp;
      lastValidHumid = humid;
      return true;
    }
    delay(150); // Đợi 150ms rồi thử lại (DHT11 cần ít nhất 1s giữa các lần đọc)
  }
  
  // Thất bại sau maxRetry lần, dùng giá trị cache
  Serial.println("⚠ DHT11 lỗi (nhiễu từ relay), dùng giá trị cache");
  temp = lastValidTemp;
  humid = lastValidHumid;
  return false;
}

// Đọc khoảng cách với retry (chống nhiễu)
float getDistance_cm_withRetry(int maxRetry = 3)
{
  // Chờ relay ổn định trước khi đọc
  if (!isSafeToReadSensors())
  {
    delay(RELAY_SETTLE_TIME);
  }
  
  for (int i = 0; i < maxRetry; i++)
  {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    long duration = pulseIn(ECHO_PIN, HIGH, 30000);
    float distance_cm = (duration * 0.0343) / 2.0;

    if (distance_cm > 0 && distance_cm < 400)
    {
      lastValidDistance = distance_cm;
      return distance_cm;
    }
    delay(50); // Đợi 50ms rồi thử lại
  }
  
  // Thất bại, dùng giá trị cache
  Serial.println("⚠ HC-SR04 lỗi (nhiễu từ relay), dùng giá trị cache");
  return lastValidDistance;
}

float getDistance_cm()
{
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  float distance_cm = (duration * 0.0343) / 2.0;

  if (distance_cm == 0 || distance_cm > 400)
  {
    return -1;
  }
  return distance_cm;
}

void setup()
{
  Serial.begin(115200);

  // Khởi tạo settings TRƯỚC TIÊN
  settings_init(); // <-- THÊM DÒNG NÀY

  dht.begin();
  pinMode(LED_PIN, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(HEATER_PIN, OUTPUT);
  digitalWrite(HEATER_PIN, LOW);

  pinMode(PUMP_PIN, OUTPUT);
  digitalWrite(PUMP_PIN, LOW);

  setup_neopixel();
  setup_servo();
  setup_wifi();
  tb_init(mqttClient);
}

void loop()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Mất kết nối WiFi!  Đang kết nối lại...");
    setup_wifi();
    return;
  }

  if (!mqttClient.connected())
  {
    tb_reconnect(mqttClient);
  }

  mqttClient.loop();

  long now = millis();
  if (now - lastSend > 5000)
  {
    lastSend = now;

    // Đọc cảm biến với retry (chống nhiễu từ máy bơm)
    float h, t;
    bool dhtOK = readDHT_withRetry(t, h, 3);
    float distance = getDistance_cm_withRetry(3);

    // Tính mực nước thực tế từ khoảng cách
    float waterLevel = settings_distance_to_water_level(distance);

    // --- LOGIC TỰ ĐỘNG BẬT SƯỞI (SỬ DỤNG NGƯỠNG ĐỘNG) ---
    if (!isnan(t))
    {
      if (t < currentSettings.heaterOnTemp)
      {
        setRelay(HEATER_PIN, true);  // Dùng hàm setRelay thay vì digitalWrite
        Serial.println("Nhiệt độ thấp!  Đã BẬT máy sưởi.");
      }
      else if (t > currentSettings.heaterOffTemp)
      {
        setRelay(HEATER_PIN, false);
        Serial.println("Nhiệt độ đạt yêu cầu.  Đã TẮT máy sưởi.");
      }
    }

    // --- LOGIC TỰ ĐỘNG BẬT MÁY BƠM (SỬ DỤNG NGƯỠNG ĐỘNG VÀ MỰC NƯỚC) ---
    if (distance > 0)
    {
      // So sánh MỰC NƯỚC (không phải khoảng cách)
      if (waterLevel < currentSettings.pumpOnLevel)
      {
        setRelay(PUMP_PIN, true);  // Dùng hàm setRelay
        Serial.print("Mực nước thấp (");
        Serial.print(waterLevel);
        Serial.println(" cm)! Đã BẬT máy bơm.");
      }
      else if (waterLevel > currentSettings.pumpOffLevel)
      {
        setRelay(PUMP_PIN, false);
        Serial.print("Mực nước đủ (");
        Serial.print(waterLevel);
        Serial.println(" cm). Đã TẮT máy bơm.");
      }
    }

    // In ra Serial để kiểm tra - GIẢI THÍCH:
    // - Khoảng cách: Từ cảm biến siêu âm đến mặt nước (cm)
    // - Mực nước: Độ cao thực của nước trong bể = tankHeight - khoảng cách
    Serial.println("--- ĐO MỰC NƯỚC ---");
    Serial.print("  Khoảng cách (cảm biến->nước): ");
    Serial.print(distance);
    Serial.println(" cm");
    Serial.print("  Độ cao bể (tankHeight):       ");
    Serial.print(currentSettings.tankHeight);
    Serial.println(" cm");
    Serial.print("  => Mực nước thực tế:          ");
    Serial.print(waterLevel);
    Serial.println(" cm");
    Serial.println("-------------------");

    // Gửi telemetry - GỬI MỰC NƯỚC THỰC TẾ (không phải khoảng cách)
    tb_send_telemetry(mqttClient, t, h, waterLevel);
  }
}