# Wireless Soil Nutrient and Temperature Monitoring System for Precision Agriculture

## Project Overview

This project implements a wireless soil nutrient and temperature monitoring system designed to support precision agriculture. It uses an ESP32 microcontroller to measure soil NPK levels (Nitrogen, Phosphorous, Potassium) and soil temperature, then transmits the data wirelessly using the **LoRa** protocol to a central receiver.

The system enables farmers to optimize fertilizer usage, automate irrigation control, and monitor multiple nodes across large farms with real-time data and local displays.

---

## Features

- **Soil Nutrient Monitoring:** Measures N, P, K levels (mg/kg) via RS485-based NPK sensor using Modbus RTU.
- **Soil Temperature Sensing:** Measures soil temperature (°C) with DS18B20 waterproof sensor.
- **Wireless Communication:** LoRa protocol for low-power, long-range, and reliable data transmission.
- **Local Display:** I2C 16x2 LCD and 128x64 OLED for onsite real-time data visualization.
- **Multi-node Support:** Multiple ESP32 sensor nodes communicate with a central ESP32 gateway.
- **Cloud Integration:** Optional data logging to platforms like ThingSpeak, Blynk, Firebase for analysis.

---

## Hardware Components

| Component              | Description                                   |
|-----------------------|-----------------------------------------------|
| ESP32                 | Microcontroller handling sensor and comms    |
| RS485 NPK Sensor      | Soil nutrient detection (N, P, K)             |
| MAX485 Module         | UART to RS485 conversion                       |
| DS18B20 Sensor        | Waterproof temperature sensor (1-Wire)        |
| I2C LCD (16x2)        | Local nutrient and temperature display        |
| OLED Display (128x64) | Additional local display                        |
| LoRa Module (e.g., SX1278) | For long-range wireless communication    |
| Power Supply          | Suitable power source for ESP32 and sensors   |

---

## System Architecture

### 1. NPK Sensor Communication

- Uses Modbus RTU protocol over RS485.
- ESP32 sends query frames requesting nutrient data.
- MAX485 controls transmit/receive mode via DE/RE pins.

### 2. Temperature Sensing

- DS18B20 sensor uses 1-Wire protocol.
- ESP32 reads temperature using DallasTemperature library.

### 3. Wireless Transmission

- ESP32 nodes send data via **LoRa** to a receiver ESP32 gateway.
- LoRa provides long-range, low-power, and reliable communication suitable for agricultural fields.


---

## Applications

- **Precision Farming:** Optimize fertilizer application based on real-time soil nutrient data.
- **Automated Irrigation:** Integrate with solenoid valves to adjust watering based on soil conditions.
- **Multi-Node Monitoring:** Deploy multiple sensor nodes for large farm coverage.
- **Data Logging & Analysis:** Store data on cloud platforms for trend analysis and decision making.

---

---

## Troubleshooting

- **No data received:** Check wiring, especially MAX485 DE/RE pin control and LoRa connections.
- **LoRa transmission fails:** Confirm LoRa module wiring, frequency settings, and antenna connection.
- **Sensor readings incorrect:** Verify sensor calibration and Modbus register addresses.
- **Display not working:** Check I2C connections and addresses.

---

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

Thank you for using this Wireless Soil Nutrient and Temperature Monitoring System with LoRa!  
Happy precision farming! 🌱🚜

