
#include "SerialData.h"

String Buffer;
SerialData weather("0", Serial);
SerialData lights("1", Serial);
SerialData Time("2", Serial);
SerialData config("3", Serial);
SerialData sensor("4", Serial);

bool serialCheck;

void setup() {
  Serial.begin(9600);
  weather.addData("weather", "Cloudy");
  weather.addData("Temperature", 16);
  weather.addData("Humidity", 70);
 lights.addData("indoor", false);
  lights.addData("outdoor", true);
  
}

void loop() {
  String newBuffer;
  while (Serial.available() > 0) {
    serialCheck = true;
    char dta = Serial.read();
    newBuffer = newBuffer + dta;
  }
  if (serialCheck == true) {
    Buffer = newBuffer;
    serialCheck = false;
    //Serial.println(Buffer);
  }
  weather.run(Buffer);
  lights.run(Buffer);
  Time.run(Buffer);
  config.run(Buffer);  
Serial.print("Connection status:");
Serial.println(weather.serialConnected());
  delay(3000);
}


//{"id":"0","type":"updated","data":""}
//{"id":"0","type":"rtu","data":"{\"weather\":\"snow\",\"Temperature\":6,\"Humidity\":100}"}
