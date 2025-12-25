// src/settings_manager.h

#pragma once
#include <Arduino.h>

// Struct chứa các cài đặt
struct DeviceSettings
{
    float tankHeight;    // Độ cao bể cá (cm)
    float heaterOnTemp;  // Ngưỡng bật máy sưởi (°C)
    float heaterOffTemp; // Ngưỡng tắt máy sưởi (°C)
    float pumpOnLevel;   // Ngưỡng bật bơm (cm mực nước)
    float pumpOffLevel;  // Ngưỡng tắt bơm (cm mực nước)
};

// Biến toàn cục chứa cài đặt hiện tại
extern DeviceSettings currentSettings;

// Khai báo hàm
void settings_init();
void settings_save();
void settings_load();
void settings_update_from_json(const char *jsonParams);
float settings_distance_to_water_level(float distance);