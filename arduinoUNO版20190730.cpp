//本次实验电机驱动采用共阴极接线方式
//将步进电机的参数写到一个子函数中，并加入限位开关，触动限位开关可实现步进电机停转

//20190501 in Zijingang Campus

// 舵机控制
#include <Servo.h>    // 声明调用Servo.h库
#include "blueTooth.hpp"

//define control signal port\
//use PORT_STEPPER_XXXX+1 and PORT_STEPPER_XXXX+2 to get the other two port
#define PORT_STEPPER_BIG_ARM 2
#define PORT_STEPPER_SMALL_ARM 5
#define PORT_STEPPER_GIMBAL 8
#define PORT_STEPPER_HORIZONTAL 11
#define PORT_SERVO 12   

//define micro motion switch
#define SWITCH_STEPPER_BIG_ARM A0
#define SWITCH_STEPPER_SMALL_ARM A1
#define SWITCH_STEPPER_GIMBAL A2


void StepperMotor(int PULPin, boolean DIR, int steps, bool ENA, int velocity=10);

// 创建一个舵机对象
Servo myServo;
Servo horizontal;
float data_final[5];

typedef struct motor_t{
    int state = 0;
    int angle = 0;
    int target = 0;
    float offset = 0.0f;
    float step_length = 0.0f;
    int port;
  } motor;

// structure
struct motor_controler {
  int safe = 0;
  motor stepper_big_arm;
  motor stepper_small_arm;
  motor stepper_gimbal;
  motor stepper_horizontal;
  motor servo;
} MC;


/**
* @fun_c check whether each motor is at the initial position
* @params mc: the motor controler
*/
void check_init(motor_controler& mc) {
if (digitalRead(SWITCH_STEPPER_BIG_ARM)==HIGH) {
mc.stepper_big_arm.angle = 0;
}
if (digitalRead(SWITCH_STEPPER_GIMBAL)==HIGH) {
mc.stepper_gimbal.angle = 0;
}
if (digitalRead(SWITCH_STEPPER_SMALL_ARM)==HIGH) {
mc.stepper_small_arm.angle = 0;
}
}

/**
* @fun_c check whether each motor reach the initial position
* if not, make each motor set at the initial position
* if it is, lock that motor, change the state to 1
* @params mc: the motor controler
*/
void check_zero_state (motor_controler& mc) {
  Serial.print("check zero state\n");
  mc.stepper_horizontal.angle = 0;
  mc.stepper_horizontal.state = 1;
  int gimbal_count = 0;
  bool gimbal_init_dir = true;
  int gimbal_init_max_ang = 50;
  //rotate each motor one by one until all motors back to initial position
  //while one motor is rotating, other motors are not locked
  //when the digitalRead of the port connected to switch is high, that means the motor has reached its initial position, stop rotating. while not, keep roating
  //TODO: currently at a low speed to ensure safty, need to choose a suitable speed by change the time delay 
  while (!(mc.stepper_big_arm.state&&mc.stepper_gimbal.state&&mc.stepper_small_arm.state)) {
    if (digitalRead(SWITCH_STEPPER_BIG_ARM)==HIGH) {
      mc.stepper_big_arm.angle = (int)(mc.stepper_big_arm.offset/mc.stepper_big_arm.step_length);
      mc.stepper_big_arm.state = 1;
      StepperMotor(PORT_STEPPER_BIG_ARM, false, 2, true, 100);
      delay(1);
    } else {
      Serial.println("big_arm\n");
      StepperMotor(PORT_STEPPER_BIG_ARM, false, 2, false, 100);
      delay(1);
      continue;
    }
    if (digitalRead(SWITCH_STEPPER_GIMBAL)==HIGH) {
      mc.stepper_gimbal.angle = (int)(mc.stepper_gimbal.offset/mc.stepper_gimbal.step_length);
      mc.stepper_gimbal.state = 1;
      StepperMotor(PORT_STEPPER_GIMBAL, true, 1, true, 40);
      gimbal_count = 0;
      delay(5);
    } else {
      StepperMotor(PORT_STEPPER_GIMBAL, gimbal_init_dir, 1, false, 40);
      gimbal_count++;
      if(gimbal_count>gimbal_init_max_ang) {
        gimbal_init_max_ang = 3*gimbal_init_max_ang;
        gimbal_count = 0;
        gimbal_init_dir = !gimbal_init_dir;
      }
      delay(20);
      continue;
    }
    if (digitalRead(SWITCH_STEPPER_SMALL_ARM)==HIGH) {
      mc.stepper_small_arm.angle = (int)(mc.stepper_small_arm.offset/mc.stepper_small_arm.step_length);
      mc.stepper_small_arm.state = 1;
      Serial.print("small_arm freeze\n");
      StepperMotor(PORT_STEPPER_SMALL_ARM, true, 1, true, 100);
      delay(1);
    } else {
      StepperMotor(PORT_STEPPER_SMALL_ARM, true, 1, false, 100);
      Serial.print("small_arm\n");
      delay(5);
      continue;
    }
  }
}
 
