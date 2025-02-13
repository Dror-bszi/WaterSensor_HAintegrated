#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define SENSOR_PIN A0    // Soil Moisture Sensor
#define SENSOR_VCC 12    // GPIO12 (D6) Powers the Sensor
#define LED_PIN 14       // LED simulating pump (D5 - GPIO14)

// **Adjustable Settings**
#define MOISTURE_CHECK_INTERVAL_SECONDS 10  // Moisture check every 10 seconds
#define MOISTURE_THRESHOLD 500              // Moisture threshold for dry soil

// **WiFi & MQTT Credentials**
const char* ssid = "Dror&Yuval";
const char* password = "318187747";
const char* mqtt_server = "10.100.102.19";
const int mqtt_port = 1883;
const char* mqtt_user = "drorb_mqtt";
const char* mqtt_pass = "1997";

WiFiClient espClient;
PubSubClient client(espClient);

// **WiFi Connection Function**
void connectToWiFi() {
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(1000);
    Serial.print(".");
    attempts++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi Connected!");
  } else {
    Serial.println("\nWiFi Failed! Retrying in loop...");
  }
}

// **MQTT Connection Function**
void connectToMQTT() {
  client.setServer(mqtt_server, mqtt_port);
  int attempts = 0;
  while (!client.connected() && attempts < 10) {
    if (client.connect("ESP8266WaterSensor", mqtt_user, mqtt_pass)) {
      Serial.println("Connected to MQTT!");
    } else {
      Serial.print("MQTT Failed. RC=");
      Serial.print(client.state());
      Serial.println(" Retrying...");
      attempts++;
      delay(5000);
    }
  }
  if (!client.connected()) {
    Serial.println("MQTT Connection Failed! Retrying in loop...");
  }
}

// **Function to Read Moisture & Publish to MQTT**
void readMoistureAndPublish() {
  Serial.println("Checking Soil Moisture...");
  digitalWrite(SENSOR_VCC, HIGH); // Turn ON soil moisture sensor
  delay(500);  // Allow sensor to stabilize

  int rawSensorValue = analogRead(SENSOR_PIN);
  int moisturePercent = map(rawSensorValue, 1023, 300, 0, 100);
  moisturePercent = constrain(moisturePercent, 0, 100);  // Keep values between 0-100%

  // **Print Raw & Mapped Values for Calibration**
  Serial.print("Raw Moisture Sensor Value: ");
  Serial.print(rawSensorValue);
  Serial.print(" | Moisture Level: ");
  Serial.print(moisturePercent);
  Serial.println("%");

  digitalWrite(SENSOR_VCC, LOW); // Turn OFF sensor

  // **Publish Raw & Percentage Values to MQTT**
  char msg_percent[10];
  char msg_raw[10];

  dtostrf(moisturePercent, 6, 2, msg_percent);
  dtostrf(rawSensorValue, 6, 0, msg_raw);

  client.publish("home/plant/moisture_percent", msg_percent);
  client.publish("home/plant/moisture_raw", msg_raw);

  Serial.print("MQTT Sent → Percent: ");
  Serial.print(moisturePercent);
  Serial.print("% | Raw: ");
  Serial.println(rawSensorValue);

  // **Pump Control (LED)**
  if (rawSensorValue > MOISTURE_THRESHOLD) {
    digitalWrite(LED_PIN, HIGH);
    Serial.println("Soil is DRY → Turning ON Pump (LED)");
    client.publish("home/plant/pump_status", "ON");
  } else {
    digitalWrite(LED_PIN, LOW);
    Serial.println("Soil is MOIST → Turning OFF Pump (LED)");
    client.publish("home/plant/pump_status", "OFF");
  }

  client.loop();  // Ensure MQTT messages are sent
}

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  pinMode(SENSOR_VCC, OUTPUT);
  digitalWrite(SENSOR_VCC, LOW); // Start with sensor OFF

  Serial.println("ESP8266 Waking Up...");

  connectToWiFi();
  connectToMQTT();
}

void loop() {
  // **Reconnect if WiFi or MQTT disconnects**
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi Disconnected! Reconnecting...");
    connectToWiFi();
  }
  
  if (!client.connected()) {
    Serial.println("MQTT Disconnected! Reconnecting...");
    connectToMQTT();
  }

  readMoistureAndPublish();
  delay(MOISTURE_CHECK_INTERVAL_SECONDS * 1000);  // Wait for the next check
}
