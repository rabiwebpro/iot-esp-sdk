#include <Arduino.h>
#ifdef ESP8266 
       #include <ESP8266WiFi.h>
#endif 
#ifdef ESP32   
       #include <WiFi.h>
#endif

#include "CircuitIoTMultiSwitch.h"

#define RelayPin1 D1  //D1
#define RelayPin2 D2  //D2
#define RelayPin3 D5  //D5
#define RelayPin4 D6  //D6

#define SwitchPin1 D7  //D7
#define SwitchPin2 1   //TX 
#define SwitchPin3 D3  //D3
#define SwitchPin4 3   //RX

//WiFi Status LED
#define wifiLed    D4   //D0

int toggleState_1 = 1; //Define integer to remember the toggle state for relay 1
int toggleState_2 = 1; //Define integer to remember the toggle state for relay 2
int toggleState_3 = 1; //Define integer to remember the toggle state for relay 3
int toggleState_4 = 1; //Define integer to remember the toggle state for relay 4

const char* ssid = "NoNet_2";
const char* password = "Developer@4";

char token[] = "eyJpZCI6ImRkZDFiRXh2YTFCaFBlVWQ0Iiwic2VjcmV0IjoiSUR4Vk1rallvZWlYUUFaazgzX28iLCJjYXQiOiJzd2l0Y2giLCJjaGlsZHMiOlsiZGRkZkloTUpwR0RrbzhuVVciLCJkZGR5d09UUld5eWthTGZxRCIsImRkZEJSSUlTRVowQkE5NVBIIiwiZGRkeDltaG96Z2dIbFdybXkiXX0=";

WiFiClientSecure espClient;
CircuitIoTMultiSwitch powerSwitch(espClient, token);

void setupWifi() {
 delay(10);
 WiFi.begin(ssid, password);
 delay(3000);
// while (WiFi.status() != WL_CONNECTED) {
// delay(500);
// Serial.print(".");
 //}

randomSeed(micros());
 
// Serial.println("");
// Serial.println("WiFi connected");
// Serial.println("IP address: ");
// Serial.println(WiFi.localIP());
}


void powerStateChangeCallback(int deviceIndex, const char* value) {
  Serial.println(".");
  Serial.printf("Device index: [%s] ", String(deviceIndex));
  Serial.println(".");
  Serial.printf("Power state received: [%s] ", value);
  Serial.println(".");

  if(deviceIndex == 1){
    if (strcmp(value, "1") == 0){
      Serial.println("device 1 on");
      digitalWrite(RelayPin1, LOW);
      //toggleState_1 = 1;
    }else{
      Serial.println("device 1 off");
      digitalWrite(RelayPin1, HIGH);
      //toggleState_1 = 0;
    }
  }
  if(deviceIndex == 2){
    if (strcmp(value, "1") == 0){
      //Serial.println("device 2 on");
      digitalWrite(RelayPin2, LOW);
      //toggleState_2 = 1;
    }else{
      //Serial.println("device 2 off");
      digitalWrite(RelayPin2, HIGH);
      //toggleState_2 = 0;
    }
  }
  if(deviceIndex == 3){
    if (strcmp(value, "1") == 0){
      //Serial.println("device 3 on");
      digitalWrite(RelayPin3, LOW);
    }else{
      //Serial.println("device 3 off");
      digitalWrite(RelayPin3, HIGH);
    }
  }
  if(deviceIndex == 4){
    if (strcmp(value, "1") == 0){
      //Serial.println("device 4 on");
      digitalWrite(RelayPin4, LOW);
    }else{
      //Serial.println("device 4 off");
      digitalWrite(RelayPin4, HIGH);
    }
  }
  
}

void setupSwitch() {  
 delay(10);
 powerSwitch.setup();
 powerSwitch.onPowerStateChange(powerStateChangeCallback);
}

