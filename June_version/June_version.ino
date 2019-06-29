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
void setup()
{
  //Set the pin to put power out
  //SoftwareSerial bluetooth(rxpin, txpin);
  //Initialize Serial for debugging purposes
  Serial.begin(9600);
  //Serial.println("Serial ready");
  //Initialize the bluetooth
  //bluetooth.begin(9600);
  ////Serial.println("Bluetooth ready");

}


byte readoff(byte data, int wait_time = 1) {
  char buff[40];
  ////Serial.println("Readoff");
  ////Serial.println(bluetooth.available());
  bool recieved = false;
  while (!recieved) {
    while (!Serial.available()){
      delay(wait_time);
    }
    buff = Serial.readBytes(buff,40);
    for (int t=0; i<40; i++) {
      char ch = buff[i];
      if (ch=="X" && i<=19) {
        
      }
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

void loop() {
  // put your main code here, to run repeatedly:

  // start receiving data
  
  byte buff = Serial.read();
  int num = 0;
  int sign = 1;
  ////Serial.println(bluetooth.available());
  if (buff == StartByte) {
    ////Serial.println((byte)buff); 
    //Serial.println(buff);
    Serial.write(buff);
    buff = readoff(StartByte, time_de);
    //buff = Serial.read();
    num = buff;
    //Serial.println(num);
    Serial.write(buff);
    buff = readoff(num, time_de);
    
    
    
    int data[num];
    for(int i = 0; i < num; i++) {
        buff = readoff(InterStart, time_de);
        //Serial.print("Sign: ");
        //Serial.println(buff);
        sign = buff;
        Serial.write(sign);
        
        buff = readoff(sign, time_de);
        if (sign == negative) {
          sign = -1;
        } else {
          sign = 1;
        }
        int dataabs = buff;
        data[i] = sign*dataabs;
        //Serial.println(data[i]); 
        Serial.write(data[i]);  
        buff = readoff(dataabs, time_de);
        //Serial.println("InterEnd");
        buff = readoff(InterEnd, time_de);
        //Serial.println("InterEnddone");
 
    }
    readtill(EndByte, time_de);
    // print out what we have received to Serial
    /*
    //Serial.println("data:");
    for(int i = 0; i < num; i++) {
      //Serial.print(data[i]);
      //Serial.print(" ");
    }
    */
    //Serial.println();
    int data_final[num/expansion];
    for (int i = 0; i < num; i++){
      int sign = 1;
      int current_res = 0;
      for (int j = 0; j < expansion; j++){
        int current_val = data[i*expansion+j];
        if (current_val < 0)
        {
            sign = -1;
            current_val = -current_val;
        }
        current_res = current_res*100;
        current_res += current_val;
      }
      data_final[i] = sign*current_res;
    }
    Serial.println(num);
    for (int i = 0; i < num/expansion; i++){
      Serial.print(data_final[i]);
      Serial.print(" ");
    }
    Serial.println();
}
  delay(500);

}