// 步进电机子函数
// 函数：StepperMotor    功能：控制步进电机是否脱机、方向、步数
// 参数：DIRPin---方向控制的pin
//      ENA---脱机状态，true为脱机 (updated on 20190430)
//      DIR---方向控制
//      steps---步进的步数，若steps为0，则电机上电电磁锁死，不转
//      velocity----TBD
// 无返回值

void StepperMotor(int PULPin, boolean DIR, int steps, bool ENA, int velocity)
{
  int DIRPin = PULPin + 1;
  int ENAPin = PULPin + 2;
  digitalWrite(ENAPin, ENA);
  digitalWrite(DIRPin, DIR);
  int delay_time = 2000/velocity;
  for (int i = 0; i < steps; i++) //Forward XXXX steps
  {
    digitalWrite(PULPin, HIGH);
    delayMicroseconds(delay_time);
    digitalWrite(PULPin, LOW);
    delayMicroseconds(delay_time);
  }
}

// 舵机设定角度子函数
// 函数：ServoMotor     功能：设置舵机的角度
// 参数：target-----欲要让舵机转到的角度
//       current-----当前舵机的位置
// 无返回值
// WARNING!!!!　
// current变量一定不能随便乱来，否则可能舵机转的太猛造成机械结构的损坏

void ServoMotor(Servo myServo_, int target, int current, int velocity=10)
{
  if (target >= current){
    for(int pos = current; pos<=target; pos++) {    // 每次1°                   
       myServo_.write(pos);        // 写角度到舵机     
       delay(15);                // 延时15ms让舵机转到指定位置
    } 
  } else {
    for(int pos = current; pos>=target; pos--) {    // 每次1°                   
       myServo_.write(pos);        // 写角度到舵机     
       delay(15);                // 延时15ms让舵机转到指定位置
    }
  }
}


// 金属夹子函数
// 函数：PickUp    功能：抓起物品
// parameter: pick --- 为0时抓起，不为0则放下
void ServoPickup(int pick){
  if (pick == 0){
    for (int i = 180; i > 0; i--){
      myServo.write(i);
      delay(15);
      Serial.println("现在正在尝试抓起");
    }
  } else {
    for (int i = 0; i<180; i++){
      myServo.write(i);
      delay(15);
      Serial.println("现在正在尝试放下");
    }
  }
}

/*
 * @func: process bluetooth data, convert target angles to actual step to move for each motor 
 * @params: data_final: the array from bluetooth
 */
void feedMotorCurrent(motor_controler& mc, float data_final[5]){
  mc.stepper_big_arm.target = (int)((data_final[0]>=mc.stepper_big_arm.offset?mc.stepper_big_arm.offset:data_final[0])/mc.stepper_big_arm.step_length);
  mc.stepper_small_arm.target = (int)((data_final[1]>=mc.stepper_small_arm.offset?mc.stepper_small_arm.offset:data_final[1])/mc.stepper_small_arm.step_length);
  if (data_final[2]<=180 && data_final[2]>=-180) {
    mc.stepper_gimbal.target = (int)((data_final[2])/mc.stepper_gimbal.step_length);
  }
  else {
    mc.stepper_gimbal.target = (int)((data_final[2]>180?180:-180)/mc.stepper_gimbal.step_length);
  }
  if (data_final[3]>=0 && data_final[3]<=90) {
    mc.stepper_horizontal.target = (int)((data_final[3])/mc.stepper_horizontal.step_length);
  }
  else {
    mc.stepper_horizontal.target = (int)((data_final[3]>90?90:0)/mc.stepper_horizontal.step_length);
  }
  if (data_final[4]==0 || data_final[4]==1) {
    mc.servo.target = data_final[4];
  }
  else {
    mc.servo.target = 1;
  }
  //change state to 1, means ready to move
  mc.stepper_big_arm.state = 1;
  mc.stepper_small_arm.state = 1;
  mc.stepper_gimbal.state = 1;
  mc.stepper_horizontal.state = 1;
  mc.servo.state = 1;
}


