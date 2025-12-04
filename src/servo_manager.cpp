#include <ESP32Servo.h>
#include "config.h"
#include "servo_manager.h"

Servo fishServo;
const int POS_CLOSE = 0;  // Góc đóng
const int POS_OPEN = 360; // Góc mở để thức ăn rơi xuống

void setup_servo()
{
    // Chưa cần attach ngay để tiết kiệm năng lượng và tránh rung
    // fishServo.setPeriodHertz(50);
}

void feed_fish()
{
    Serial.println("Đang cho cá ăn...");

    fishServo.attach(SERVO_PIN); // Kết nối servo

    // Mở cửa xả thức ăn
    fishServo.write(POS_OPEN);
    delay(1000); // Giữ trong 1 giây (hoặc chỉnh tùy lượng thức ăn)

    // Đóng cửa xả
    fishServo.write(POS_CLOSE);
    delay(500); // Đợi servo quay về

    fishServo.detach(); // Ngắt kết nối để servo không bị rung/nóng
    Serial.println("Đã cho ăn xong!");
}