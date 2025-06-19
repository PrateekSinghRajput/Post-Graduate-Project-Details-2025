#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <WiFi.h>
#include <esp_now.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"

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
  byte nitrogen;
  byte phosphorous;
  byte potassium;
} struct_message;

// Data storage
struct_message node1Data;
struct_message node2Data;
bool node1Received = false;
bool node2Received = false;
bool newDataAvailable = false;

// WiFi credentials
#define WIFI_SSID "Prateek"
#define WIFI_PASSWORD "justdoelectronics@#12345"

// Firebase configuration
#define FIREBASE_PROJECT_ID "fieldnet-841e0"
#define FIREBASE_CLIENT_EMAIL "firebase-adminsdk-fbsvc@fieldnet-841e0.iam.gserviceaccount.com"
#define FIREBASE_PRIVATE_KEY  "-----BEGIN PRIVATE KEY-----\nMIIEvwIBADANBgkqhkiG9w0BAQEFAASCBKkwggSlAgEAAoIBAQCUNNXBNkaFK68a\n1O6Lx6pbYKHE8Q9j91/0G+xO2dQCgHFPbXYGB0+d+/nAUOUWN1b4/ymB9InF2ChY\nhsS4i+D4K/LF2IOv6L0n5rKfAIrqbJVO/Z+y+CcO4ufg+3q+AmzTwcvVYHdRlnoE\nIzIfsJegWxqdlhIEEGNURb+U+ouF6vsWBs5FQhML7kTKrbprmZuhbhJW7Ul3llSt\npS0fY/rizkE2dLzOGhSG9RAtGcHG63D6rvZXnbwC9/+5LaYdr5woLPzGvGPWjqmY\ne8RC4LnCZ4Uk1CRay/uu8HuZopZCm0PAxIixABDKj6v9DxcxNAgg63CqdY/hd408\nDJ2eogtfAgMBAAECggEAFcOMgEsQIA5CMJaEUVKI2dJe7yTXNb7mAFr5YLrsP1HN\nUfSpGEmKj5CpJGy8t3547yfJWKQodz0+74Ea8l7YMpmBlTwuWm67GFjwchkKTVKU\nunzULtvyIGWB17AWXkDM6mvTGwqHexu131WrzquIGKxIREv9aUteyslTUPVZ99hI\nvFRIx2X9ti/yDVIxpcvBoySG3TmopENCzAxxPKDlVRgWTk+ifK+sj2Cn+FvdUBTz\ndoMBBQxZaOu+AOuzsPsAKTzTcBqZnoc0te6xVXELcLDUV3LeweJjnzErpsGy8gR2\nhK7Qe6BLZpvU6j6iT6ErgqBrlr76RpV/4SjLza5AwQKBgQDIG6j1pIAQu1NYPvqD\nExrrs5tvsI477P9kPeyObrSCQRRQUllntHkm+jM0fM5JBxoFmhfHGdKHdkM7OwIN\nQKQkVqxsWaOrS8QUALsjqB52C9JZobKXLT93V8BTzDDqvHqkOj/j+F8VQuQUXlN0\nVz51/0MtTCJGyDS7kligESSo8wKBgQC9mgv+nxaRa0SFMmxhewmpXIdAE4ZD3GoQ\nKYRXzIF0N2TxW/f8dbbQNV61yHPAyLILGVfIPoCNkr2FYS8XkYj9jsNCaOSJ6J1i\nYj+wdGQu0AFaG7sHaYdDZOSJDf5JUtupSKn1hVKykXrEXAgfmWNK9HBYCEkLPQSC\nUty3Xmfu5QKBgQCNKn4qNE8n/o6QTXV6bErshjscMshUXB/a/phX7p8m2HRVKZLq\nxvVE2vKuj3ETMZBY7Vji20OazoIlUb5jzPs+q2pd0WrV19BOcCmUwPAkRLizBJAx\nd/ky3tPOCdlkb2xxWKpjz358SEEQYW6sPzPQGDCyzufLWUdiH56n5RAgowKBgQCK\nw9z1fL/KrGRYy9+wtJkMoBDI38FLl1z0+T3H+H/ia8xwLWxhFFI5LmZjyxKUzQ4p\nu2GYx83UT8pvfQI3UKefU8Yqq5RgLqRJZjjQz06nSEtFBLisoxqBQ61Tda9fQMpN\nqkXh0LMGyBy+69ByOBQIvsAeXen/1KehLj4l4m1U1QKBgQCftyRQ3KIdsgqX2GzD\ntOOaouoUg+KPrWkBNV/JNOb8gX3gm1l8661H9nE3RuljXOPizUUUzcukllYObSI/\nF3mDKdjKEmlcibLCLShQN2+KJdXxS96K/rkG0Zm8g3Gb1JhVwJdUhGNrzEzu5HQU\nzbgZnmK1YajzgiLgyOsV9e5uyQ==\n-----END PRIVATE KEY-----\n";

