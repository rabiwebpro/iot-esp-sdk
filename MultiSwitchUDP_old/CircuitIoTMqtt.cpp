#include <Arduino.h>
#include <string.h>
#include "CircuitIoTMqtt.h"

#include "CircuitIoTConfig.h"
CircuitIoTMqtt::CircuitIoTMqtt(WiFiClientSecure& wifiClient) : PubSubClient() {
  wifiClient.setFingerprint(mqttCertFingerprint);
  this->_wifiClient = &wifiClient;
  PubSubClient::setClient(wifiClient);
  //PubSubClient::setCallback(onCallback);
  //PubSubClient::setCallback([this] (char* topic, byte* payload, unsigned int length) { this->onCallback(topic, payload, length); });
};

void CircuitIoTMqtt::setupMqtt(const char* clientId, const char* username, const char* password) {
  setServer(CIRCUITIOT_BROKER_URL, CIRCUITIOT_BROKER_PORT);
  this->_clientId = clientId;
  this->_username = username;
  this->_password = password;
  this->_id = "myid";
  this->_rootTopic = String("device/" + _clientId).c_str();

  reconnect();
  //sendLog("I'm born. Hello World!");
  //sendBootNotification();
}

void CircuitIoTMqtt::runMqtt() {
  if (!connected()) {
     reconnect();
   }
  loop();
}

bool CircuitIoTMqtt::reconnect() {
  while (!this->connected()) {
    if (!PubSubClient::connect(_clientId.c_str(), _username.c_str(), _password.c_str())) {
      Serial.printf("!! Error while connecting to the MQTT broker, code: %d\n", this->state());
      // Wait 1 second before retrying
      delay(1000);
      continue;
    }
    Serial.println("Connection to MQTT broker done :-)");

    String topic  = String(_rootTopic + "/#").c_str();
    Serial.printf("Subscribing to MQTT topic: %s", topic.c_str());
    this->subscribe(topic.c_str());
  }
  return true;
}

bool CircuitIoTMqtt::sendMessage(const char *topic, const char *payload) {
  Serial.printf("Sending message '%s' to topic '%s'\n", payload, topic);
  return this->publish(topic, payload);
}

bool CircuitIoTMqtt::sendBootNotification() {
  return sendMessage(String(_rootTopic + "/boot").c_str(), "1");
}

bool CircuitIoTMqtt::sendLog(const char *message) {
  if (!_doSendLogs) return false;
  return sendMessage(String(_rootTopic + "/log").c_str(), message);
}

void CircuitIoTMqtt::onCallbackkk(char* topic, byte* payload, unsigned int length) {
  Serial.printf("Message [%s] ", topic);
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char) payload[i]);
  }
  Serial.println();

  String s_topic = String(topic).substring(_rootTopic.length() + 1);
  Serial.println(s_topic);

  //_deviceCallback(topic);


  /*********
   * Perform action depending on received topic
   *********/
  // send my identity (kind of inventory)
  if (String(topic) == String(MQTT_INVENTORY_TOPIC)) {
    delay(random(1000));
    char msg[100];
    sprintf(msg, "Nice to meet you, I'm %s, sending on topic %s.", _id.c_str(), MQTT_SENSOR_ROOT);
    sendLog(msg);
    char topic[60];
    sprintf(topic, "%s/%s", MQTT_INVENTORY_TOPIC, "result");
    sendMessage(topic, _id.c_str());
  }
  // reboot the ESP
  else if (s_topic == "do_reboot") {
    /* See https://github.com/esp8266/Arduino/issues/1722#issuecomment-192624783
    * ESP.restart() does not work after the first restart after serial flashing.
    * However it works after manual reboot by power of RST switch.
    */
    Serial.printf("!! Reboot command catched (%s): rebooting...", topic);
    sendLog("OK I'm tired and I'll have to restart, please wait while I'm coming back. See you...");
    delay(1000);
    ESP.restart();
  }

  //enable or disable MQTT log
  else if (s_topic == "enable_mqtt_logs") {
    if ((char)payload[0] == '0') {
      if (_doSendLogs) sendLog("I'm stopping sending log message via MQTT. This is my last one. Bye.");
      _doSendLogs = false;
    } else {
      if (!_doSendLogs) {
        _doSendLogs = true;
        sendLog("Hello, log messages via MQTT are now activated. Nice to meet you :-)");
      }
    }
  }

  // launch firmware upgrade via HTTP download
  else if (s_topic == "do_upgrade") {
    Serial.printf("!! ESP firmware upgrade requested (%s); firmware: %s\n", topic, payload);
    char msg[70 + sizeof((char*)payload)];
    sprintf(msg, "Yeah, I'll become better by upgrading my fw to %s!\n", payload);
    sendLog(msg);
    HTTPUpdateResult ret = ESPhttpUpdate.update(*_wifiClient, String((char *)payload));
    if (ret == HTTP_UPDATE_FAILED) {
      // We reach this point only if the update did not work
      Serial.printf("!! Update error: %d - %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
      sprintf(msg, "Oh No, upgrade didn't work :-(: %d - %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
      sendLog(msg);
    }
  }

  // test MQTT message by controlling the builtin LED
  else if (s_topic == "ctrl_led") {
  Serial.printf("ctrl_led message received, value: %c \n", (char)payload[0]);
    if ((char)payload[0] == '0') {
      digitalWrite(LED_BUILTIN, HIGH);
      Serial.println("Turning off the builtin LED");
      sendLog("OK, I'm turning off the builtin LED.");
    } else {
      digitalWrite(LED_BUILTIN, LOW);
      Serial.println("Turning on the builtin LED.");
      sendLog("OK, I'm turning on the builtin LED.");
    }
  }

  // return some configuration and state information's
  else if (s_topic == "get_config") {
    char msg[100];
    sprintf(msg, "I'm sending info to '%s' topic.", MQTT_SENSOR_ROOT);
    sendLog(msg);
    sprintf(msg, "File %s compiled on %s %s.", __FILE__, __DATE__, __TIME__);
    sendLog(msg);
  }
}
