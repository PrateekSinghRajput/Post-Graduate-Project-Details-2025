# 🌱 Wireless Soil Nutrient & Temperature Monitoring System for Precision Agriculture 🚜

Welcome to your **Wireless Soil Nutrient and Temperature Monitoring System** powered by ESP32 and LoRa! This solution brings real-time, wireless soil analysis to your farm, helping you make data-driven decisions for higher yields and smarter resource use.

---

## 🛠️ Project Overview

This system measures **soil NPK nutrients** (Nitrogen, Phosphorus, Potassium) and **soil temperature**, then transmits the data wirelessly via **LoRa** to a central gateway. Farmers can view data locally on displays or remotely on the cloud, enabling:

- **Optimized fertilizer use**
- **Automated irrigation**
- **Multi-zone monitoring across large fields**
- **Real-time and historical data analysis**

---

## 🌟 Features

- **Soil Nutrient Monitoring:**  
  Measures N, P, K levels (mg/kg) using an RS485-based NPK sensor with Modbus RTU protocol.

- **Soil Temperature Sensing:**  
  Uses a waterproof DS18B20 sensor for accurate soil temperature readings (°C).

- **Wireless Communication:**  
  LoRa protocol ensures low-power, long-range, and robust data transmission, ideal for large agricultural fields.

- **Local Display:**  
  Real-time data visualization on I2C 16x2 LCD and 128x64 OLED displays.

- **Multi-Node Support:**  
  Deploy multiple ESP32 sensor nodes, all communicating with a central ESP32 LoRa gateway.

- **Cloud Integration:**  
  Optional logging to platforms like ThingSpeak, Blynk, or Firebase for advanced analytics and remote monitoring[2][6].

---

## 🔩 Hardware Components

| Component              | Description                                    |
|------------------------|------------------------------------------------|
| **ESP32**              | Microcontroller for sensors & communication    |
| **RS485 NPK Sensor**   | Measures soil Nitrogen, Phosphorus, Potassium  |
| **MAX485 Module**      | UART to RS485 (Modbus RTU) conversion          |
| **DS18B20 Sensor**     | Waterproof 1-Wire temperature sensor           |
| **I2C LCD (16x2)**     | Local display for nutrients & temperature      |
| **OLED Display (128x64)** | Additional local display                    |
| **LoRa Module (e.g., SX1278)** | Long-range wireless communication     |
| **Power Supply**       | Stable power for ESP32 & sensors               |

---

## 🏗️ System Architecture

### 1️⃣ NPK Sensor Communication

- Communicates via Modbus RTU over RS485.
- ESP32 sends query frames, receives NPK data.
- MAX485 module switches between transmit/receive modes using DE/RE pins.

### 2️⃣ Temperature Sensing

- DS18B20 sensor uses 1-Wire protocol.
- ESP32 reads temperature via DallasTemperature library.

### 3️⃣ Wireless Transmission

- Sensor nodes send data via LoRa to a central gateway ESP32.
- LoRa enables long-range, low-power communication—perfect for remote fields[1][2].

---

## 🚜 Applications

- **Precision Farming:**  
  Apply fertilizer only where and when needed, based on real-time soil data.

- **Automated Irrigation:**  
  Integrate with irrigation valves to water crops only when necessary, saving water and energy.

- **Multi-Node Monitoring:**  
  Cover large farms by deploying multiple wireless sensor nodes.

- **Data Logging & Analysis:**  
  Store and analyze data on cloud platforms for trend analysis and smarter farm management[6].

---

## 🧰 Troubleshooting Guide

- **❌ No data received:**  
  - Check wiring, especially MAX485 DE/RE pins and LoRa connections.
- **📶 LoRa transmission fails:**  
  - Verify LoRa wiring, frequency/channel settings, and antenna.
- **⚠️ Sensor readings incorrect:**  
  - Ensure proper sensor calibration and correct Modbus register addresses.
- **🖥️ Display not working:**  
  - Confirm I2C wiring and correct device addresses.

---

## 🌐 Why Wireless Monitoring?

Wireless sensor networks (WSNs) are transforming agriculture by enabling real-time monitoring of key soil parameters. By integrating NPK and temperature sensors with wireless communication, farmers can:

- **Optimize fertilizer and water usage**
- **Prevent nutrient imbalances**
- **Boost crop yields**
- **Reduce environmental impact**[1][4][6]

---

## 🎉 Happy Precision Farming!

With this system, you can monitor your soil’s health anytime, anywhere—making your farm smarter, more sustainable, and more productive.  
**Grow more with data!** 🌾📈

---

*For more details, see the full project repository or reach out for support. Happy farming!*