// 蓝牙模块接收函数
// 函数：BlueToothReceiver
// 参数：TBD
// 返回值：TBD
void BlueToothReceiver(motor_controler& mc)
{
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
    //feedMotorCurrent(mc, data_final);
    Serial.print(mc.stepper_big_arm.angle);
    Serial.print(" ");
    Serial.print(mc.stepper_small_arm.angle);
    Serial.print(" ");
    Serial.print(mc.stepper_gimbal.angle);
    Serial.print(" ");
    Serial.print(mc.stepper_horizontal.angle);
    Serial.print(" ");
    Serial.println(mc.servo.angle);
  }
}


// 返回所有电机的初始位置
// 函数：BackToOriginPosition()
// 参数：TBD
// 返回值：TBD
// 这个函数将所有电机调回他们的初始位置
void BackToOriginPosition()
{
    // TODO by CJB
}

//serial_state_controler
struct serial_state_handler_t {
  int state = 0;  //current state of arduino. 0:the robotic arm is still picking objects; 1: the robotic arm is ready to conduct next command
  int recieved = 0; //0:signal not recieved sucessfully; 1:signal recieved sucessfully
  int count = -1;
}serial_state_handler;


void serial_fetch(int* data_final) {
  serial_state_handler.recieved = 0;
  int sign = 1;
  Serial.write(0xa6); //send 0xa6 to inform PC that Arduino is ready to recieve an order
  Serial.println("trying!!!");
  while (Serial.available()>0)
  {
    byte data= Serial.read();
    if (data == 165) {  //mark the start of PC message
      Serial.println("succeed!!!");
      serial_state_handler.recieved = 1;
      serial_state_handler.count = 0;
    }
    if (serial_state_handler.count==0) {
      serial_state_handler.count+=1;
      continue;
    }
    //read the first five value and store it in data_final
    if (serial_state_handler.count>=1 && serial_state_handler.count<9) {
      if (serial_state_handler.count==1 || serial_state_handler.count==3 || serial_state_handler.count==5) {
        Serial.print("sign: ");
        Serial.println(data);
        sign = (data==1?-1:1);
        serial_state_handler.count++;
        continue;
      }
      if (serial_state_handler.count==2 || serial_state_handler.count==4 || serial_state_handler.count==6) {
        Serial.print("abs: ");
        Serial.println(data);
        data_final[(int)serial_state_handler.count/2-1] = sign*data;
        serial_state_handler.count++;
        continue;
      }
      if (serial_state_handler.count==7 || serial_state_handler.count==8) {
        Serial.print("abs: ");
        Serial.println(data);
        data_final[serial_state_handler.count-4]=(int)data;
        serial_state_handler.count++;
        if (serial_state_handler.count == 9) {
          Serial.print("finish reading\n");
          serial_state_handler.count = -1;
          for (int i=0; i<5; i++) {
            Serial.println(data_final[i]);
          }
          return;
        }
        continue;
      }
    }
    delay(2);
  }
}

