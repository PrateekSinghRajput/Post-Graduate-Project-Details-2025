#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <esp_now.h>
#include <WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_SSD1306.h>

// NPK Sensor Definitions
#define DE_RE 4  // Control pin for RS485
HardwareSerial rs485(1);  // Using UART1
LiquidCrystal_I2C lcd(0x27, 16, 2); // LCD I2C Address 0x27, 16 columns x 2 rows

const byte nitro[] = {0x01, 0x03, 0x00, 0x1E, 0x00, 0x01, 0xE4, 0x0C};
const byte phos[]  = {0x01, 0x03, 0x00, 0x1F, 0x00, 0x01, 0xB5, 0xCC};
const byte pota[]  = {0x01, 0x03, 0x00, 0x20, 0x00, 0x01, 0x85, 0xC0};
byte values[7];

// Temperature Sensor Definitions
#define ONE_WIRE_BUS 12        // GPIO pin for DS18B20
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature ds18b20(&oneWire);

// OLED Definitions
#define OLED_SDA 21            // GPIO pin for OLED SDA
#define OLED_SCL 22            // GPIO pin for OLED SCL
#define OLED_RST -1            // Reset pin (-1 if not available)
#define SCREEN_WIDTH 128       // OLED display width
#define SCREEN_HEIGHT 64       // OLED display height
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

// ESP-NOW Definitions
typedef struct struct_message {
  int nodeId;
  float temperature;
  byte nitrogen;
  byte phosphorous;
  byte potassium;
} struct_message;

struct_message myData;
uint8_t broadcastAddress[] = {0x78, 0x42, 0x1C, 0x6C, 0xE1, 0x94}; // Update with receiver's MAC
esp_now_peer_info_t peerInfo;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

byte readSensor(const byte* cmd) {
  digitalWrite(DE_RE, HIGH);  // Enable transmit mode
  delay(10);
  rs485.write(cmd, 8);
  delay(10);
  digitalWrite(DE_RE, LOW);   // Enable receive mode

  int index = 0;
  unsigned long startTime = millis();
  while (millis() - startTime < 200) {  
    if (rs485.available()) {
      values[index++] = rs485.read();
      if (index >= 7) break;  // Stop when expected bytes are received
    }
  }

  if (index < 7) {
    Serial.println("Error: Incomplete response");
    return 0;
  }

  return values[4];  // Return the data byte
}

void displayValues(byte n, byte p, byte k, float temp) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("N:"); lcd.print(n); lcd.print(" P:"); lcd.print(p);
  lcd.setCursor(0, 1);
  lcd.print("K:"); lcd.print(k); lcd.print(" T:"); lcd.print(temp,1);
  
  display.clearDisplay();
  display.setCursor(0,0);
  display.print("Node: ");
  display.println(myData.nodeId);
  display.print("Temp: ");
  display.print(temp,1);
  display.println(" C");
  display.print("N:"); display.print(n); 
  display.print(" P:"); display.print(p);
  display.print(" K:"); display.println(k);
  display.display();
}

void setup() {
  Serial.begin(115200);
  
  // Initialize NPK sensor
  rs485.begin(9600, SERIAL_8N1, 16, 17);  // RX=16, TX=17
  pinMode(DE_RE, OUTPUT);
  digitalWrite(DE_RE, LOW); // Receiver mode
  
  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("NPK Sensor");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  
  // Initialize OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Initializing...");
  display.display();
  
  // Initialize DS18B20
  ds18b20.begin();
  
  // Initialize ESP-NOW
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  esp_now_register_send_cb(OnDataSent);
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
  
  // Set node ID based on MAC address (last byte)
  myData.nodeId = ESP.getEfuseMac() & 0xFF;
  
  delay(2000);
}

void loop() {
  // Read NPK values
  byte val1 = readSensor(nitro);
  delay(250);
  byte val2 = readSensor(phos);
  delay(250);
  byte val3 = readSensor(pota);
  delay(250);

  // Read temperature
  ds18b20.requestTemperatures();
  float temperature = ds18b20.getTempCByIndex(0);
  
  // Display values
  displayValues(val1, val2, val3, temperature);
  
  // Prepare data for transmission
  myData.temperature = temperature;
  myData.nitrogen = val1;
  myData.phosphorous = val2;
  myData.potassium = val3;
  
  // Send via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  } else {
    Serial.println("Error sending the data");
  }
  
  delay(10000); // Send every 10 seconds
}