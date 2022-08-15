#ifndef _CIRCUITIOT_SWITCH_H_
#define _CIRCUITIOT_SWITCH_H_

#include "CircuitIoTDevice.h"

#if defined(ESP8266) || defined(ESP32)
#include <functional>
#define POWER_STATE_CALLBACK_SIGNATURE std::function<void(const char*)> powerStateCallback
#else
#define POWER_STATE_CALLBACK_SIGNATURE void (*powerStateCallback)(const char*)
#endif
#define PROP_POWER "power"

class CircuitIoTSwitch: public CircuitIoTDevice {
  
  public:  
    CircuitIoTSwitch(WiFiClientSecure& wifiClient, char token[]);
    void onPowerStateChange(POWER_STATE_CALLBACK_SIGNATURE);
    bool pushPowerStateChange(const char* value);
  private:
    POWER_STATE_CALLBACK_SIGNATURE;
  protected:
    virtual void onDeviceStateChange(const char* property, const char* value);
};

#endif
