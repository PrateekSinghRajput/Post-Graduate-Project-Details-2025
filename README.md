# Post-Graduate-Project-Details-2025

Project Overview

This project is a wireless soil nutrient and temperature monitoring system designed for precision agriculture. It uses an ESP32 microcontroller to measure

Soil NPK levels (Nitrogen, Phosphorous, Potassium)
Soil temperature
and transmits the data wirelessly using ESP-NOW to a central receiver (another ESP32 or gateway).

The system includes
RS485-based NPK sensor (for soil nutrient detection)
DS18B20 waterproof temperature sensor (for soil temperature)
I2C LCD (16x2) and OLED (128x64) displays for local monitoring
ESP-NOW protocol for long-range, low-power wireless communication

1.NPK Sensor Communication
The NPK sensor uses Modbus RTU over RS485.
The ESP32 sends Modbus query frames to request N, P, and K values.
The sensor responds with the nutrient levels in mg/kg (ppm).
MAX485 module handles the UART-to-RS485 conversion (DE/RE pins control transmit/receive mode).

2.Temperature Sensing
The DS18B20 (1-Wire protocol) measures soil temperature.
The ESP32 reads temperature in °C using the DallasTemperature library.

3.Wireless Transmission (ESP-NOW)
The ESP32 sends data to a receiver ESP32 (MAC address configured).
ESP-NOW is used instead of WiFi for
Lower latency
Longer range
No need for a WiFi router

struct struct_message {
  int nodeId;       // Unique node identifier (based on MAC)
  float temperature; // Soil temp in °C
  byte nitrogen;     // N value (mg/kg)
  byte phosphorous;  // P value (mg/kg)
  byte potassium;    // K value (mg/kg)
};

Applications in Agriculture
Precision Farming
Farmers can optimize fertilizer usage based on real-time NPK data.

Automated Irrigation Control
Can be integrated with solenoid valves to adjust water based on soil conditions.

Multi-Node Monitoring
Multiple ESP32 nodes can send data to a central dashboard for large farms.

Data Logging & Analysis
Data can be stored in cloud platforms (ThingSpeak, Blynk, Firebase) for long-term trends.
