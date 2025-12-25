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

    float h = dht.readHumidity();
    float t = dht.readTemperature();
    float distance = getDistance_cm();

    // Tính mực nước thực tế từ khoảng cách
    float waterLevel = settings_distance_to_water_level(distance);

    // --- LOGIC TỰ ĐỘNG BẬT SƯỞI (SỬ DỤNG NGƯỠNG ĐỘNG) ---
    if (!isnan(t))
    {
      if (t < currentSettings.heaterOnTemp) // <-- SỬ DỤNG BIẾN
      {
        digitalWrite(HEATER_PIN, HIGH);
        Serial.println("Nhiệt độ thấp!  Đã BẬT máy sưởi.");
      }
      else if (t > currentSettings.heaterOffTemp) // <-- SỬ DỤNG BIẾN
      {
        digitalWrite(HEATER_PIN, LOW);
        Serial.println("Nhiệt độ đạt yêu cầu.  Đã TẮT máy sưởi.");
      }
    }

    // --- LOGIC TỰ ĐỘNG BẬT MÁY BƠM (SỬ DỤNG NGƯỠNG ĐỘNG VÀ MỰC NƯỚC) ---
    if (distance > 0)
    {
      // So sánh MỰC NƯỚC (không phải khoảng cách)
      if (waterLevel < currentSettings.pumpOnLevel) // <-- Mực nước thấp
      {
        digitalWrite(PUMP_PIN, HIGH);
        Serial.print("Mực nước thấp (");
        Serial.print(waterLevel);
        Serial.println(" cm)! Đã BẬT máy bơm.");
      }
      else if (waterLevel > currentSettings.pumpOffLevel) // <-- Mực nước đủ
      {
        digitalWrite(PUMP_PIN, LOW);
        Serial.print("Mực nước đủ (");
        Serial.print(waterLevel);
        Serial.println(" cm). Đã TẮT máy bơm.");
      }
    }

    // In ra Serial để kiểm tra
    Serial.print("Khoảng cách: ");
    Serial.print(distance);
    Serial.print(" cm, Mực nước: ");
    Serial.print(waterLevel);
    Serial.println(" cm");

    // Gửi telemetry (bạn có thể thêm waterLevel vào)
    tb_send_telemetry(mqttClient, t, h, waterLevel); // <-- Gửi mực nước thay vì khoảng cách
  }
}