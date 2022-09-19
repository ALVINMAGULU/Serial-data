#ifndef SERIAL_DATA_H
#define SERIAL_DATA_H
#include <Arduino.h>



class SerialData {
private:
  
  String type;
  int arrayLength = 5;
  int newAddress = 0;
  String boolToString(bool Bool);
  void listener(String buffer);
  int isConnected = -1;
  void verifyConnection();
  bool connectionVerified = false;
protected:
  Stream& stream;


public:
  SerialData(String id, Stream& str)
    : stream(str) {
    this->id = id;
    
  };
  bool isChanged = false;
  bool update = false;
  bool newDataRecieved = false;
  String id;
  String array[5][3];
  void updateData(String name, String value);
  void updateData(String name, int value);
  void requestToUpdate();
  String arrayToJson();
  String callJson(String type, String data);
  void jsonToArray(String updateJson);
  void addData(String name ,int value);
  void addData(String name, String value);
  String data(String name);
  void run(String serialBuffer);
  int serialConnected();

   };
#endif