<div align="center">

# 🏠 Smart Home IoT

**Hệ Thống Giám Sát & Điều Khiển Nhà Thông Minh**

![ESP32](https://img.shields.io/badge/ESP32-000000?style=for-the-badge&logo=espressif&logoColor=white)
![C++](https://img.shields.io/badge/C++-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![PlatformIO](https://img.shields.io/badge/PlatformIO-F58220?style=for-the-badge&logo=platformio&logoColor=white)
![MQTT](https://img.shields.io/badge/MQTT-3C5280?style=for-the-badge&logo=mqtt&logoColor=white)
![Node-RED](https://img.shields.io/badge/Node--RED-8F0000?style=for-the-badge&logo=node-red&logoColor=white)
![Wokwi](https://img.shields.io/badge/Wokwi-00979D?style=for-the-badge&logo=wokwi&logoColor=white)

</div>

---

## 📖 Giới thiệu dự án

**Smart Home IoT** là dự án mô phỏng hệ thống nhà thông minh toàn diện, cho phép người dùng giám sát môi trường sống và điều khiển các thiết bị điện từ xa thông qua giao diện Web Dashboard (Node-RED). Dự án sử dụng vi điều khiển ESP32, giao thức nhắn tin thời gian thực MQTT và được mô phỏng hoàn toàn bằng phần mềm Wokwi.

## ✨ Tính năng nổi bật

### 📊 1. Giám sát thời gian thực
- **Nhiệt độ & Độ ẩm:** Thu thập dữ liệu liên tục từ cảm biến DHT22.
- **Ánh sáng:** Theo dõi cường độ sáng môi trường qua quang trở (LDR).
- **Lưu trữ lịch sử:** Biểu đồ dạng đường (Line Chart) theo dõi biến thiên Nhiệt/Ẩm theo thời gian.

### 🕹️ 2. Điều khiển từ xa
- **Hệ thống chiếu sáng:** Bật/Tắt đèn LED trực tiếp từ xa thông qua công tắc trên Dashboard.
- **Rèm cửa/Cửa tự động:** Điều khiển mượt mà góc quay của động cơ Servo (0-180 độ) thông qua thanh trượt Slider.

### 🤖 3. Tự động hóa & Cảnh báo an toàn
- **Báo cháy/Quá nhiệt:** Khi nhiệt độ vượt ngưỡng an toàn (**>35°C**), hệ thống tự động:
  - Kích hoạt **Còi hú (Buzzer)** cảnh báo tại phần cứng.
  - Hiển thị **Popup (Toast)** cảnh báo khẩn cấp màu đỏ trên màn hình Dashboard của người dùng.

---

## 🛠️ Kiến trúc hệ thống

Dự án được chia thành 2 phần chính hoạt động độc lập và giao tiếp qua MQTT:

1. **Hardware Node (ESP32):**
   - Đọc dữ liệu từ DHT22, LDR.
   - Đóng gói dữ liệu dạng **JSON** và Publish lên MQTT Broker (`iot/myhome_999/sensor/data`).
   - Subscribe các topic điều khiển để thực thi lệnh (Bật/Tắt LED, Xoay Servo, Kích hoạt Còi).
2. **Dashboard Node (Node-RED):**
   - Subscribe dữ liệu JSON từ phần cứng để hiển thị lên Gauge và Chart.
   - Xử lý logic tự động hóa (Node Function) để tự động gửi lệnh kích hoạt còi khi nhiệt độ cao.
   - Chứa các thành phần UI để Publish lệnh điều khiển xuống ESP32.

---

## 📂 Cấu trúc thư mục

```text
IOT_4/
├── esp32/                  # Mã nguồn C++ cho ESP32
│   ├── src/
│   │   └── main.cpp        # Code logic chính điều khiển thiết bị
│   ├── platformio.ini      # File cấu hình thư viện và môi trường PlatformIO
│   └── diagram.json        # File sơ đồ mạch điện mô phỏng trên Wokwi
│
├── node_red/               # Cấu hình giao diện Web
│   └── flows.json          # File export chứa toàn bộ logic và Dashboard của Node-RED
│
└── README.md               # Tài liệu hướng dẫn dự án
```

---

## 🚀 Hướng dẫn cài đặt & Chạy thử nghiệm

### Yêu cầu chuẩn bị
- Visual Studio Code đã cài đặt extension **PlatformIO** và **Wokwi Simulator**.
- Node.js và đã cài đặt **Node-RED** cùng gói thư viện giao diện `node-red-dashboard`.

### Bước 1: Khởi động phần cứng ESP32 (Mô phỏng)
1. Mở thư mục `IOT_4/esp32` bằng VS Code.
2. Tại thanh trạng thái dưới cùng của PlatformIO, bấm biểu tượng **Build (✓)** để biên dịch mã nguồn và tải thư viện tự động.
3. Mở file `diagram.json`, nhấn nút **Play (▶)** để bắt đầu chạy mô phỏng mạch điện.
4. Kiểm tra cửa sổ Terminal để đảm bảo ESP32 đã báo `WiFi OK!` và `Đang kết nối MQTT... OK!`.

### Bước 2: Triển khai Web Dashboard
1. Mở Terminal mới, gõ lệnh `node-red` để khởi động server.
2. Truy cập vào trang quản lý tại địa chỉ: `http://localhost:1880`
3. Chọn Menu (3 dấu gạch ngang góc phải) ➔ **Import**.
4. Chọn file `IOT_4/node_red/flows.json` và nhấn nút Import.
5. Nhấn **Deploy** (màu đỏ) ở góc phải trên cùng để lưu thay đổi.

### Bước 3: Trải nghiệm
- Mở giao diện điều khiển (UI) tại: **`http://localhost:1880/ui`**
- Thử nghiệm kéo thả thanh trượt nhiệt độ trên cảm biến DHT22 trong Wokwi, bạn sẽ thấy sự thay đổi ngay lập tức trên web. Kéo nhiệt độ vượt qua 35°C để xem hệ thống cảnh báo hoạt động!

---
<div align="center">
<i>Được phát triển trong quá trình học tập và nghiên cứu IoT Engineering.</i>
</div>
