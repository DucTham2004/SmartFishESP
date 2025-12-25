// src/settings_manager.cpp

#include <Preferences.h>
#include <ArduinoJson.h>
#include "config.h"
#include "settings_manager.h"

Preferences preferences;
DeviceSettings currentSettings;

void settings_init()
{
    preferences.begin("smartfish", false); // Mở namespace "smartfish"
    settings_load();
}

void settings_load()
{
    // Đọc từ NVS (Non-Volatile Storage), nếu chưa có thì dùng giá trị mặc định
    currentSettings.tankHeight = preferences.getFloat("tankHeight", DEFAULT_TANK_HEIGHT);
    currentSettings.heaterOnTemp = preferences.getFloat("heaterOn", DEFAULT_TEMP_THRESHOLD_ON);
    currentSettings.heaterOffTemp = preferences.getFloat("heaterOff", DEFAULT_TEMP_THRESHOLD_OFF);
    currentSettings.pumpOnLevel = preferences.getFloat("pumpOn", DEFAULT_WATER_LEVEL_ON);
    currentSettings.pumpOffLevel = preferences.getFloat("pumpOff", DEFAULT_WATER_LEVEL_OFF);

    Serial.println("=== Đã tải cài đặt ===");
    Serial.print("Độ cao bể:  ");
    Serial.println(currentSettings.tankHeight);
    Serial.print("Sưởi ON: ");
    Serial.println(currentSettings.heaterOnTemp);
    Serial.print("Sưởi OFF: ");
    Serial.println(currentSettings.heaterOffTemp);
    Serial.print("Bơm ON (mực nước): ");
    Serial.println(currentSettings.pumpOnLevel);
    Serial.print("Bơm OFF (mực nước): ");
    Serial.println(currentSettings.pumpOffLevel);
}

void settings_save()
{
    preferences.putFloat("tankHeight", currentSettings.tankHeight);
    preferences.putFloat("heaterOn", currentSettings.heaterOnTemp);
    preferences.putFloat("heaterOff", currentSettings.heaterOffTemp);
    preferences.putFloat("pumpOn", currentSettings.pumpOnLevel);
    preferences.putFloat("pumpOff", currentSettings.pumpOffLevel);

    Serial.println("=== Đã lưu cài đặt vào bộ nhớ!  ===");
}

void settings_update_from_json(const char *jsonParams)
{
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, jsonParams);

    if (error)
    {
        Serial.print("Lỗi parse JSON: ");
        Serial.println(error.c_str());
        return;
    }

    // Cập nhật các giá trị từ JSON
    if (doc.containsKey("tankHeight"))
    {
        currentSettings.tankHeight = doc["tankHeight"].as<float>();
    }
    if (doc.containsKey("heaterOn"))
    {
        currentSettings.heaterOnTemp = doc["heaterOn"].as<float>();
    }
    if (doc.containsKey("heaterOff"))
    {
        currentSettings.heaterOffTemp = doc["heaterOff"].as<float>();
    }
    if (doc.containsKey("pumpOn"))
    {
        currentSettings.pumpOnLevel = doc["pumpOn"].as<float>();
    }
    if (doc.containsKey("pumpOff"))
    {
        currentSettings.pumpOffLevel = doc["pumpOff"].as<float>();
    }

    // Lưu vào bộ nhớ
    settings_save();

    Serial.println("=== Đã cập nhật cài đặt từ App ===");
}

// Chuyển đổi khoảng cách (từ cảm biến) sang mực nước thực tế
float settings_distance_to_water_level(float distance)
{
    // Mực nước = Độ cao bể - Khoảng cách đo được
    float waterLevel = currentSettings.tankHeight - distance;
    if (waterLevel < 0)
        waterLevel = 0;
    return waterLevel;
}