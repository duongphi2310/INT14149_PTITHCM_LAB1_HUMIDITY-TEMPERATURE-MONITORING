#include "DHT.h"
#include "EspMQTTClient.h"
#include "mqtt_secrets.h"

#define DHTPIN D2
#define DHTTYPE DHT11
unsigned long delayTime = 10000;

DHT dht(DHTPIN, DHTTYPE);
float h = 0.0;
float t = 0.0;

// Khởi tạo đối tượng client của thư viện EspMQTTClient
EspMQTTClient client (
  SECRET_WIFI_NAME,
  SECRET_WIFI_PASSWORD,
  MQTT_BROKER_SERVER_IP,
  SECRET_MQTT_USERNAME,
  SECRET_MQTT_PASSWORD, 
  SECRET_MQTT_CLIENT_ID
);

// kết nối với máy chủ MQTT 
void onConnectionEstablished() {
    // đăng ký một kênh MQTT để nhận dữ liệu từ máy chủ MQTT
    client.subscribe("channels/" + String(CHANNEL_ID) + "/subscribe", [](const String & payload) {
      Serial.println(payload);  
    });  
    Serial.print("NHIET DO: ");
    Serial.print(t);
    Serial.print(" , DO AM: ");
    Serial.print(h);
    Serial.println("%");
    Serial.println("Gui Du Lieu Den ThingSpeak.");
    // Tạo một thông điệp để gửi đến máy chủ MQTT với dữ liệu nhiệt độ và độ ẩm.
    String message = "field1=" + String(t) + "&field2=" + String(h);
    // gửi dữ liệu lên máy chủ MQTT
    client.publish("channels/" + String(CHANNEL_ID) + "/publish", message);
    client.executeDelayed(5 * 1000, [](){});
}

void setup() {
  Serial.begin(115200);
  // khoi dong cam bien
  dht.begin();
  client.enableDebuggingMessages();
  client.enableHTTPWebUpdater();
  client.enableOTA();
  client.enableLastWillMessage("TestClient/lastwill", "I am going offline");
}

void loop() {
  // doc gia tri nhietdo va doam tu DHT11
  h = dht.readHumidity();
  t = dht.readTemperature();
  // Kiểm tra xem việc đọc cảm biến có thành công không.
  if (isnan(t) || isnan(h)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
  }
  client.loop();
  delay(delayTime);
}
