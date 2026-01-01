// src/thingsboard_manager.cpp

#include <ArduinoJson.h>
#include <PubSubClient.h>
#include "config.h"              // Lấy token, server, pin...
#include "thingsboard_manager.h" // Include chính nó
#include "neopixel_manager.h"
#include "servo_manager.h"
#include "settings_manager.h" // <-- THÊM DÒNG NÀY

/**
 * @brief Hàm xử lý lệnh RPC đến
 */
void tb_callback(char *topic, byte *payload, unsigned int length)
{
  Serial.println("--------------------");
  Serial.println("Có lệnh mới từ ThingsBoard!");
  Serial.print("Topic: ");
  Serial.println(topic);

  payload[length] = '\0';
  Serial.print("Message: ");
  Serial.println((char *)payload);

  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, payload);
  if (error)
  {
    Serial.print(F("deserializeJson() thất bại: "));
    Serial.println(error.c_str());
    return;
  }

  const char *method = doc["method"];
  if (method != NULL && strcmp(method, "setLight") == 0)
  {
    bool status = doc["params"];
    Serial.print("Lệnh 'setLight' nhận được: ");
    Serial.println(status);

    digitalWrite(LED_PIN, status ? HIGH : LOW);
    Serial.println(status ? "Đã BẬT đèn LED" : "Đã TẮT đèn LED");
  }

  // --- THÊM XỬ LÝ LỆNH 'setColor' (NeoPixel) ---
  if (strcmp(method, "setColor") == 0)
  {
    const char *colorHex = doc["params"]; // Nhận chuỗi màu, ví dụ "#FF0000"

    if (colorHex != NULL)
    {
      Serial.print("Lệnh 'setColor' nhận được: ");
      Serial.println(colorHex);
      neopixel_set_color_hex(colorHex); // Gọi hàm mới
    }
  }

  // --- THÊM XỬ LÝ LỆNH 'setBrightness' ---
  if (strcmp(method, "setBrightness") == 0)
  {
    int brightness = doc["params"]; // Nhận giá trị số (0-255)
    Serial.print("Lệnh 'setBrightness' nhận được: ");
    Serial.println(brightness);

    // Gọi hàm mới, ép kiểu về byte
    neopixel_set_brightness((uint8_t)brightness);
  }

  // --- THÊM XỬ LÝ LỆNH 'feedFish' ---
  if (strcmp(method, "feedFish") == 0)
  {
    Serial.println("Nhận lệnh cho cá ăn!");
    feed_fish(); // Gọi hàm quay servo
  }

  // --- XỬ LÝ LỆNH 'setThresholds' (MỚI) ---
  if (strcmp(method, "setThresholds") == 0)
  {
    const char *params = doc["params"];
    if (params != NULL)
    {
      Serial.println("Nhận lệnh 'setThresholds':");
      Serial.println(params);
      settings_update_from_json(params);
    }
  }

  Serial.println("--------------------");
  Serial.println("--------------------");
}

/**
 * @brief Khởi tạo ThingsBoard client
 */
void tb_init(PubSubClient &client)
{
  client.setServer(TB_SERVER, TB_PORT);
  client.setCallback(tb_callback); // Gọi hàm callback ở trên
}

/**
 * @brief Kết nối lại MQTT
 */
void tb_reconnect(PubSubClient &client)
{
  while (!client.connected())
  {
    Serial.print("Đang cố gắng kết nối MQTT tới ThingsBoard...");
    if (client.connect("ESP32_DHT11_Device", TB_TOKEN, NULL))
    {
      Serial.println("Đã kết nối!");
      client.subscribe("v1/devices/me/rpc/request/+");
      Serial.println("Đã đăng ký nhận lệnh RPC.");
    }
    else
    {
      Serial.print("Lỗi, rc=");
      Serial.print(client.state());
      Serial.println(" Thử lại sau 5 giây");
      delay(5000);
    }
  }
}

/**
 * @brief Gửi dữ liệu (telemetry)
 */
// src/thingsboard_manager.cpp
// ... (Hàm tb_callback, tb_init, tb_reconnect giữ nguyên) ...

/**
 * @brief Gửi dữ liệu (telemetry)
 * @param waterLevel: Mực nước thực tế (cm) = tankHeight - khoảng cách đo được
 */
void tb_send_telemetry(PubSubClient &client, float temp, float humid, float waterLevel)
{
  if (isnan(humid) || isnan(temp))
  {
    Serial.println("Lỗi đọc dữ liệu DHT11!");
    // Vẫn gửi waterLevel ngay cả khi DHT lỗi
  }

  Serial.print("[ThingsBoard] Gửi: Nhiệt độ=");
  Serial.print(temp);
  Serial.print("°C, Độ ẩm=");
  Serial.print(humid);
  Serial.print("%, Mực nước=");
  Serial.print(waterLevel);
  Serial.println(" cm");

  StaticJsonDocument<200> doc;
  doc["nhietDo"] = temp;
  doc["doAm"] = humid;

  // Gửi mực nước thực tế (chỉ gửi nếu đọc thành công)
  if (waterLevel >= 0)
  {
    doc["mucNuoc_cm"] = waterLevel;
  }

  char jsonBuffer[200];
  serializeJson(doc, jsonBuffer);

  client.publish("v1/devices/me/telemetry", jsonBuffer);
  Serial.println("Đã gửi dữ liệu lên ThingsBoard.");
}