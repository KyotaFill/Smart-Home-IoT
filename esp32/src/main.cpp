#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <ArduinoJson.h>
#include <ESP32Servo.h>

// 1. Cấu hình chân phần cứng
#define DHT_PIN 4
#define DHT_TYPE DHT22
#define LED_PIN 2
#define BUZZER_PIN 5
#define SERVO_PIN 18
#define LDR_PIN 34

// 2. Cấu hình WiFi (Dành riêng cho Wokwi Simulator)
const char* WIFI_SSID = "Wokwi-GUEST";
const char* WIFI_PASS = "";

// 3. Cấu hình MQTT Broker
const char* MQTT_BROKER = "broker.hivemq.com";
const int MQTT_PORT = 1883;

// 4. Định nghĩa các MQTT Topics (Đổi tên cho độc nhất để không bị trùng với người khác trên public broker)
const char* TOPIC_DATA = "iot/myhome_999/sensor/data";
const char* TOPIC_LED = "iot/myhome_999/led/control";
const char* TOPIC_BUZZER = "iot/myhome_999/buzzer/control";
const char* TOPIC_SERVO = "iot/myhome_999/servo/control";

DHT dht(DHT_PIN, DHT_TYPE);
WiFiClient espClient;
PubSubClient client(espClient);
Servo myServo;

unsigned long lastMsg = 0;

// Hàm callback được gọi khi có thông điệp MQTT gửi đến (điều khiển LED, Còi, Servo)
void mqttCallback(char* topic, byte* payload, unsigned int len) {
  String msg = "";
  for (int i = 0; i < len; i++) {
    msg += (char)payload[i];
  }
  msg.trim(); // Cắt bỏ khoảng trắng dư thừa hoặc ký tự xuống dòng (nếu có)
  
  Serial.println("[MQTT] Nhận lệnh từ: " + String(topic) + " | Lệnh: [" + msg + "]");

  if (String(topic) == TOPIC_LED) {
    if (msg == "ON") {
      digitalWrite(LED_PIN, HIGH);
      Serial.println("=> Đã BẬT đèn LED");
    }
    else if (msg == "OFF") {
      digitalWrite(LED_PIN, LOW);
      Serial.println("=> Đã TẮT đèn LED");
    }
  }
  else if (String(topic) == TOPIC_BUZZER) {
    if (msg == "ON") tone(BUZZER_PIN, 1000); // Kêu ở tần số 1000Hz
    else if (msg == "OFF") noTone(BUZZER_PIN); // Tắt còi
  }
  else if (String(topic) == TOPIC_SERVO) {
    int angle = msg.toInt(); // Chuyển chuỗi thành số nguyên (góc 0-180)
    if (angle >= 0 && angle <= 180) {
      myServo.write(angle);
    }
  }
}

// Hàm kết nối lại với MQTT Broker
void reconnect() {
  while (!client.connected()) {
    String id = "ESP32_SmartHome_" + String(random(0xffff), HEX);
    Serial.print("Đang kết nối MQTT...");
    if (client.connect(id.c_str())) {
      Serial.println("OK!");
      client.subscribe(TOPIC_LED);
      client.subscribe(TOPIC_BUZZER);
      client.subscribe(TOPIC_SERVO);
    } else {
      Serial.println("Thất bại. Thử lại sau 5s...");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  dht.begin();
  
  // Cấu hình Timer cho Servo (Rất quan trọng cho ESP32)
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  myServo.setPeriodHertz(50); // Servo tiêu chuẩn 50Hz
  myServo.attach(SERVO_PIN, 500, 2400); 
  myServo.write(0); // Khởi tạo servo ở góc 0 độ

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Đang kết nối WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi OK! IP: " + WiFi.localIP().toString());

  client.setServer(MQTT_BROKER, MQTT_PORT);
  client.setCallback(mqttCallback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  // Gửi dữ liệu mỗi 5 giây
  if (now - lastMsg > 5000) {
    lastMsg = now;
    
    float t = dht.readTemperature();
    float h = dht.readHumidity();
    int lightRaw = analogRead(LDR_PIN);
    
    // Chuyển đổi giá trị quang trở (0-4095) sang % ánh sáng
    // LDR trên Wokwi: Sáng nhất ~ 0, tối nhất ~ 4095. Tùy thuộc cách mắc mạch.
    // Với sơ đồ chân AO nối thẳng, sáng = giá trị thấp.
    int lightPercent = map(lightRaw, 0, 4095, 100, 0);
    if(lightPercent < 0) lightPercent = 0;
    if(lightPercent > 100) lightPercent = 100;

    if (!isnan(t) && !isnan(h)) {
      StaticJsonDocument<256> doc;
      doc["temperature"] = t;
      doc["humidity"] = h;
      doc["light"] = lightPercent;
      doc["device"] = "ESP32_SmartHome";
      
      char buffer[256];
      serializeJson(doc, buffer);
      
      client.publish(TOPIC_DATA, buffer);
      Serial.println("Đã gửi Data: " + String(buffer));
    } else {
      Serial.println("Lỗi đọc DHT22!");
    }
  }
}
