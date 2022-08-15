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

//  Serial.println(".");
//  Serial.printf("Cleint ID [%s] ", _deviceId.c_str());
//  Serial.println(".");
//  Serial.println(".");
//  Serial.printf("Username [%s] ", _deviceId.c_str());
//  Serial.println(".");
//  Serial.println(".");
//  Serial.printf("Password [%s] ", _deviceSecret.c_str());
//  Serial.println(".");
  
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


    int deviceSize = doc["childs"].size();
    _deviceSize = deviceSize+1;
    //_devices = new String[deviceSize];
    //Serial.println(deviceSize);
    if(deviceSize > 0){
      _devices[0] = id;
      for (int i =0; i< deviceSize; i++) {
        String dev = doc["childs"][i];
        //Serial.println(dev);
        _devices[i+1] = dev;
      }
//      Serial.println(".......");
//      Serial.println(_devices[0]);
//      Serial.println(_devices[1]);
    }
    
    
    

    
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

  Serial.println(".......");
  //Serial.println(property);

  if(property.indexOf("/") > 0){
    //Serial.println("contains backslash");
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
    //Serial.println(strings[0]);
    //Serial.println(strings[1]);

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
  //int deviceSize = _devices.length;

//int count=0;
//  for (int i = 0; i < (sizeof(_devices) / sizeof(_devices[0])); i++) {
//    if (_devices[i] != "") {
//      count++;
//    }
//  }
  
Serial.println(_deviceSize);
  for (int i =1; i< _deviceSize; i++) {
    String ppTopic = String(_publishRoot + "/" + _devices[i] + "/" + tpcc).c_str();
    Serial.println("..........");
    Serial.printf("publishing sub devices topic [%s] ", ppTopic.c_str());
    publish(ppTopic.c_str(), val);
  }
}
  
}

void CircuitIoTDevice::setDeviceCallback(DEVICE_CALLBACK_SIGNATURE) {
    this->deviceCallback = deviceCallback;
}
