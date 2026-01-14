# 🐟 SmartFish - Hệ Thống Bể Cá Thông Minh IoT

<div align="center">

![ESP32](https://img.shields.io/badge/ESP32-DevKit%20V1-blue?style=for-the-badge&logo=espressif)
![PlatformIO](https://img.shields.io/badge/PlatformIO-IDE-orange?style=for-the-badge&logo=platformio)
![ThingsBoard](https://img.shields.io/badge/ThingsBoard-IoT%20Platform-green?style=for-the-badge)
![Arduino](https://img.shields.io/badge/Framework-Arduino-teal?style=for-the-badge&logo=arduino)

**Hệ thống giám sát và điều khiển bể cá tự động sử dụng ESP32 và ThingsBoard Cloud**

</div>

---

## 📋 Mục Lục

- [Giới Thiệu](#-giới-thiệu)
- [Tính Năng](#-tính-năng)
- [Phần Cứng Yêu Cầu](#-phần-cứng-yêu-cầu)
- [Sơ Đồ Kết Nối](#-sơ-đồ-kết-nối)
- [Cài Đặt](#-cài-đặt)
- [Cấu Hình](#-cấu-hình)
- [Cấu Trúc Dự Án](#-cấu-trúc-dự-án)
- [API & Lệnh RPC](#-api--lệnh-rpc)
- [Cách Hoạt Động](#-cách-hoạt-động)
- [Khắc Phục Sự Cố](#-khắc-phục-sự-cố)
- [Đóng Góp](#-đóng-góp)
- [Giấy Phép](#-giấy-phép)

---

## 🎯 Giới Thiệu

**SmartFish** là một dự án IoT hoàn chỉnh giúp bạn giám sát và điều khiển bể cá từ xa thông qua nền tảng ThingsBoard Cloud. Hệ thống sử dụng vi điều khiển ESP32 kết hợp với các cảm biến để:

- Theo dõi nhiệt độ và độ ẩm môi trường
- Giám sát mực nước trong bể
- Tự động bật/tắt máy sưởi và máy bơm
- Điều khiển đèn LED NeoPixel trang trí
- Cho cá ăn tự động qua servo motor

---

## ✨ Tính Năng

### 🌡️ Giám Sát Môi Trường
- **Cảm biến DHT11**: Đo nhiệt độ và độ ẩm không khí
- **Cảm biến siêu âm HC-SR04**: Đo mực nước trong bể
- Gửi dữ liệu telemetry lên ThingsBoard mỗi 5 giây

### 🔄 Điều Khiển Tự Động
- **Máy sưởi (Heater)**: Tự động bật khi nhiệt độ thấp hơn ngưỡng, tắt khi đủ ấm
- **Máy bơm (Pump)**: Tự động bơm nước khi mực nước thấp, tắt khi đủ nước
- Ngưỡng có thể điều chỉnh qua app/dashboard

### 💡 Điều Khiển Đèn
- **LED onboard**: Bật/tắt qua lệnh RPC
- **NeoPixel Ring (12 LED)**: 
  - Đổi màu theo mã HEX
  - Điều chỉnh độ sáng (0-255)

### 🍽️ Cho Cá Ăn
- **Servo motor**: Điều khiển cửa xả thức ăn
- Kích hoạt qua lệnh RPC từ app

### ⚙️ Cài Đặt Động
- Lưu cấu hình vào bộ nhớ NVS (Non-Volatile Storage)
- Cập nhật ngưỡng từ xa qua JSON
- Không mất cài đặt khi mất điện

### 🛡️ Chống Nhiễu Cảm Biến
- Retry tự động khi đọc cảm biến thất bại
- Cache giá trị hợp lệ gần nhất
- Chờ relay ổn định trước khi đọc cảm biến

---

## 🔧 Phần Cứng Yêu Cầu

| Linh Kiện | Số Lượng | Mô Tả |
|-----------|----------|-------|
| ESP32 DevKit V1 | 1 | Vi điều khiển chính |
| DHT11 | 1 | Cảm biến nhiệt độ & độ ẩm |
| HC-SR04 | 1 | Cảm biến siêu âm đo khoảng cách |
| NeoPixel Ring | 1 | Vòng LED WS2812B (12 LED) |
| Servo Motor | 1 | Điều khiển cửa xả thức ăn |
| Relay Module | 2 | Điều khiển máy sưởi & máy bơm |
| LED | 1 | LED onboard (GPIO 2) |
| Dây nối | - | Dây Dupont các loại |
| Nguồn 5V | 1 | Cấp nguồn cho hệ thống |

---

## 📌 Sơ Đồ Kết Nối

```
ESP32 DevKit V1 - Sơ Đồ Chân

┌─────────────────────────────────────────┐
│                 ESP32                    │
├─────────────────────────────────────────┤
│                                          │
│  GPIO 2  ──────────► LED Onboard        │
│  GPIO 4  ──────────► DHT11 (Data)       │
│  GPIO 5  ──────────► Relay Máy Sưởi     │
│  GPIO 12 ──────────► HC-SR04 (Trig)     │
│  GPIO 13 ──────────► Servo Motor        │
│  GPIO 14 ──────────► HC-SR04 (Echo)     │
│  GPIO 25 ──────────► Relay Máy Bơm      │
│  GPIO 26 ──────────► NeoPixel (DIN)     │
│                                          │
│  3.3V    ──────────► DHT11 (VCC)        │
│  5V      ──────────► HC-SR04, NeoPixel  │
│  GND     ──────────► Tất cả GND         │
│                                          │
└─────────────────────────────────────────┘
```

### Chi Tiết Kết Nối

#### DHT11
```
DHT11          ESP32
─────          ─────
VCC    ───►    3.3V
GND    ───►    GND
DATA   ───►    GPIO 4
```

#### HC-SR04
```
HC-SR04        ESP32
───────        ─────
VCC    ───►    5V
GND    ───►    GND
Trig   ───►    GPIO 12
Echo   ───►    GPIO 14
```

#### NeoPixel Ring
```
NeoPixel       ESP32
────────       ─────
VCC    ───►    5V
GND    ───►    GND
DIN    ───►    GPIO 26
```

#### Servo Motor
```
Servo          ESP32
─────          ─────
VCC    ───►    5V
GND    ───►    GND
Signal ───►    GPIO 13
```

---

## 🚀 Cài Đặt

### Yêu Cầu Phần Mềm
- [Visual Studio Code](https://code.visualstudio.com/)
- [PlatformIO IDE Extension](https://platformio.org/platformio-ide)
- Tài khoản [ThingsBoard Cloud](https://thingsboard.cloud/) (hoặc server tự host)

### Các Bước Cài Đặt

#### 1. Clone Repository
```bash
git clone https://github.com/your-username/SmartFish.git
cd SmartFish
```

#### 2. Mở Dự Án
- Mở VS Code
- Chọn **File → Open Folder** và chọn thư mục `SmartFish`
- PlatformIO sẽ tự động cài đặt các thư viện cần thiết

#### 3. Cấu Hình WiFi & ThingsBoard
Mở file `src/config.h` và cập nhật thông tin:

```cpp
// --- CẤU HÌNH WIFI ---
#define WIFI_SSID "TEN_WIFI_CUA_BAN"
#define WIFI_PASSWORD "MAT_KHAU_WIFI"

// --- CẤU HÌNH THINGSBOARD ---
#define TB_SERVER "eu.thingsboard.cloud"  // hoặc server của bạn
#define TB_TOKEN "ACCESS_TOKEN_THIET_BI"
#define TB_PORT 1883
```

#### 4. Upload Code
- Kết nối ESP32 với máy tính qua USB
- Nhấn **PlatformIO: Upload** (hoặc Ctrl+Alt+U)
- Mở Serial Monitor để xem log (115200 baud)

---

## ⚙️ Cấu Hình

### File `config.h` - Tất cả cấu hình tập trung

```cpp
// --- CẤU HÌNH WIFI ---
#define WIFI_SSID "your_wifi_ssid"
#define WIFI_PASSWORD "your_wifi_password"

// --- CẤU HÌNH THINGSBOARD ---
#define TB_SERVER "eu.thingsboard.cloud"
#define TB_TOKEN "your_device_token"
#define TB_PORT 1883

// --- CẤU HÌNH PIN ---
#define DHTPIN 4              // Chân cảm biến DHT11
#define LED_PIN 2             // LED onboard
#define TRIG_PIN 12           // HC-SR04 Trigger
#define ECHO_PIN 14           // HC-SR04 Echo
#define NEOPIXEL_PIN 26       // NeoPixel Data
#define NEOPIXEL_COUNT 12     // Số LED trong vòng
#define SERVO_PIN 13          // Servo motor
#define HEATER_PIN 5          // Relay máy sưởi
#define PUMP_PIN 25           // Relay máy bơm

// --- GIÁ TRỊ MẶC ĐỊNH ---
#define DEFAULT_TANK_HEIGHT 30.0f      // Độ cao bể (cm)
#define DEFAULT_TEMP_THRESHOLD_ON 28.5f   // Bật sưởi khi < 28.5°C
#define DEFAULT_TEMP_THRESHOLD_OFF 29.0f  // Tắt sưởi khi > 29°C
#define DEFAULT_WATER_LEVEL_ON 10.0f      // Bật bơm khi mực nước < 10cm
#define DEFAULT_WATER_LEVEL_OFF 20.0f     // Tắt bơm khi mực nước > 20cm
```

---

## 📁 Cấu Trúc Dự Án

```
SmartFish/
├── platformio.ini          # Cấu hình PlatformIO & thư viện
├── README.md               # Tài liệu dự án (file này)
├── include/
│   └── README              # Hướng dẫn thư mục include
├── lib/
│   └── README              # Hướng dẫn thư viện riêng
├── src/
│   ├── config.h            # ⚙️ Tất cả cấu hình (WiFi, Pin, Ngưỡng)
│   ├── main.cpp            # 🚀 Chương trình chính
│   ├── wifi_manager.cpp    # 📶 Quản lý kết nối WiFi
│   ├── wifi_manager.h
│   ├── thingsboard_manager.cpp  # ☁️ Giao tiếp ThingsBoard (MQTT)
│   ├── thingsboard_manager.h
│   ├── neopixel_manager.cpp     # 💡 Điều khiển đèn NeoPixel
│   ├── neopixel_manager.h
│   ├── servo_manager.cpp        # 🍽️ Điều khiển servo cho cá ăn
│   ├── servo_manager.h
│   ├── settings_manager.cpp     # 💾 Quản lý cài đặt (NVS)
│   └── settings_manager.h
└── test/
    └── README              # Hướng dẫn test
```

### Mô Tả Các Module

| Module | Chức Năng |
|--------|-----------|
| `main.cpp` | Khởi tạo hệ thống, vòng lặp chính, đọc cảm biến, logic tự động |
| `wifi_manager` | Kết nối và quản lý WiFi |
| `thingsboard_manager` | Giao tiếp MQTT, gửi telemetry, nhận lệnh RPC |
| `neopixel_manager` | Điều khiển dải LED NeoPixel (màu sắc, độ sáng) |
| `servo_manager` | Điều khiển servo motor cho cá ăn |
| `settings_manager` | Lưu/đọc cài đặt từ NVS, cập nhật ngưỡng động |

---

## 📡 API & Lệnh RPC

### Dữ Liệu Telemetry (Gửi lên ThingsBoard)

```json
{
  "nhietDo": 28.5,      // Nhiệt độ (°C)
  "doAm": 65.0,         // Độ ẩm (%)
  "mucNuoc_cm": 15.0    // Mực nước thực tế (cm)
}
```

### Lệnh RPC (Nhận từ ThingsBoard)

#### 1. Bật/Tắt LED
```json
{
  "method": "setLight",
  "params": true   // true = bật, false = tắt
}
```

#### 2. Đổi Màu NeoPixel
```json
{
  "method": "setColor",
  "params": "#FF0000"   // Mã màu HEX (ví dụ: đỏ)
}
```

#### 3. Điều Chỉnh Độ Sáng
```json
{
  "method": "setBrightness",
  "params": 128   // Giá trị 0-255
}
```

#### 4. Cho Cá Ăn
```json
{
  "method": "feedFish",
  "params": null
}
```

#### 5. Cập Nhật Ngưỡng
```json
{
  "method": "setThresholds",
  "params": "{\"tankHeight\":30,\"heaterOn\":28.5,\"heaterOff\":29,\"pumpOn\":10,\"pumpOff\":20}"
}
```

---

## 🔄 Cách Hoạt Động

### Luồng Hoạt Động Chính

```
┌─────────────────────────────────────────────────────────────┐
│                         KHỞI ĐỘNG                            │
├─────────────────────────────────────────────────────────────┤
│  1. Khởi tạo Serial (115200 baud)                           │
│  2. Đọc cài đặt từ NVS (settings_init)                      │
│  3. Khởi tạo cảm biến DHT11                                 │
│  4. Cấu hình các chân GPIO                                  │
│  5. Khởi tạo NeoPixel & Servo                               │
│  6. Kết nối WiFi                                            │
│  7. Kết nối MQTT đến ThingsBoard                            │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                      VÒNG LẶP CHÍNH                          │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────┐    │
│  │ Kiểm tra kết nối WiFi → Kết nối lại nếu mất        │    │
│  └─────────────────────────────────────────────────────┘    │
│                              │                               │
│                              ▼                               │
│  ┌─────────────────────────────────────────────────────┐    │
│  │ Kiểm tra kết nối MQTT → Kết nối lại nếu mất        │    │
│  └─────────────────────────────────────────────────────┘    │
│                              │                               │
│                              ▼                               │
│  ┌─────────────────────────────────────────────────────┐    │
│  │ Xử lý message MQTT (mqttClient.loop())              │    │
│  └─────────────────────────────────────────────────────┘    │
│                              │                               │
│                              ▼                               │
│  ┌─────────────────────────────────────────────────────┐    │
│  │ Mỗi 5 giây:                                         │    │
│  │  • Đọc nhiệt độ & độ ẩm (DHT11)                     │    │
│  │  • Đo khoảng cách (HC-SR04)                         │    │
│  │  • Tính mực nước = tankHeight - khoảng cách         │    │
│  │  • Logic tự động bật/tắt máy sưởi                   │    │
│  │  • Logic tự động bật/tắt máy bơm                    │    │
│  │  • Gửi telemetry lên ThingsBoard                    │    │
│  └─────────────────────────────────────────────────────┘    │
└─────────────────────────────────────────────────────────────┘
```

### Logic Điều Khiển Tự Động

#### Máy Sưởi (Heater)
```
Nếu nhiệt độ < heaterOnTemp (mặc định 28.5°C):
    → BẬT máy sưởi

Nếu nhiệt độ > heaterOffTemp (mặc định 29.0°C):
    → TẮT máy sưởi
```

#### Máy Bơm (Pump)
```
Nếu mực nước < pumpOnLevel (mặc định 10cm):
    → BẬT máy bơm

Nếu mực nước > pumpOffLevel (mặc định 20cm):
    → TẮT máy bơm
```

---

## 🐛 Khắc Phục Sự Cố

### Lỗi Thường Gặp

| Lỗi | Nguyên Nhân | Giải Pháp |
|-----|-------------|-----------|
| `WiFi không kết nối được` | Sai SSID/Password | Kiểm tra `config.h` |
| `MQTT connection failed` | Sai Token/Server | Kiểm tra token trên ThingsBoard |
| `DHT11 đọc NaN` | Nhiễu từ relay, dây lỏng | Hệ thống tự retry & dùng cache |
| `HC-SR04 trả về -1` | Khoảng cách quá xa/gần | Kiểm tra vị trí cảm biến |
| `Servo bị rung` | PWM không ổn định | Servo được detach sau khi dùng |
| `NeoPixel không sáng` | Thiếu nguồn 5V, sai chân | Kiểm tra kết nối nguồn |

### Debug Tips

1. **Mở Serial Monitor** (115200 baud) để xem log chi tiết
2. **Kiểm tra IP** sau khi kết nối WiFi
3. **Xem Dashboard ThingsBoard** để kiểm tra dữ liệu telemetry
4. **Test RPC** bằng cách gửi lệnh từ ThingsBoard

---

## 📚 Thư Viện Sử Dụng

| Thư Viện | Version | Mục Đích |
|----------|---------|----------|
| `Adafruit Unified Sensor` | ^1.1.9 | Base cho các cảm biến Adafruit |
| `DHT sensor library` | ^1.4.6 | Đọc cảm biến DHT11/DHT22 |
| `PubSubClient` | ^2.8 | Giao thức MQTT |
| `ArduinoJson` | ^6.21.4 | Parse/Generate JSON |
| `Adafruit NeoPixel` | ^1.12.0 | Điều khiển LED WS2812B |
| `ESP32Servo` | ^3.0.5 | Điều khiển Servo trên ESP32 |

---

## 🤝 Đóng Góp

Mọi đóng góp đều được chào đón! Vui lòng:

1. Fork repository
2. Tạo branch mới (`git checkout -b feature/AmazingFeature`)
3. Commit thay đổi (`git commit -m 'Add some AmazingFeature'`)
4. Push lên branch (`git push origin feature/AmazingFeature`)
5. Tạo Pull Request

---

## 📄 Giấy Phép

Dự án này được phân phối dưới giấy phép MIT. Xem file `LICENSE` để biết thêm chi tiết.

---

## 📞 Liên Hệ

- **Tác giả**: [Tên của bạn]
- **Email**: [email@example.com]
- **GitHub**: [https://github.com/your-username](https://github.com/your-username)

---

<div align="center">

**⭐ Nếu dự án hữu ích, hãy cho một sao nhé! ⭐**

Made with ❤️ for 🐟

</div>
