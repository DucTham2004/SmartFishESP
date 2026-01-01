// src/config.h

#pragma once

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
#define NEOPIXEL_PIN 26
#define NEOPIXEL_COUNT 12

// THÊM CẤU HÌNH SERVO
#define SERVO_PIN 13

// --- CẤU HÌNH MÁY SƯỞI ---
#define HEATER_PIN 5

// --- CẤU HÌNH MÁY BƠM ---
#define PUMP_PIN 25

// --- GIÁ TRỊ MẶC ĐỊNH CHO NGƯỠNG (sẽ được ghi đè bởi giá trị từ EEPROM) ---
#define DEFAULT_TANK_HEIGHT 30.0f
#define DEFAULT_TEMP_THRESHOLD_ON 28.5f
#define DEFAULT_TEMP_THRESHOLD_OFF 29.0f
#define DEFAULT_WATER_LEVEL_ON 10.0f
#define DEFAULT_WATER_LEVEL_OFF 20.0f