#include <WiFi.h>

void setup() {
  Serial.begin(115200);
  
  // Initialize WiFi
  WiFi.mode(WIFI_MODE_STA);  // Set WiFi to station mode
  
  // Give some time for initialization
  delay(1000);
  
  // Now get the MAC address
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());
}

void loop() {
}