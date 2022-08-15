#ifndef _CIRCUITIOT_DEVICE_H_
#define _CIRCUITIOT_DEVICE_H_

#include "CircuitIoTMqtt.h"
#include "Base64.h"

#include <ArduinoJson.h>

#define MQTT_SENSOR_TOPIC "/house/sensors/water/city"

#if defined(ESP8266) || defined(ESP32)
#include <functional>
#define DEVICE_CALLBACK_SIGNATURE std::function<void(const char*, const char*, const char*)> deviceCallback
#else
#define DEVICE_CALLBACK_SIGNATURE void (*deviceCallback)(const char*, const char*, const char*)
#endif


class CircuitIoTDevice: public CircuitIoTMqtt {
  public:
    CircuitIoTDevice(WiFiClientSecure& wifiClient, char token[]);
    void setDeviceCallback(DEVICE_CALLBACK_SIGNATURE);
    void setup();
    bool sendLiter(float value);
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
