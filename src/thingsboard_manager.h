// src/thingsboard_manager.h

#pragma once
#include <PubSubClient.h> // Cần include ở đây

// Khai báo các hàm sẽ được dùng bên main.cpp
void tb_init(PubSubClient& client);
void tb_reconnect(PubSubClient& client);
void tb_send_telemetry(PubSubClient& client, float temp, float humid, float distance); // <-- SỬA Ở ĐÂY
void tb_callback(char* topic, byte* payload, unsigned int length);