#include "Arduino.h"

#include "SerialData.h"

//listen for any incoming updates, or confirmed updates
void SerialData::listener(String buffer) {

  String data;

  JSONencoder.printTo(data);
  DynamicJsonBuffer JSON;
  JsonObject& myObject = JSON.parseObject(buffer);


  if (myObject.success() && myObject["id"] == id) {
    isConnected = 0;
    String type = myObject["type"];
    if (type == "u") {

      newDataRecieved = true;
      update = false;
    } else if (type == "rtu") {

      stream.println(callJson("updated", ""));
      newDataRecieved = true;
    } else if (type == "ru") {
       stream.println(callJson("u",data));
    } else if (type == "updated") {
      isChanged = false;
    } else if (type == "vc") {
      stream.println(callJson("verified", ""));

    } else if (type == "verified") {
      connectionVerified = true;
    }
  }
}


//add data
void SerialData::addData(String name, String value) {
  JSONencoder[name] = value;
}

void SerialData::addData(String name, int value) {
  JSONencoder[name] = value;
}

String SerialData::data(String name) {
 String data = JSONencoder[name].as<String>();
  return data;
}

//build json for serial communication
String SerialData::callJson(String type, String data) {
  String string;
  StaticJsonBuffer<300> Jsonbuffer;
  JsonObject& callJson = Jsonbuffer.createObject();
  callJson["id"] = id;
  callJson["type"] = type;
  callJson["data"] = data;
  callJson.printTo(string);
  return string;
}

//run object
void SerialData::run(String serialBuffer) {
  if (connectionVerified == true) {
    if (isChanged == true) { 
      String data;
      JSONencoder.printTo(data);   
      stream.println(callJson("rqu",data));
      isConnected++;
    }
    if (update == true) {
      stream.println(callJson("ru", ""));
      isConnected++;
    }
  } else {
    verifyConnection();
  }
  listener(serialBuffer);
}

int SerialData::serialConnected() {
  int state;
  if (isConnected <= 3 && isConnected >= 0) {
    state = 1;
  } else if (isConnected > 3) {
    state = 0;
  } else if (isConnected < 0) {
    state = 2;
  }
  return state;
}

void SerialData::verifyConnection() {
  stream.println(callJson("vc", ""));
}