void setup()
{
  // 步进电机
  // 1是dir 2是pul 3是ena
  // big_arm
  Serial.begin(9600);
  pinMode (PORT_STEPPER_BIG_ARM, OUTPUT);
  pinMode (PORT_STEPPER_BIG_ARM+1, OUTPUT);
  pinMode (PORT_STEPPER_BIG_ARM+2, OUTPUT);
  pinMode (PORT_STEPPER_SMALL_ARM, OUTPUT);
  pinMode (PORT_STEPPER_SMALL_ARM+1, OUTPUT);
  pinMode (PORT_STEPPER_SMALL_ARM+2, OUTPUT);
  pinMode (PORT_STEPPER_GIMBAL, OUTPUT);
  pinMode (PORT_STEPPER_GIMBAL+1, OUTPUT);
  pinMode (PORT_STEPPER_GIMBAL+2, OUTPUT);
  pinMode (PORT_STEPPER_HORIZONTAL, OUTPUT);
  pinMode (PORT_STEPPER_HORIZONTAL+1, OUTPUT);
  pinMode (PORT_STEPPER_HORIZONTAL+2, OUTPUT);
  pinMode (PORT_SERVO, OUTPUT);
  pinMode (PORT_SERVO+1, OUTPUT);
  pinMode (PORT_SERVO+2, OUTPUT);


  pinMode(SWITCH_STEPPER_BIG_ARM, INPUT_PULLUP);
  pinMode(SWITCH_STEPPER_GIMBAL, INPUT_PULLUP);
  pinMode(SWITCH_STEPPER_SMALL_ARM, INPUT_PULLUP);

  MC.stepper_big_arm.offset = 90.0;
  MC.stepper_small_arm.offset = 150.0;
  MC.stepper_gimbal.offset = -22.0;
  MC.stepper_horizontal.offset = 0.0;

  MC.stepper_big_arm.step_length = 1.8/10/8;
  MC.stepper_small_arm.step_length = 1.8/10/8;
  MC.stepper_gimbal.step_length = 1.8/8/13.7;
  MC.stepper_horizontal.step_length = 1;

  MC.stepper_big_arm.port = PORT_STEPPER_BIG_ARM;
  MC.stepper_small_arm.port = PORT_STEPPER_SMALL_ARM;
  MC.stepper_gimbal.port = PORT_STEPPER_GIMBAL;
  MC.stepper_horizontal.port = PORT_STEPPER_HORIZONTAL;
  MC.servo.port = PORT_SERVO;

  // 舵机
  myServo.attach(PORT_SERVO);  // 将引脚9上的舵机与声明的舵机对象连接起来
  horizontal.attach(PORT_STEPPER_HORIZONTAL); //新版本把水平座电机换成了舵机

  //归零位
};



