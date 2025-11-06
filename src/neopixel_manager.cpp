// src/neopixel_manager.cpp
#include <Adafruit_NeoPixel.h>
#include "config.h"
#include "neopixel_manager.h"

// Khởi tạo đối tượng NeoPixel
Adafruit_NeoPixel strip(NEOPIXEL_COUNT, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

void setup_neopixel() {
    strip.begin();           // Khởi tạo
    strip.show();            // Tắt hết LED khi khởi động
    strip.setBrightness(50); // QUAN TRỌNG: Đặt độ sáng thấp (0-255)
}

// Hàm helper để đặt màu cho tất cả LED
void neopixel_set_color(uint32_t color) {
    for(int i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, color);
    }
    strip.show(); // Hiển thị màu
}

// Hàm helper để chuyển đổi chuỗi Hex (ví dụ: "#FF0000") sang mã màu
void neopixel_set_color_hex(const char* hexString) {
    // Nếu có dấu '#', bỏ qua nó
    if (hexString[0] == '#') {
        hexString++;
    }

    // Chuyển đổi chuỗi Hex (cơ số 16) thành số long
    long colorValue = strtol(hexString, NULL, 16);
    
    // Đặt màu (ví dụ: màu đen #000000 sẽ tắt LED)
    neopixel_set_color((uint32_t)colorValue);
}