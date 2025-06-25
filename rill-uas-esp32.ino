#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// === WIFI CONFIG ===
const char* ssid = "codewithwan";
const char* password = "123321123";

// === MQTT CONFIG ===
const char* mqtt_server = "192.168.137.1";  
const int mqtt_port = 1883;
const char* mqtt_user = "uas25_ridwan";
const char* mqtt_pass = "uas25_ridwan";

// === TOPIK MQTT ===
const char* STATUS_TOPIC = "UAS25-IOT/Status";
const char* SUHU_TOPIC = "UAS25-IOT/33423315/SUHU";
const char* HUM_TOPIC  = "UAS25-IOT/33423315/KELEMBAPAN";
const char* LUX_TOPIC  = "UAS25-IOT/33423315/LUX";

// === PIN CONFIG ===
#define DHTPIN 4       // D4 → GPIO4
#define DHTTYPE DHT11
#define LDRPIN 34      // D34 → GPIO34 (ADC)
#define LEDPIN 13       // D2 → GPIO2

DHT dht(DHTPIN, DHTTYPE);
String status = "STOP";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;

void connectToWiFi() {
  Serial.print("[WIFI] Connecting to WiFi");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println(" Connected!");
  Serial.print("[WIFI] ESP32 IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.println("[DEBUG] Buka CMD di PC kamu, ketik 'ipconfig'");
  Serial.println("[DEBUG] Cari 'IPv4 Address' dari hotspot → isi itu ke mqtt_server di kode ini.");
}

void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("[MQTT] Message on ");
  Serial.print(topic);
  Serial.print(": ");
  Serial.println(message);

  if (String(topic) == STATUS_TOPIC) {
    status = message;
    status.trim();
    digitalWrite(LEDPIN, status.equalsIgnoreCase("START") ? HIGH : LOW);
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("[MQTT] Connecting...");
    if (client.connect("ESP32Client", mqtt_user, mqtt_pass)) {
      Serial.println(" connected!");
      client.subscribe(STATUS_TOPIC);
    } else {
      Serial.print(" failed: ");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, LOW); // LED mati awalnya

  dht.begin();
  connectToWiFi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;

    if (status.equalsIgnoreCase("START")) {
      float suhu = dht.readTemperature();
      float hum = dht.readHumidity();
      int lux = analogRead(LDRPIN); // nilai 0-4095

      // Validasi data sensor
      if (isnan(suhu) || isnan(hum)) {
        Serial.println("[ERROR] Failed to read from DHT!");
        return;
      }

      char suhuStr[10], humStr[10], luxStr[10];
      dtostrf(suhu, 4, 1, suhuStr);
      dtostrf(hum, 4, 1, humStr);
      sprintf(luxStr, "%d", lux);

      client.publish(SUHU_TOPIC, suhuStr);
      client.publish(HUM_TOPIC, humStr);
      client.publish(LUX_TOPIC, luxStr);

      Serial.printf("[PUBLISH] SUHU=%.1f | HUM=%.1f | LUX=%d\n", suhu, hum, lux);
    } else {
      Serial.printf("[STATUS] %s - STOP - waiting...\n", status.c_str());
    }
  }
}