void manualControl(){
    //Manual Switch 1
    if (digitalRead(SwitchPin1) == LOW){
      delay(100); 
      if(toggleState_1 == 0){
        //Serial.println("Turning on switch 1");
        digitalWrite(RelayPin1, LOW);
        toggleState_1 = 1;
        if (powerSwitch.connected()) powerSwitch.pushPowerStateChange(1, "1");
      }
    }else{
      delay(100);
      if(toggleState_1 == 1){
        //Serial.println("Turning off switch 1");
        digitalWrite(RelayPin1, HIGH);
        toggleState_1 = 0;
        if (powerSwitch.connected()) powerSwitch.pushPowerStateChange(1, "0");
      }
    }
    //Manual Switch 2
    if (digitalRead(SwitchPin2) == LOW){
      delay(100); 
      if(toggleState_2 == 0){
        //Serial.println("Turning on switch 2");
        digitalWrite(RelayPin2, LOW);
        toggleState_2 = 1;
        if (powerSwitch.connected()) powerSwitch.pushPowerStateChange(2, "1");
      }
    }else{
      delay(100);
      if(toggleState_2 == 1){
        //Serial.println("Turning off switch 2");
        digitalWrite(RelayPin2, HIGH);
        toggleState_2 = 0;
        if (powerSwitch.connected()) powerSwitch.pushPowerStateChange(2, "0");
      }
    }
    //Manual Switch 3
    if (digitalRead(SwitchPin3) == LOW){
      delay(100); 
      if(toggleState_3 == 0){
        //Serial.println("Turning on switch 3");
        digitalWrite(RelayPin3, LOW);
        toggleState_3 = 1;
        if (powerSwitch.connected()) powerSwitch.pushPowerStateChange(3, "1");
      }
    }else{
      delay(100);
      if(toggleState_3 == 1){
        //Serial.println("Turning off switch 3");
        digitalWrite(RelayPin3, HIGH);
        toggleState_3 = 0;
        powerSwitch.pushPowerStateChange(3, "0");
      }
    }
    //Manual Switch 4
    if (digitalRead(SwitchPin4) == LOW){
      delay(100); 
      if(toggleState_4 == 0){
        //Serial.println("Turning on switch 4");
        digitalWrite(RelayPin4, LOW);
        toggleState_4 = 1;
        if (powerSwitch.connected()) powerSwitch.pushPowerStateChange(4, "1");
      }
    }else{
      delay(100);
      if(toggleState_4 == 1){
        //Serial.println("Turning off switch 4");
        digitalWrite(RelayPin4, HIGH);
        toggleState_4 = 0;
        if (powerSwitch.connected()) powerSwitch.pushPowerStateChange(4, "0");
      }
    }
    delay(100);
}



void setup() {
  Serial.begin(9600);

  pinMode(RelayPin1, OUTPUT);
  pinMode(RelayPin2, OUTPUT);
  pinMode(RelayPin3, OUTPUT);
  pinMode(RelayPin4, OUTPUT);

  pinMode(SwitchPin1, INPUT_PULLUP);
  pinMode(SwitchPin2, INPUT_PULLUP);
  pinMode(SwitchPin3, INPUT_PULLUP);
  pinMode(SwitchPin4, INPUT_PULLUP);

  pinMode(wifiLed, OUTPUT);
  //During Starting all Relays should TURN OFF
  digitalWrite(RelayPin1, HIGH);
  digitalWrite(RelayPin2, HIGH);
  digitalWrite(RelayPin3, HIGH);
  digitalWrite(RelayPin4, HIGH);

  //During Starting WiFi LED should TURN OFF
  digitalWrite(wifiLed, HIGH);
  
  
  Serial.println("Connecting..."); 
  setupWifi();
  setupSwitch();
}

void loop() {
  if (!powerSwitch.connected()) {
    digitalWrite(wifiLed, HIGH);
  }
  else{
    digitalWrite(wifiLed, LOW);
  }
  manualControl();  
  powerSwitch.run();
  delay(100);   

}
