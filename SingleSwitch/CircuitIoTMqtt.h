#ifndef _CIRCUITIOT_MQTT_H_
#define _CIRCUITIOT_MQTT_H_

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ESP8266httpUpdate.h>


// Specific topic that all hardware on the network must listen to and answer.
// It's a kind of discovery topic to have a view on active hardware.
#define MQTT_INVENTORY_TOPIC "house/hardware/ping_all"
#define MQTT_ROOT "house/hardware"
#define MQTT_SENSOR_ROOT "house/sensors/"

class CircuitIoTMqtt : public PubSubClient {
  public:
    CircuitIoTMqtt(WiFiClientSecure& wifiClient);
    // to be called within Arduino setup() global function
    void setupMqtt(const char* clientId, const char* username, const char* password);
    bool reconnect();
    void run();
    // send message to a specific mqtt log topic
    bool sendLog(const char *message);
    //void setDeviceCallback(void (*deviceCallback)(char* topic));
    String _rootTopic;

  private:
    WiFiClientSecure* _wifiClient;
    String _clientId;
    String _username;
    String _password;
    //String _rootTopic;
    String _id;
    bool _doSendLogs = false; // set to false to avoid sending log message via MQTT (/log topic)

    //void callback(char* topic, byte* payload, unsigned int length);
    bool connect();
    bool sendBootNotification();
    //void (*_deviceCallback)(char* topic);

  protected:
    //virtual void onCallback(char* topic, byte* payload, unsigned int length);
    // Generic function to send a message to MQTT broker
    bool sendMessage(const char *topic, const char *payload);
    virtual void onCallbackkk(char* topic, byte* payload, unsigned int length);
};

#endif
