#  ESP8266 Soil Moisture Sensor with MQTT

This project uses an **ESP8266** microcontroller to measure soil moisture, control a simulated water pump (LED), and publish readings to an **MQTT** broker for smart garden automation. Ideal for integration with **Home Assistant** or other IoT platforms.

---

## 🔧 Features

- Reads analog soil moisture values via a sensor
- Converts raw values into percentage moisture
- Powers sensor only during reads (GPIO-controlled)
- Simulates a water pump using an onboard LED (GPIO14)
- Publishes moisture data and pump status to MQTT topics
- Automatically reconnects to Wi-Fi and MQTT on disconnection
- Easily adjustable moisture threshold and check intervals

---

##  Hardware Requirements

- **ESP8266 board** (e.g., NodeMCU or Wemos D1 Mini)
- **Soil moisture sensor** (analog type)
- **LED** + resistor to simulate a pump (connected to GPIO14)
- **Jumper wires**
- (Optional) Breadboard or enclosure

---

##  Wiring

| Component        | ESP8266 Pin   | Description                  |
|------------------|---------------|------------------------------|
| Soil Sensor OUT  | A0            | Analog read pin              |
| Soil Sensor VCC  | D6 / GPIO12   | Powered only during read     |
| LED (Pump Sim.)  | D5 / GPIO14   | Turns on when soil is dry    |
| GND              | G             | Common ground                |

---

##  MQTT Topics

| Topic                         | Description                      |
|------------------------------|----------------------------------|
| `home/plant/moisture_raw`    | Raw ADC value (0–1023)           |
| `home/plant/moisture_percent`| Mapped moisture percentage (0–100%) |
| `home/plant/pump_status`     | `"ON"` or `"OFF"` based on dryness |

---

##  Configuration

All secrets (Wi-Fi & MQTT) should be placed in `secrets.h`:

```cpp
#define WIFI_SSID "YourSSID"
#define WIFI_PASS "YourPassword"

#define MQTT_SERVER "192.168.1.x"
#define MQTT_PORT 1883
#define MQTT_USER "mqtt_user"
#define MQTT_PASS "mqtt_pass"
