#include "CircuitIoTDevice.h"

CircuitIoTDevice::CircuitIoTDevice(WiFiClientSecure& wifiClient, char token[]) : CircuitIoTMqtt(wifiClient) {
  this->_token = token;
  //setDeviceCallback(NULL);
  PubSubClient::setCallback([this] (char* topic, byte* payload, unsigned int length) { this->onCallback(topic, payload, length); });
 };

 void CircuitIoTDevice::setup() {
  parseToken();
  delay(1000);
  this->_subscriptionRoot = String("device/" + _deviceId).c_str();
  this->_publishRoot = String("client/" + _deviceId).c_str();
  setupMqtt(_deviceId.c_str(), _deviceId.c_str(), _deviceSecret.c_str());
 }

 void CircuitIoTDevice::parseToken(){
    StaticJsonDocument<200> doc;

    int inputStringLength = strlen(_token);
    
    int decodedLength = Base64.decodedLength(_token, inputStringLength);
    char decodedString[decodedLength];
    Base64.decode(decodedString, _token, inputStringLength);
    
    DeserializationError error = deserializeJson(doc, decodedString);

     // Test if parsing succeeds.
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }

    String id = doc["id"];
    String secret = doc["secret"];
    String cat = doc["cat"];
    
    _deviceId = id;
    _deviceSecret = secret;
    _deviceCategory = cat;


    //int deviceSize = doc["childs"].size();
    //String test = "test";
    //_childs[0] = & test;
    //_childs = doc["childs"];
    //for (int i =0; i< deviceSize; i++) {
      //const char* device = doc["childs"][i];
     //_childs[i] = const_cast<char *>(device);
     //_childs[i] = doc["childs"][i];
     //String device = doc["childs"][i];
     //_childs[i] = & device;
    //}
}

bool CircuitIoTDevice::sendLiter(float value) {
  // Send a value in liter.  This should be the best way to do it:
  // converting sensor data to physical value (liter) and then send to db.
  char msg[16];
  sprintf(msg, "%.2f", value);
  return sendMessage(MQTT_SENSOR_TOPIC, msg);
}


void CircuitIoTDevice::onCallback(char* topic, byte* payload, unsigned int length) {

  char val[length+1];
  for (int i=0;i<length;i++) 
  {
    val[i] = (char)payload[i];
  }
  val[length] = NULL;

  
  String property = String(topic).substring(_rootTopic.length() + 1);
  if (deviceCallback != NULL) deviceCallback(property.c_str(), val);

  String publishTopic = String(_publishRoot + "/power").c_str();
  publish(publishTopic.c_str(), val);
  
  
}

void CircuitIoTDevice::setDeviceCallback(DEVICE_CALLBACK_SIGNATURE) {
    this->deviceCallback = deviceCallback;
}