#define FIREBASE_DATABASE_URL "https://fieldnet-841e0-default-rtdb.firebaseio.com" //https://fieldnet-841e0-default-rtdb.firebaseio.com/

// Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Colors
#define BLACK     ST7735_BLACK
#define BLUE      ST7735_BLUE
#define RED       ST7735_RED
#define GREEN     ST7735_GREEN
#define YELLOW    ST7735_YELLOW
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
  tft.println("Initializing...");
  
  // Connect to WiFi
  connectToWiFi();

  // Initialize ESP-NOW
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    displayErrorScreen("ESP-NOW Init Fail");
    while(1);
  }
  
  esp_now_register_recv_cb(OnDataRecv);
  
  // Initialize Firebase
  initializeFirebase();
  
  // Draw initial screen
  drawAllDataScreen();
}

void loop() {
  // Update screen when new data arrives
  if (newDataAvailable) {
    newDataAvailable = false;
    drawAllDataScreen();
    
    // Send data to Firebase when both nodes have reported
    if (node1Received && node2Received) {
      sendDataToFirebase();
    }
  }
  
  // Periodic refresh every 10 seconds
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
  } else if (tempData.nodeId == 60) {
    memcpy(&node2Data, &tempData, sizeof(struct_message));
    node2Received = true;
  }
  newDataAvailable = true;
  interrupts();

  // Debug output
  Serial.print("Node ID: ");
  Serial.println(tempData.nodeId);
  Serial.print("Temperature: ");
  Serial.print(tempData.temperature);
  Serial.println(" °C");
  Serial.print("Soil Moisture: ");
  Serial.println(tempData.soilMoisture);
  Serial.print("NPK: ");
  Serial.print(tempData.nitrogen);
  Serial.print("-");
  Serial.print(tempData.phosphorous);
  Serial.print("-");
  Serial.println(tempData.potassium);
}

void connectToWiFi() {
  tft.setCursor(10, 20);
  tft.print("Connecting to WiFi...");
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    tft.print(".");
  }
  
  tft.setCursor(10, 30);
  tft.println("WiFi Connected!");
  Serial.println("WiFi Connected");
}

void initializeFirebase() {
  tft.setCursor(10, 40);
  tft.print("Initializing Firebase...");

  // Assign the database URL
  config.database_url = FIREBASE_DATABASE_URL;
  
  // Assign the project ID (required for service account authentication)
  config.service_account.data.project_id = FIREBASE_PROJECT_ID;
  
  // Assign the client email
  config.service_account.data.client_email = FIREBASE_CLIENT_EMAIL;
  
  // Assign the private key (make sure it's properly formatted)
  config.service_account.data.private_key = FIREBASE_PRIVATE_KEY;

  // Set token callback function
  config.token_status_callback = tokenStatusCallback;
  
  // Enable WiFi reconnection
  Firebase.reconnectWiFi(true);
  
  // Set floating point precision
  Firebase.setDoubleDigits(5);

  // Initialize Firebase
  Firebase.begin(&config, &auth);
  
  // Check Firebase initialization status
  tft.setCursor(10, 50);
  if (Firebase.ready()) {
    tft.println("Firebase Ready!");
    Serial.println("Firebase initialized");
  } else {
    Serial.println("Firebase initialization failed");
    Serial.print("Error: ");
    Serial.println(config.signer.signupError.message.c_str());
    displayErrorScreen("Firebase Init Fail");
    while(1);
  }
}

