#include <Arduino.h>
#ifdef ESP8266 
       #include <ESP8266WiFi.h>
#endif 
#ifdef ESP32   
       #include <WiFi.h>
#endif

#include "CircuitIoTMotionSensor.h"

#define SensorPin D2  //D2

//WiFi Status LED
#define wifiLed    D4   //D4

bool isTriggered = false;

const char* ssid = "Red Bull";
const char* password = "sohel@1996";

char token[] = "eyJpZCI6ImRkZDBOTGdDMzZUWnRKWG5zIiwic2VjcmV0IjoiMXRvX0hOY0RmS3c5JFpCV3lCM0MiLCJjYXQiOiJzZW5zb3IiLCJjaGlsZHMiOltdfQ==";

WiFiClientSecure espClient;
CircuitIoTMotionSensor motionSensor(espClient, token);

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

void setupMotionSensor() {  
 delay(10);
 motionSensor.setup();
}


void setup() {
  Serial.begin(9600);

  pinMode(SensorPin, INPUT);

  pinMode(wifiLed, OUTPUT);

  //During Starting WiFi LED should TURN OFF
  digitalWrite(wifiLed, HIGH);
  
  
  Serial.println("Connecting..."); 
  setupWifi();
  setupMotionSensor();
}

void loop() {
  if (!motionSensor.connected()) {
    digitalWrite(wifiLed, HIGH);
  }
  else{
    digitalWrite(wifiLed, LOW);
  }  
  motionSensor.run();
  delay(1000);

   if(digitalRead(SensorPin) == 0){
       Serial.println("DETECTED...");
       if (isTriggered==false) motionSensor.pushMotionDetected("1");
       isTriggered = true;
   }else{
       Serial.println("Not detected...");
       if (isTriggered==true)  motionSensor.pushMotionDetected("0");
       isTriggered = false;
   }

}
