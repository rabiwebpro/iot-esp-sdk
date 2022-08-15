#include "CircuitIoTMultiSwitch.h"

CircuitIoTMultiSwitch::CircuitIoTMultiSwitch(WiFiClientSecure& wifiClient, char token[]) : CircuitIoTDevice(wifiClient, token) {
    //onPowerStateChange(NULL);  
    CircuitIoTDevice::setDeviceCallback([this] (const char* deviceId, const char* property, const char* value) { this->onDeviceStateChange(deviceId, property, value); });
 };

void CircuitIoTMultiSwitch::onDeviceStateChange(const char* deviceId, const char* property, const char* value) {
//  Serial.println(".");
//  Serial.printf("Device ID [%s] ", deviceId);
//  Serial.println(".");

  int deviceIndex = 0;
  for (int i =1; i< _deviceSize; i++) {
    //Serial.println(".");
    if (strcmp(deviceId, _devices[i].c_str()) == 0) {
      deviceIndex = i;
      break;
    }
  }
  
//  Serial.printf("Device Index [%s] ", String(deviceIndex));
//  Serial.println(".");
  
  if (powerStateCallback != NULL && (String(property) == PROP_POWER)) powerStateCallback(deviceIndex, value);
}


void CircuitIoTMultiSwitch::onPowerStateChange(POWER_STATE_CALLBACK_SIGNATURE)
{
    this->powerStateCallback = powerStateCallback;
}

bool CircuitIoTMultiSwitch::pushPowerStateChange(int deviceIndex, const char* value){
  String publishTopic = String(_publishRoot + "/power").c_str();
  Serial.printf("Publishing to [%s] ", publishTopic.c_str());
  return sendMessage(publishTopic.c_str(), value);
}