void loop()
{
  // 我们假定这个函数可以将蓝牙模块传来的信息弄进我们预先设定的struct里面
  Serial.println("BlueTooth start!\n");
  //BlueToothReceiver(MC);
  // 这个函数将所有motor调回初始状态
  //BackToOriginPosition();
  // MC：实例化的motorcontroler，关于其初始化，参见void setup
  // 同样地，我不太确定 CJB dl 的接口如何，此处暂时留空。（另外，全局变量不好吗⋯⋯我觉得弄个structure是画蛇添足）
  
  // 为了方便debug我们每次只动一个motor
  // FUTURE TO DO: 让他们一块转！（虽然效率没有改变但显得更一体化了）
  
  // 注：具体输入视receiver的数据而定⋯⋯我不清楚蓝牙会传回什么样的数据，因此本句无法具体决定
  
  /*
   * test programe without bluetooth
   */
   //TODO: replace this with a function that read data from bluetooth 
  
  /*
  data_final[0] = 57.123; //big_arm
  data_final[1] = 92.712; //small_arm
  data_final[2] = -4.137; //gimbal*/
  /*
  data_final[0] = 49.860;
  data_final[1] = 107.334;
  data_final[2] = -3.217;
  data_final[3] = 0;
  data_final[4] = 0;
  */
  check_zero_state(MC); 
  
  serial_state_handler.recieved=0;
  serial_state_handler.state=1;
  
  int data_raw[5];
  while (serial_state_handler.recieved==0 && serial_state_handler.state==1) {
    serial_fetch(data_raw);
  }
  
  data_final[0] = data_raw[0];
  data_final[1] = data_raw[1];
  data_final[2] = data_raw[2];
  data_final[3] = (float)data_raw[3];
  data_final[4] = (float)data_raw[4];

  Serial.print("finish read serial: ");
  for(int i=0; i<5; i++) {
    Serial.print(data_final[i]);
    Serial.print(" ");
  }
  Serial.print("\n");

  serial_state_handler.state = 0;
  serial_state_handler.recieved = 0;

  //convert to actual angle into number of steps
  feedMotorCurrent(MC,data_final);
  
  /**
   * MOVE THE ROBOTIC ARM
   * state = 1: enable
   * state = 2: lock
   * important message are print on Serial
   * after this part, state for each motor change from 1 to 2, indicate motion complete, motor locked and ready for another motion
   * Note: in this version, I move all motors one by one
   */
  //rotate big arm to asigned angle 
  Serial.print("BIG_ARM: ");
  Serial.print("target: ");
  Serial.print(MC.stepper_big_arm.target);
  //Serial.print("  angle: ");
  while (MC.stepper_big_arm.state==1) {
      //Serial.print(MC.stepper_big_arm.angle);
      //Serial.print("  ");
      if (MC.stepper_big_arm.angle>MC.stepper_big_arm.target) {
          StepperMotor(MC.stepper_big_arm.port, 1, 1, false, 40);
          MC.stepper_big_arm.angle--;
          delay(5);
      }
      else if (MC.stepper_big_arm.angle<MC.stepper_big_arm.target) {
          StepperMotor(MC.stepper_big_arm.port, 0, 1, false, 40);
          MC.stepper_big_arm.angle++;
          delay(5);
      }
      else MC.stepper_big_arm.state+=1;
  }
  Serial.print("\n");
  if(MC.stepper_big_arm.state==2) StepperMotor(MC.stepper_big_arm.port, 0, 0, true); //unable the motor

  //rotate small arm to asigned angle 
  Serial.print("SMALL_ARM: ");
  Serial.print("target: ");
  Serial.print(MC.stepper_small_arm.target);
  //Serial.print("  angle: ");
  while (MC.stepper_small_arm.state==1) {
      //Serial.print(MC.stepper_small_arm.angle);
      //Serial.print("  ");
      if (MC.stepper_small_arm.angle>MC.stepper_small_arm.target) {
          StepperMotor(MC.stepper_small_arm.port, 0, 1, false, 40);
          MC.stepper_small_arm.angle--;
          delay(5);
      }
      else if (MC.stepper_small_arm.angle<MC.stepper_small_arm.target) {
          StepperMotor(MC.stepper_small_arm.port, 1, 1, false, 40);
          MC.stepper_small_arm.angle++;
          delay(5);
      }
      else MC.stepper_small_arm.state+=1;
  }
  Serial.print("\n");
  if(MC.stepper_small_arm.state==2) StepperMotor(MC.stepper_small_arm.port, 0, 0, true); //unable the motor

  //rotate gimbal motor to asigned angle 
  Serial.print("GIMBAL: ");
  Serial.print("target: ");
  Serial.print(MC.stepper_gimbal.target);
  //Serial.print("  angle: ");
  while (MC.stepper_gimbal.state==1) {
      //Serial.print(MC.stepper_gimbal.angle);
      //Serial.print("  ");
      if (MC.stepper_gimbal.angle>MC.stepper_gimbal.target) {
          StepperMotor(MC.stepper_gimbal.port, 1, 1, false, 40);
          MC.stepper_gimbal.angle--;
          delay(20);
      }
      else if (MC.stepper_gimbal.angle<MC.stepper_gimbal.target) {
          StepperMotor(MC.stepper_gimbal.port, 0, 1, false, 40);
          MC.stepper_gimbal.angle++;
          delay(20);
      }
      else MC.stepper_gimbal.state+=1;
  }
  Serial.print("\n");
  if(MC.stepper_gimbal.state==2) StepperMotor(MC.stepper_gimbal.port, 0, 0, true); //unable the motor

  delay(3000);
  //rotate servo on the horizontal base to asigned angle 
  Serial.print("HORIZONTAL: ");
  Serial.print("target: ");
  Serial.print(MC.stepper_horizontal.target);
  if (MC.stepper_horizontal.state==1) {
      ServoMotor(horizontal, MC.stepper_horizontal.target, MC.stepper_horizontal.angle);
      MC.stepper_horizontal.angle = MC.stepper_horizontal.target;
      MC.stepper_horizontal.state+=1;
  }
  Serial.print("start picking up\n");
  delay(3000);
  //pick up or release the object
  ServoPickup(0);
  Serial.print("start dropping up\n");
  delay(3000);
  ServoPickup(1);
  delay(3000);
  
  serial_state_handler.state = 1;

  /**
   * SEND A MESSAGE TO COMPUTER TO REQUEST A NEW COMMAND
   * DONE BY JINBO CI
   */
   //TODO: ADD CODE SENDING A MESSAGE TO COMPUTER TO REQUEST A NEW COMMAND
}
