// src/config.h

#pragma once // Đảm bảo file này chỉ được include 1 lần

// --- CẤU HÌNH WIFI ---
#define WIFI_SSID "ductham2004"
#define WIFI_PASSWORD "ductham2004"

// --- CẤU HÌNH THINGSBOARD ---
#define TB_SERVER "eu.thingsboard.cloud"
#define TB_TOKEN "oU0vQ2FnDV0Tq05Lkdrl"
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