#ifndef _CIRCUITIOT_MQTT_H_
#define _CIRCUITIOT_MQTT_H_

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ESP8266httpUpdate.h>



#define MQTT_INVENTORY_TOPIC "house/hardware/ping_all"
#define MQTT_ROOT "house/hardware"
#define MQTT_SENSOR_ROOT "house/sensors/"

class CircuitIoTMqtt : public PubSubClient {
  public:
    CircuitIoTMqtt(WiFiClientSecure& wifiClient);
    // to be called within Arduino setup() global function
    void setupMqtt(const char* clientId, const char* username, const char* password);
    bool reconnect();
    void runMqtt();
    bool sendLog(const char *message);
    String _rootTopic;

  private:
    WiFiClientSecure* _wifiClient;
    String _clientId;
    String _username;
    String _password;
    //String _rootTopic;
    String _id;
    bool _doSendLogs = false; 
    bool connect();
    bool sendBootNotification();

  protected:
    bool sendMessage(const char *topic, const char *payload);
    virtual void onCallbackkk(char* topic, byte* payload, unsigned int length);
};

#endif
