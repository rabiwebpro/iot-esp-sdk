#include <Arduino.h>
#ifdef ESP8266 
       #include <ESP8266WiFi.h>
#endif 
#ifdef ESP32   
       #include <WiFi.h>
#endif

#include "CircuitIoTSwitch.h"

const char* ssid = "NoNet_2";
const char* password = "Developer@4";

//char token[] = "eyJpZCI6ImRkZDJRNUtDeWxjYnJnaU9zIiwic2VjcmV0Ijoia05PdkNrNl9DNm9CenhIR0g3QzMiLCJjYXQiOiJTV0lUQ0giLCJjaGlsZHMiOlsiZGRkV2phdElrdEk1akJ6M1oiLCJkZGRJbFB5cW1jMEt6NHB6MiJdfQ==";
char token[] = "eyJpZCI6ImRkZGxoSWVQYWxGMkRDc0ZBIiwic2VjcmV0IjoiMEYjMUtxWm0zVFFVaDF2ZWlabFEiLCJjYXQiOiJTV0lUQ0giLCJjaGlsZHMiOltdfQ==";

WiFiClientSecure espClient;
CircuitIoTSwitch powerSwitch(espClient, token);

void setupWifi() {
 delay(10);
 WiFi.begin(ssid, password);
 while (WiFi.status() != WL_CONNECTED) {
 delay(500);
 Serial.print(".");
 }

randomSeed(micros());
 
 Serial.println("");
 Serial.println("WiFi connected");
 Serial.println("IP address: ");
 Serial.println(WiFi.localIP());
}


void powerStateChangeCallback(const char* value) {
  Serial.println(".");
  Serial.printf("Power state received: [%s] ", value);
  Serial.println(".");
}

void setupSwitch() {  
 delay(10);
 powerSwitch.setup();
 powerSwitch.onPowerStateChange(powerStateChangeCallback);
}


void setup() {
  Serial.begin(9600);
  Serial.println("Connecting..."); 
  setupWifi();
  setupSwitch();
}

void loop() {
  powerSwitch.run();
  delay(5000);

}
