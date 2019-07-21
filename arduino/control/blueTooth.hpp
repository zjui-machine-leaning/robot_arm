//#include <SoftwareSerial.h>

const int rxpin = 0;
const int txpin = 1;
const int StartByte = 121;
const int InterStart = 122;
const int InterEnd = 123;
const int EndByte = 124;
const int positive = 125;
const int negative = 126;
const int time_de= 1;
const int expansion = 2;
//Connect the Bluetooth module
//SoftwareSerial bluetooth(rxpin, txpin);



byte readoff(byte data, int wait_time) {
  byte buff;
  ////Serial.println("Readoff");
  ////Serial.println(bluetooth.available());
  while (true) {
    while (!Serial.available()){
      delay(wait_time);
    }
    buff = Serial.read();
    ////Serial.println(buff);
    if (buff == data){
      Serial.write(data);
      buff = Serial.read();
      delay(wait_time);
    } else {
      break;
    }
  }
  return buff;
}
void readtill(byte data, int wait_time) {
  byte buff;
  ////Serial.println("Readoff");
  ////Serial.println(bluetooth.available());
  while (true) {
    while (!Serial.available()){
      delay(wait_time);
    }
    buff = Serial.read();
    ////Serial.println(buff);
    if (buff != data){
      Serial.write(data);
      buff = Serial.read();
      delay(wait_time);
    } else {
      for (int i = 0; i < 5; i++) {
        Serial.write(data);
        delay(wait_time);
      }
      break;
    }
  }
}
