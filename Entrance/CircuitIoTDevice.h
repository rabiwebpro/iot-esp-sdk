#ifndef _CIRCUITIOT_DEVICE_H_
#define _CIRCUITIOT_DEVICE_H_

#include "CircuitIoTMqtt.h"
#include "CircuitIoTUdp.h"
#include "Base64.h"

#include <ArduinoJson.h>

#if defined(ESP8266) || defined(ESP32)
#include <functional>
#define DEVICE_CALLBACK_SIGNATURE std::function<void(const char*, const char*, const char*)> deviceCallback
#else
#define DEVICE_CALLBACK_SIGNATURE void (*deviceCallback)(const char*, const char*, const char*)
#endif


class CircuitIoTDevice: public CircuitIoTMqtt, public CircuitIoTUdp {
  public:
    CircuitIoTDevice(WiFiClientSecure& wifiClient, char token[]);
    void setDeviceCallback(DEVICE_CALLBACK_SIGNATURE);
    void setup();
    void run();
    String _deviceId;
    String _deviceSecret;
    String _deviceCategory;
    char* _token;
    String _devices[16]; 
    String _subscriptionRoot;
    String _publishRoot;
    int _deviceSize;  
  private:
    void parseToken();
    DEVICE_CALLBACK_SIGNATURE;    
  protected:
    virtual void onCallback(char* topic, byte* payload, unsigned int length);
        
};

#endif
