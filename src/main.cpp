// src/main.cpp

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// Include các file custom của chúng ta
#include "config.h"
#include "wifi_manager.h"
#include "thingsboard_manager.h"
#include "neopixel_manager.h"
#include "servo_manager.h"

// --- KHỞI TẠO CÁC ĐỐI TƯỢNG TOÀN CỤC ---
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
DHT dht(DHTPIN, DHTTYPE);

long lastSend = 0;

float getDistance_cm()
{
  // Phát một xung 10 micro-giây
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Đọc thời gian của xung phản hồi (tính bằng micro-giây)
  // pulseIn sẽ chờ tối đa 1 giây (1,000,000 us)
  long duration = pulseIn(ECHO_PIN, HIGH, 1000000);

  // Tính toán khoảng cách (cm)
  // Tốc độ âm thanh = 343 m/s = 0.0343 cm/us
  // Khoảng cách = (duration * 0.0343) / 2  (Chia 2 vì sóng đi và về)
  float distance_cm = (duration * 0.0343) / 2.0;

  if (distance_cm == 0 || distance_cm > 400)
  {
    // Nếu đọc lỗi (quá gần hoặc quá xa)
    return -1;
  }
  else
  {
    return distance_cm;
  }
}

void setup()
{
  Serial.begin(115200);
  dht.begin();
  pinMode(LED_PIN, OUTPUT);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  // Cấu hình chân điều khiển máy sưởi
  pinMode(HEATER_PIN, OUTPUT);
  digitalWrite(HEATER_PIN, LOW); // Mặc định tắt

  // Cấu hình chân điều khiển máy bơm nước
  pinMode(PUMP_PIN, OUTPUT);
  digitalWrite(PUMP_PIN, LOW); // Mặc định tắt

  setup_neopixel();
  setup_servo();

  setup_wifi(); // <-- Gọi hàm từ file wifi_manager

  tb_init(mqttClient); // <-- Gọi hàm từ file thingsboard_manager
}

void loop()
{
  // 1. Kiểm tra WiFi
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Mất kết nối WiFi! Đang kết nối lại...");
    setup_wifi();
    return;
  }

  // 2. Kiểm tra MQTT
  if (!mqttClient.connected())
  {
    tb_reconnect(mqttClient); // <-- Gọi hàm
  }

  // 3. Duy trì MQTT
  mqttClient.loop();

  // 4. Gửi dữ liệu
  long now = millis();
  if (now - lastSend > 5000)
  {
    lastSend = now;

    float h = dht.readHumidity();
    float t = dht.readTemperature();
    float distance = getDistance_cm(); // <-- LẤY DỮ LIỆU MỚI

    // --- LOGIC TỰ ĐỘNG BẬT SƯỞI ---
    if (!isnan(t))
    { // Chỉ xử lý nếu đọc được nhiệt độ hợp lệ
      if (t < TEMP_THRESHOLD_ON)
      {
        digitalWrite(HEATER_PIN, HIGH); // Bật Relay/LED
        Serial.println("Nhiệt độ thấp! Đã BẬT máy sưởi.");
      }
      else if (t > TEMP_THRESHOLD_OFF)
      {
        digitalWrite(HEATER_PIN, LOW); // Tắt Relay/LED
        Serial.println("Nhiệt độ đạt yêu cầu. Đã TẮT máy sưởi.");
      }
    }

    // --- LOGIC TỰ ĐỘNG BẬT MÁY BƠM NƯỚC ---
    // Khoảng cách càng lớn = mực nước càng thấp
    if (distance > 0)
    { // Chỉ xử lý nếu đọc được khoảng cách hợp lệ
      if (distance > WATER_LEVEL_THRESHOLD_ON)
      {
        digitalWrite(PUMP_PIN, HIGH); // Bật máy bơm
        Serial.println("Mực nước thấp! Đã BẬT máy bơm.");
      }
      else if (distance < WATER_LEVEL_THRESHOLD_OFF)
      {
        digitalWrite(PUMP_PIN, LOW); // Tắt máy bơm
        Serial.println("Mực nước đủ. Đã TẮT máy bơm.");
      }
    }

    // In ra Serial để kiểm tra
    Serial.print("Khoảng cách: ");
    Serial.println(distance);

    // Gửi cả 3 dữ liệu đi
    tb_send_telemetry(mqttClient, t, h, distance); // <-- SỬA HÀM NÀY
  }
}