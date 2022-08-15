#include "CircuitIoTDevice.h"

CircuitIoTDevice::CircuitIoTDevice(WiFiClientSecure& wifiClient, char token[]) : CircuitIoTMqtt(wifiClient), CircuitIoTUdp() {
  this->_token = token;
  PubSubClient::setCallback([this] (char* topic, byte* payload, unsigned int length) { this->onCallback(topic, payload, length); });
  CircuitIoTUdp::setUdpCallback([this] (char* topic, byte* payload, unsigned int length) { this->onCallback(topic, payload, length); });
 };

 void CircuitIoTDevice::setup() {
  parseToken();
  delay(1000);
  this->_subscriptionRoot = String("device/" + _deviceId).c_str();
  this->_publishRoot = String("client/" + _deviceId).c_str();

//  Serial.println("clientId: ");
//  Serial.println(_deviceId.c_str());
//
//  Serial.println("clientSecret: ");
//  Serial.println(_deviceSecret.c_str());
  
  setupMqtt(_deviceId.c_str(), _deviceId.c_str(), _deviceSecret.c_str());

  setupUdp();
 }

 void CircuitIoTDevice::run() {
  runUdp();
  CircuitIoTMqtt::runMqtt();
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


    int deviceSize = doc["childs"].size();
    _deviceSize = deviceSize+1;
    if(deviceSize > 0){
      _devices[0] = id;
      for (int i =0; i< deviceSize; i++) {
        String dev = doc["childs"][i];
        _devices[i+1] = dev;
      }
    }
}


void CircuitIoTDevice::onCallback(char* topic, byte* payload, unsigned int length) {

  char val[length+1];
  for (int i=0;i<length;i++) 
  {
    val[i] = (char)payload[i];
  }
  val[length] = NULL;

  
  String property = String(topic).substring(_rootTopic.length() + 1);

  if(property.indexOf("/") > 0){
    char *strings[2];
    char *ptr = NULL;
    byte index = 0;
    char* np = (char *) property.c_str();
    ptr = strtok(np, "/");
    while (ptr != NULL)
     {
        strings[index] = ptr;
        index++;
        ptr = strtok(NULL, "/");
     }

    if (deviceCallback != NULL) deviceCallback(strings[0], strings[1], val);
  }else{
    
    if (deviceCallback != NULL) deviceCallback(_deviceId.c_str(), property.c_str(), val);
  }

  //Need to modify to publish for sub devices as well
String tpcc = String(topic).substring(_rootTopic.length() + 1);
String pbTopic = String(_publishRoot + "/" + tpcc).c_str();
publish(pbTopic.c_str(), val);

//If parent device
if(tpcc.indexOf("/") <= 0){
  for (int i =1; i< _deviceSize; i++) {
    String ppTopic = String(_publishRoot + "/" + _devices[i] + "/" + tpcc).c_str();
    publish(ppTopic.c_str(), val);
  }
}
  
}

void CircuitIoTDevice::setDeviceCallback(DEVICE_CALLBACK_SIGNATURE) {
    this->deviceCallback = deviceCallback;
}
