#ifndef SERIAL_DATA_H
#define SERIAL_DATA_H
#include <Arduino.h>
#include <ArduinoJson.h>



class SerialData {
private:
  
  String type;
  String boolToString(bool Bool);
  void listener(String buffer);
  int isConnected = -1;
  void verifyConnection();
  bool connectionVerified = false;
  StaticJsonBuffer<50> Jsonbuffer;
  JsonObject& JSONencoder = Jsonbuffer.createObject();
  
protected:
  Stream& stream;


public:
  SerialData(String id, Stream& str)
    : stream(str) {
    this->id = id;
    
    JSONencoder["id"] = id;
  };
  bool isChanged = false;
  bool update = false;
  bool newDataRecieved = false;
  String id;
  String callJson(String type, String data);
    void addData(String name ,int value);
  void addData(String name, String value);
  String data(String name);
  void run(String serialBuffer);
  int serialConnected();

   };
#endif