// src/config.h

#pragma once // Đảm bảo file này chỉ được include 1 lần

// --- CẤU HÌNH WIFI ---
#define WIFI_SSID "ductham2004"
#define WIFI_PASSWORD "ductham2004"

// --- CẤU HÌNH THINGSBOARD ---
#define TB_SERVER "eu.thingsboard.cloud"
#define TB_TOKEN "f4ChAIxFLvvUKETpTgxi"
#define TB_PORT 1883

// --- CẤU HÌNH CẢM BIẾN & PIN ---
#define DHTPIN 4
#define DHTTYPE DHT11
#define LED_PIN 2
#define TRIG_PIN 12
#define ECHO_PIN 14

// THÊM CẤU HÌNH NEOPIXEL
#define NEOPIXEL_PIN 27   // Chân DATA nối với DIN
#define NEOPIXEL_COUNT 12 // Số lượng LED (bạn có 12)

// THÊM CẤU HÌNH SERVO
#define SERVO_PIN 13

// --- CẤU HÌNH MÁY SƯỞI (DEMO BẰNG LED/RELAY) ---
#define HEATER_PIN 5            // Chọn chân GPIO 5
#define TEMP_THRESHOLD_ON 28.5  // Nhiệt độ thấp hơn mức này sẽ BẬT sưởi
#define TEMP_THRESHOLD_OFF 29.0 // Nhiệt độ cao hơn mức này sẽ TẮT sưởi

// --- CẤU HÌNH MÁY BƠM NƯỚC ---
#define PUMP_PIN 25                   // Chân điều khiển relay máy bơm (GPIO34 chỉ input, dùng GPIO25 thay thế)
#define WATER_LEVEL_THRESHOLD_ON 10.0 // Khoảng cách (cm) > ngưỡng này = mực nước thấp -> BẬT bơm
#define WATER_LEVEL_THRESHOLD_OFF 5.0 // Khoảng cách (cm) < ngưỡng này = mực nước đủ -> TẮT bơm