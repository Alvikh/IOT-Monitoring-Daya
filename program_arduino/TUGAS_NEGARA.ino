#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient client(espClient);
String buffData = "";

const char* WIFI_SSID = "OPPO";
const char* WIFI_PASS = "Qwertyui";
const char* MQTT_SERVER = "broker.hivemq.com";
const int MQTT_PORT = 1883;
const char *topicPub = "kelompok-5/sensor/2";

// Function prototypes
void setup_wifi();
void callback(char* topic, byte* payload, unsigned int length);
void reconnect();
void setup();
void loop();
void kirimWatt();

void setup_wifi() {
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  // Add your custom callback handling here
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Menghubungkan MQTT... ");
    String clientId = "kelompok-5";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("Berhasil");
      client.publish(topicPub, "Yess... saya terkoneksi");
      client.subscribe("kelompok-5/sensor/2");
      kirimWatt();
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

int voltagePin = 34;
int currentPin = 35;

const float voltageReference = 5.0;
const float voltageMax = 250.0;
const float currentMax = 30.0;

void setup() {
  pinMode(voltagePin,INPUT);
  pinMode(currentPin,INPUT);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(callback);
}

unsigned long lastUpdateTime = 0;
const unsigned long updateInterval = 60000;  // Update interval in milliseconds (1 minute)

void loop() {
  unsigned long currentTime = millis();  // Added missing variable declaration
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if (currentTime - lastUpdateTime >= updateInterval) {
    kirimWatt();
    lastUpdateTime = currentTime;
  }
}

void kirimWatt() {
  int voltageRawValue = analogRead(voltagePin);
  int currentRawValue = analogRead(currentPin);

  float voltageValue = (voltageRawValue / 1023.0) * voltageMax;
  float currentVoltage = (currentRawValue / 1023.0) * voltageReference;

  float power = voltageValue * currentVoltage;
  float kWh = power / 1000.0;

  Serial.print("Nilai Mentah Tegangan: ");
  Serial.println(voltageRawValue);
  Serial.print("Nilai Mentah Arus: ");
  Serial.println(currentRawValue);
  Serial.print("Tegangan (V): ");
  Serial.println(voltageValue);
  Serial.print("Arus (A): ");
  Serial.println(currentVoltage);
  Serial.print("Daya (W): ");
  Serial.println(power);
  Serial.print("Konsumsi Energi (kWh): ");
  Serial.println(kWh);
  Serial.println();

  String message = "Nilai Mentah Tegangan : " + String(voltageRawValue) +
                   ", Nilai Mentah Arus: " + String(currentRawValue) +
                   ", Tegangan (V): " + String(voltageValue) +
                   ", Arus (A): " + String(currentVoltage) +
                   ", Daya (W): " + String(power) +
                   ", Konsumsi Energi (kWh): " + String(kWh);

  client.publish("kelompok-5/sensor/2", message.c_str());
}