void sendDataToFirebase() {
  String timestamp = String(millis() / 1000); // Simple timestamp
  
  // Create JSON objects for each node
  FirebaseJson node1Json;
  node1Json.set("temperature", node1Data.temperature);
  node1Json.set("soilMoisture", node1Data.soilMoisture);
  node1Json.set("nitrogen", node1Data.nitrogen);
  node1Json.set("phosphorous", node1Data.phosphorous);
  node1Json.set("potassium", node1Data.potassium);

  FirebaseJson node2Json;
  node2Json.set("temperature", node2Data.temperature);
  node2Json.set("soilMoisture", node2Data.soilMoisture);
  node2Json.set("nitrogen", node2Data.nitrogen);
  node2Json.set("phosphorous", node2Data.phosphorous);
  node2Json.set("potassium", node2Data.potassium);

  // Send data to Firebase
  String path1 = "/nodes/node_" + String(node1Data.nodeId) + "/" + timestamp;
  String path2 = "/nodes/node_" + String(node2Data.nodeId) + "/" + timestamp;
  
  if (Firebase.RTDB.setJSON(&fbdo, path1.c_str(), &node1Json) &&
      Firebase.RTDB.setJSON(&fbdo, path2.c_str(), &node2Json)) {
    Serial.println("Data sent to Firebase");
    tft.setTextColor(GREEN);
    tft.setCursor(10, 130);
    tft.print("Firebase: Updated");
  } else {
    Serial.println("Failed to send data");
    Serial.println("Reason: " + fbdo.errorReason());
    tft.setTextColor(RED);
    tft.setCursor(10, 130);
    tft.print("Firebase: Error");
  }
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
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(0, 0);
  tft.println("FieldNet Monitoring System");

  // Node 1 Data
  tft.setTextColor(YELLOW);
  tft.setCursor(0, 20);
  tft.print("Node 1 (172): ");
  if (node1Received) {
    tft.setTextColor(GREEN);
    tft.print("Online");
    tft.setTextColor(WHITE);
    
    tft.setCursor(0, 35);
    tft.print("Temp: ");
    tft.print(node1Data.temperature);
    tft.print(" C");
    
    tft.setCursor(0, 50);
    tft.print("Soil: ");
    tft.print(node1Data.soilMoisture);
    tft.print("%");
    
  
  } else {
    tft.setTextColor(RED);
    tft.print("Offline");
  }

  // Node 2 Data
  tft.setTextColor(YELLOW);
  tft.setCursor(0, 65);
  tft.print("Node 2 (60): ");
  if (node2Received) {
    tft.setTextColor(GREEN);
    tft.print("Online");
    tft.setTextColor(WHITE);
    
    tft.setCursor(0, 85);
    tft.print("Temp: ");
    tft.print(node2Data.temperature);
    tft.print(" C");
    
    tft.setCursor(0, 100);
    tft.print("Soil: ");
    tft.print(node2Data.soilMoisture);
    tft.print("%");
    
    tft.setCursor(0, 115);
    tft.print("NPK: ");
    tft.print(node2Data.nitrogen);
    tft.print("-");
    tft.print(node2Data.phosphorous);
    tft.print("-");
    tft.print(node2Data.potassium);
  } else {
    tft.setTextColor(RED);
    tft.print("Offline");
  }

  // Status line
  tft.setTextColor(WHITE);
  tft.setCursor(0, 150);
  tft.print("WiFi: ");
  tft.print(WiFi.SSID());
  tft.print(" (");
  tft.print(WiFi.RSSI());
  tft.print(" dBm)");
}