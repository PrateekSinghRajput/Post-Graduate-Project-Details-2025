#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <WiFi.h>
#include <esp_now.h>

// TFT display pins
#define TFT_CS    5
#define TFT_DC    2
#define TFT_RST   4
#define TFT_MOSI  23
#define TFT_SCLK  18

// Initialize TFT
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

// ESP-NOW data structure
typedef struct struct_message {
  int nodeId;
  float temperature;
  int soilMoisture;
} struct_message;

// Data storage
struct_message node1Data;
struct_message node2Data;
bool node1Received = false;
bool node2Received = false;
bool newDataAvailable = false;

// Colors
#define BLACK     ST7735_BLACK
#define BLUE      ST7735_BLUE
#define RED       ST7735_RED
#define GREEN     ST7735_GREEN
#define WHITE     ST7735_WHITE

void setup() {
  Serial.begin(115200);
  
  // Initialize TFT display
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(3);  // Landscape orientation
  tft.fillScreen(BLACK);
  
  // Display startup message
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(10, 10);
  tft.println("Initializing ");
  
  // Initialize ESP-NOW
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ");
    displayErrorScreen("Init Failed!");
    while(1);
  }
  
  esp_now_register_recv_cb(OnDataRecv);
  
  // Draw initial screen
  drawAllDataScreen();
}

void loop() {
  // Update screen when new data arrives
  if (newDataAvailable) {
    newDataAvailable = false;
    drawAllDataScreen();
  }
  
  // Optional: Periodic refresh (e.g., every 10 seconds)
  static unsigned long lastRefresh = 0;
  if (millis() - lastRefresh >= 10000) {
    lastRefresh = millis();
    drawAllDataScreen();
  }
}

// ESP-NOW callback function
void OnDataRecv(const esp_now_recv_info_t *esp_now_info, const uint8_t *incomingData, int len) {
  struct_message tempData;
  memcpy(&tempData, incomingData, sizeof(tempData));

  // Update data with critical section
  noInterrupts();
  if (tempData.nodeId == 172) {
    memcpy(&node1Data, &tempData, sizeof(struct_message));
    node1Received = true;
  } else if (tempData.nodeId == 2) {
    memcpy(&node2Data, &tempData, sizeof(struct_message));
    node2Received = true;
  }
  newDataAvailable = true;
  interrupts();

  // Print to serial for debugging
  Serial.print("Node ID: ");
  Serial.println(tempData.nodeId);
  Serial.print("Temperature: ");
  Serial.print(tempData.temperature);
  Serial.println(" °C");
  Serial.print("Soil Moisture: ");
  Serial.println(tempData.soilMoisture);
  Serial.println();
}

void displayErrorScreen(const char* errorMessage) {
  tft.fillScreen(BLACK);
  tft.setTextColor(RED);
  tft.setTextSize(1);
  tft.setCursor(10, 10);
  tft.println("ERROR:");
  tft.setCursor(10, 30);
  tft.println(errorMessage);
}

void drawAllDataScreen() {
  tft.fillScreen(BLACK);
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  tft.setCursor(10, 5);
  tft.println("Node Monitor Dashboard");
  tft.drawFastHLine(0, 15, tft.width(), WHITE);
  
  // Create local copies of the data for thread safety
  struct_message localNode1Data;
  struct_message localNode2Data;
  bool localNode1Received, localNode2Received;
  
  // Critical section to copy data
  noInterrupts();
  memcpy(&localNode1Data, &node1Data, sizeof(struct_message));
  memcpy(&localNode2Data, &node2Data, sizeof(struct_message));
  localNode1Received = node1Received;
  localNode2Received = node2Received;
  interrupts();
  
  // Node 1 data (top half)
  tft.setTextColor(GREEN);
  tft.setCursor(10, 25);
  tft.print("Node 1: ");
  if (localNode1Received) {
    tft.println("Received");
    tft.setTextColor(WHITE);
    tft.setCursor(20, 40);
    tft.print("Temperature: ");
    tft.print(localNode1Data.temperature, 1);
    tft.println(" C");
    tft.setCursor(20, 55);
    tft.print("Soil Moisture: ");
    tft.print(localNode1Data.soilMoisture);
    tft.println(" %");
  } else {
    tft.println("Waiting...");
  }
  
  // Divider line
  tft.drawFastHLine(0, 75, tft.width(), WHITE);
  
  // Node 2 data (bottom half)
  tft.setTextColor(BLUE);
  tft.setCursor(10, 85);
  tft.print("Node 2: ");
  if (localNode2Received) {
    tft.println("Received");
    tft.setTextColor(WHITE);
    tft.setCursor(20, 100);
    tft.print("Temperature: ");
    tft.print(localNode2Data.temperature, 1);
    tft.println(" C");
    tft.setCursor(20, 115);
    tft.print("Soil Moisture: ");
    tft.print(localNode2Data.soilMoisture);
    tft.println(" %");
  } else {
    tft.println("Waiting...");
  }
  
  // Last update time
  //tft.setTextColor(WHITE);
  tft.setCursor(10, 140);
  tft.print("Last update: ");
  tft.print(millis()/700);
  tft.print("s ago");
}