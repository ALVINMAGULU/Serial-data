#include "Arduino.h"
#include <ArduinoJson.h>
#include "SerialData.h"


String SerialData::boolToString(bool Bool) {
  if (Bool == true) {
    return "true";
  } else {
    return "false";
  }
}

//listen for any incoming updates, or confirmed updates
void SerialData::listener(String buffer) {
  DynamicJsonBuffer JSON;
  JsonObject& myObject = JSON.parseObject(buffer);


  if (myObject.success() && myObject["id"] == id) {
    isConnected = 0;
    String type = myObject["type"];
    if (type == "u") {
      jsonToArray(myObject["data"]);
      newDataRecieved = true;
      update = false;
    } else if (type == "rtu") {
      jsonToArray(myObject["data"]);
      stream.println(callJson("updated", ""));
      newDataRecieved = true;
    } else if (type == "ru") {
       stream.println(callJson("u",arrayToJson()));      
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
  array[newAddress][0] = "String";
  array[newAddress][1] = name;
  array[newAddress][2] = value;
  newAddress++;
}

void SerialData::addData(String name, int value) {
  array[newAddress][0] = "int";
  array[newAddress][1] = name;
  array[newAddress][2] = String(value);
  newAddress++;
}
String SerialData::data(String name) {
  String data;
  for (int i; i <= arrayLength; i++) {
    if (array[i][1] == name) {
      data = array[i][2];
    }
  }
  return data;
}


//copy data
void SerialData::jsonToArray(String updateJson) {
  DynamicJsonBuffer JSON;
  JsonObject& myObject = JSON.parseObject(updateJson);
  if (myObject.success()) {
    for (int i; i <= arrayLength; i++) {
      if (array[i][0] == "String") {
        array[i][2] = myObject[array[i][1]].as<String>();
      } else if (array[i][0] == "int") {
        array[i][2] = String(myObject[array[i][1]].as<String>());
      } else if (array[i][0] == "bool") {
        array[i][2] = boolToString(myObject[array[i][1]].as<bool>());
      }  //
    }
  }
}

//export data to json
String SerialData::arrayToJson() {
  String string;
  StaticJsonBuffer<300> Jsonbuffer;
  JsonObject& JSONencoder = Jsonbuffer.createObject();

  int address = 0;
  for (address; address <= arrayLength; address++) {
    String dataType = array[address][0];
    if (dataType == "String") {
      JSONencoder[array[address][1]] = array[address][2];
    } else if (dataType == "int") {
      JSONencoder[array[address][1]] = array[address][2].toInt();
    } else if (dataType == "bool") {
      if (array[address][2] == "false") {
        JSONencoder[array[address][1]] = false;
      } else if (array[address][2] == "true") {
        JSONencoder[array[address][1]] = true;
      }
    }
  }

  JSONencoder.printTo(string);
  return string;
}

//build json for serial communication
String SerialData::callJson(String type, String data) {
  String string;
  StaticJsonBuffer<300> Jsonbuffer;
  JsonObject& JSONencoder = Jsonbuffer.createObject();
  JSONencoder["id"] = id;
  JSONencoder["type"] = type;
  JSONencoder["data"] = data;
  JSONencoder.printTo(string);
  return string;
}

//Directly update data array
void SerialData::updateData(String name, String data) {
  for (int i; i <= arrayLength; i++) {
    if (array[i][1] == name) {
      array[i][2] = data;
      isChanged = true;
    }
  }
}

void SerialData::updateData(String name, int data) {
  for (int i; i <= arrayLength; i++) {
    if (array[i][1] == name) {
      array[i][2] = String(data);
      isChanged = true;
    }
  }
}

//update data from other side of serial
void SerialData::requestToUpdate() {
  String data = arrayToJson();
  StaticJsonBuffer<300> Jsonbuffer;
  JsonObject& JSONencoder = Jsonbuffer.createObject();
  JSONencoder["id"] = id;
  JSONencoder["type"] = "rtu";
  JSONencoder["data"] = data;
  JSONencoder.printTo(stream);
  stream.println();
}

//run object
void SerialData::run(String serialBuffer) {
  if (connectionVerified == true) {
    if (isChanged == true) {
      requestToUpdate();
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