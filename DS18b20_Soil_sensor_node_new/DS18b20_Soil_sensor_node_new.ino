#include <esp_now.h>
#include <WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

// Define sensor pins
#define ONE_WIRE_BUS 27        // GPIO pin for DS18B20
#define SOIL_MOISTURE_PIN 34   // GPIO pin for soil moisture sensor
#define OLED_SDA 21            // GPIO pin for OLED SDA
#define OLED_SCL 22            // GPIO pin for OLED SCL
#define OLED_RST -1            // Reset pin (-1 if not available)
#define SCREEN_WIDTH 128       // OLED display width
#define SCREEN_HEIGHT 64       // OLED display height

// Setup a oneWire instance
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature ds18b20(&oneWire);

// Create OLED display instance
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

// Structure to send data
typedef struct struct_message {
  int nodeId;
  float temperature;
  int soilMoisture;
} struct_message;

struct_message myData;

// Replace with the receiver's MAC Address
uint8_t broadcastAddress[] = {0x78, 0x42, 0x1C, 0x6C, 0xE1, 0x94}; // Update this

//78:42:1C:6C:E1:94

// Peer info
esp_now_peer_info_t peerInfo;

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
  Serial.begin(115200);
  
  // Initialize OLED display
  Wire.begin(OLED_SDA, OLED_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Initializing...");
  display.display();
  delay(2000);
  
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  
  // Initialize DS18B20 sensor
  ds18b20.begin();
  
  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("ESP-NOW init failed!");
    display.display();
    return;
  }
  
  // Register callback
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("Peer add failed!");
    display.display();
    return;
  }
  
  // Set node ID based on MAC address (last byte)
  myData.nodeId = ESP.getEfuseMac() & 0xFF;
  
  // Configure ADC for soil moisture
  analogReadResolution(12); // Set ADC resolution to 12 bits
  analogSetAttenuation(ADC_11db); // Set attenuation to 11dB for full range 0-3.3V
}

void loop() {
  // Request temperature from DS18B20
  ds18b20.requestTemperatures();
  myData.temperature = ds18b20.getTempCByIndex(0);
  
  // Read soil moisture (0-4095, higher means more moisture)
  myData.soilMoisture = analogRead(SOIL_MOISTURE_PIN);
  
  // Check if temperature read failed
  if (myData.temperature == DEVICE_DISCONNECTED_C) {
    Serial.println("Failed to read from DS18B20 sensor!");
  } else {
    Serial.print("Node ID: ");
    Serial.println(myData.nodeId);
    Serial.print("Temperature: ");
    Serial.print(myData.temperature);
    Serial.println(" *C");
    Serial.print("Soil Moisture: ");
    Serial.println(myData.soilMoisture);
    
    // Update OLED display
    display.clearDisplay();
    display.setCursor(0,0);
    display.print("Node: ");
    display.println(myData.nodeId);
    display.print("Temp: ");
    display.print(myData.temperature);
    display.println(" C");
    display.print("Soil: ");
    display.println(myData.soilMoisture);
    display.display();
  }
  
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  } else {
    Serial.println("Error sending the data");
  }
  
  delay(10000); // Send every 10 seconds
}