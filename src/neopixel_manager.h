// src/neopixel_manager.h
#pragma once
#include <Arduino.h>

void setup_neopixel();
void neopixel_set_color_hex(const char* hexString);
void neopixel_set_brightness(uint8_t brightness); // <-- THÊM DÒNG NÀY