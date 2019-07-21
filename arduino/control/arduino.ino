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
#define PORT_SERVO 24

//define micro motion switch
#define SWITCH_STEPPER_BIG_ARM 30
#define SWITCH_STEPPER_SMALL_ARM 31
#define SWITCH_STEPPER_GIMBAL 32
#define SWITCH_STEPPER_HORIZONTAL 33
#define SWITCH_SERVO 34


void StepperMotor(int PULPin, boolean DIR, int steps, bool ENA, int velocity=10);

// 创建一个舵机对象
Servo myServo;


// structure
struct motor_controler {
  struct motor{
    int state = 0;
    int angle = 0;
    int target = 0;
    float offset = 0.0f;
    float step_length = 0.0f;
  };
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
if (digitalRead(SWITCH_SERVO)==HIGH) {
mc.servo.angle = 0;
}
if (digitalRead(SWITCH_STEPPER_BIG_ARM)==HIGH) {
mc.stepper_big_arm.angle = 0;
}
if (digitalRead(SWITCH_STEPPER_GIMBAL)==HIGH) {
mc.stepper_gimbal.angle = 0;
}
if (digitalRead(SWITCH_STEPPER_HORIZONTAL)==HIGH) {
mc.stepper_horizontal.angle = 0;
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
  StepperMotor(PORT_STEPPER_BIG_ARM, false, 2, true, 100);
  StepperMotor(PORT_STEPPER_SMALL_ARM, true, 1, true, 100);
  StepperMotor(PORT_STEPPER_GIMBAL, true, 1, true, 40);
  while (!(mc.stepper_big_arm.state&&mc.stepper_gimbal.state&&mc.stepper_small_arm.state)) {
    /*
    if (digitalRead(SWITCH_SERVO)==HIGH) {
      mc.servo.angle = 0;
      mc.servo.state = 1;
      StepperMotor(PORT_SERVO, true, 2, true);
    } else {
      StepperMotor(PORT_SERVO, true, 2, false);
    }*/
    if (digitalRead(SWITCH_STEPPER_BIG_ARM)==HIGH) {
      mc.stepper_big_arm.angle = (int)(mc.stepper_big_arm.offset/mc.stepper_big_arm.step_length);
      mc.stepper_big_arm.state = 1;
      StepperMotor(PORT_STEPPER_BIG_ARM, false, 2, true, 100);
      delay(1);
    } else {
      Serial.println("big_arm\n");
      StepperMotor(PORT_STEPPER_BIG_ARM, false, 2, false, 100);
      delay(1);
    }
    if (digitalRead(SWITCH_STEPPER_GIMBAL)==HIGH) {
      mc.stepper_gimbal.angle = (int)(mc.stepper_gimbal.offset/mc.stepper_gimbal.step_length);
      mc.stepper_gimbal.state = 1;
      StepperMotor(PORT_STEPPER_GIMBAL, true, 1, true, 40);
      delay(5);
    } else {
      StepperMotor(PORT_STEPPER_GIMBAL, true, 1, false, 40);
      delay(5);
    }
    /*
    if (digitalRead(SWITCH_STEPPER_HORIZONTAL)==HIGH) {
      mc.stepper_horizontal.angle = 0;
      mc.stepper_horizontal.state = 1;
      StepperMotor(PORT_STEPPER_HORIZONTAL, true, 2, true);
    } else {
      StepperMotor(PORT_STEPPER_HORIZONTAL, true, 2, false);
    }*/
    
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

void ServoMotor(int target, int current, int velocity=10)
{
  if (target >= current){
    for(int pos = current; pos<=target; pos++) {    // 每次1°                   
       myServo.write(pos);        // 写角度到舵机     
       delay(15);                // 延时15ms让舵机转到指定位置
    } 
  } else {
    for(int pos = current; pos>=target; pos--) {    // 每次1°                   
       myServo.write(pos);        // 写角度到舵机     
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
    }
  } else {
    for (int i = 0; i<180; i++){
      myServo.write(i);
      delay(15);
    }
  }
}

void feedMotorCurrent(motor_controler& mc, int data_final[4]){
  mc.stepper_big_arm.target = (int)((data_final[0]>=mc.stepper_big_arm.offset?mc.stepper_big_arm.offset:data_final[0])/mc.stepper_big_arm.step_length);
  mc.stepper_small_arm.target = (int)((data_final[1]>=mc.stepper_small_arm.offset?mc.stepper_small_arm.offset:data_final[1])/mc.stepper_small_arm.step_length);
  if (data_final[2]<=180 && data_final[2]>=-180) {
    mc.stepper_gimbal.target = (int)((data_final[2])/mc.stepper_gimbal.step_length);
  }
  else {
    mc.stepper_gimbal.target = (int)((data_final[2]>180?180:-180)/mc.stepper_gimbal.step_length);
  }
  if (data_final[3]<=180 && data_final[3]>=-180) {
    mc.stepper_horizontal.target = (int)((data_final[3])/mc.stepper_horizontal.step_length);
  }
  else {
    mc.stepper_horizontal.target = (int)((data_final[3]>180?180:-180)/mc.stepper_horizontal.step_length);
  }
  if (data_final[4]<=180 && data_final[4]>=-180) {
    mc.servo.target = (int)((data_final[4])/mc.servo.step_length);
  }
  else {
    mc.servo.target = (int)((data_final[4]>180?180:-180)/mc.servo.step_length);
  }
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
    feedMotorCurrent(mc, data_final);
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
  pinMode(SWITCH_STEPPER_HORIZONTAL, INPUT_PULLUP);
  pinMode(SWITCH_SERVO, INPUT_PULLUP);

  MC.stepper_big_arm.offset = 90.0;
  MC.stepper_small_arm.offset = 120.0;
  MC.stepper_gimbal.offset = 0.0;
  MC.stepper_horizontal.offset = 0.0;

  MC.stepper_big_arm.step_length = 1.8/10/8;
  MC.stepper_small_arm.step_length = 1.8/10/8;
  MC.stepper_gimbal.step_length = 1.8/8;
  MC.stepper_horizontal.step_length = 1.8/8;

  // 舵机
  myServo.attach(53);  // 将引脚9上的舵机与声明的舵机对象连接起来
  check_zero_state(MC); 
};



void loop()
{
  // 我们假定这个函数可以将蓝牙模块传来的信息弄进我们预先设定的struct里面
  Serial.println("BlueTooth start!\n");
  //BlueToothReceiver(MC);
  // 这个函数将所有motor调回初始状态
  //BackToOriginPosition();
  // MC：实例化的motorcontroler，关于其初始化，参见void setup
  // 同样地，我不太确定 CJB dl 的接口如何，此处暂时留空。（另外，全局变量不好吗……我觉得弄个structure是画蛇添足）
  
  // 为了方便debug我们每次只动一个motor
  // FUTURE TO DO: 让他们一块转！（虽然效率没有改变但显得更一体化了）
  
  // 注：具体输入视receiver的数据而定……我不清楚蓝牙会传回什么样的数据，因此本句无法具体决定
  int safe = 0;    //not in safe mode
  Serial.println("Thread\n");
  /*
   * test programe without bluetooth
   */
   
  //initial angle
  int data_final[4];
  data_final[0] = 60;
  data_final[1] = 90;
  data_final[2] = 45;
  data_final[3] = 0;
  feedMotorCurrent(MC,data_final);
  //MC.stepper_gimbal.target = 1000;
  
  /*
  MC.stepper_big_arm.angle = 0;
  MC.stepper_small_arm.angle = 0;
  MC.stepper_gimbal.angle = 0;
  MC.stepper_horizontal.angle = 0;

  //initial_state
  MC.stepper_big_arm.state = 1;
  MC.stepper_small_arm.state = 1;
  MC.stepper_gimbal.state = 1;
  MC.stepper_horizontal.state = 1;

  //initial target
  MC.stepper_big_arm.target = 200;
  MC.stepper_small_arm.target = 200;
  MC.stepper_gimbal.target = 60;
  MC.stepper_horizontal.target = 60;
  */
  
  /**
   * state = 1: enable
   * state = 2: lock
   */
  Serial.print("BIG_ARM: ");
  Serial.print("target: ");
  Serial.print(MC.stepper_big_arm.target);
  Serial.print("  angle: ");
  while (MC.stepper_big_arm.state==1) {
      Serial.print(MC.stepper_big_arm.angle);
      Serial.print("  ");
      if (MC.stepper_big_arm.angle>MC.stepper_big_arm.target) {
          StepperMotor(PORT_STEPPER_BIG_ARM, 1, 1, false, 40);
          MC.stepper_big_arm.angle--;
          delay(5);
          
      }
      else if (MC.stepper_big_arm.angle<MC.stepper_big_arm.target) {
          StepperMotor(PORT_STEPPER_BIG_ARM, 0, 1, false, 40);
          MC.stepper_big_arm.angle++;
          delay(5);
      }
      else MC.stepper_big_arm.state+=1;
  }
  Serial.print("\n");
  StepperMotor(PORT_STEPPER_BIG_ARM, 0, 0, true);

  Serial.print("SMALL_ARM: ");
  Serial.print("target: ");
  Serial.print(MC.stepper_small_arm.target);
  Serial.print("  angle: ");
  while (MC.stepper_small_arm.state==1) {
      Serial.print(MC.stepper_small_arm.angle);
      Serial.print("  ");
      if (MC.stepper_small_arm.angle>MC.stepper_small_arm.target) {
          StepperMotor(PORT_STEPPER_SMALL_ARM, 0, 1, false, 40);
          MC.stepper_small_arm.angle--;
          delay(5);
      }
      else if (MC.stepper_small_arm.angle<MC.stepper_small_arm.target) {
          StepperMotor(PORT_STEPPER_SMALL_ARM, 1, 1, false, 40);
          MC.stepper_small_arm.angle++;
          delay(5);
      }
      else MC.stepper_small_arm.state+=1;
  }
  Serial.print("\n");
  StepperMotor(PORT_STEPPER_SMALL_ARM, 0, 0, true);

  Serial.print("GIMBAL: ");
  Serial.print("target: ");
  Serial.print(MC.stepper_gimbal.target);
  Serial.print("  angle: ");
  while (MC.stepper_gimbal.state==1) {
      if (MC.stepper_gimbal.angle>MC.stepper_gimbal.target) {
        Serial.print(MC.stepper_gimbal.angle);
      Serial.print("  ");
          StepperMotor(PORT_STEPPER_GIMBAL, 0, 1, false, 40);
          MC.stepper_gimbal.angle--;
          delay(5);
      }
      else if (MC.stepper_gimbal.angle<MC.stepper_gimbal.target) {
        Serial.print(MC.stepper_gimbal.angle);
      Serial.print("  ");
          StepperMotor(PORT_STEPPER_GIMBAL, 1, 1, false, 40);
          MC.stepper_gimbal.angle++;
          delay(5);
      }
      else MC.stepper_gimbal.state+=1;
  } 
  Serial.print("\n");
  StepperMotor(PORT_STEPPER_GIMBAL, 0, 0, true);

  Serial.print("HORIZONTAL: ");
  Serial.print("target: ");
  Serial.print(MC.stepper_horizontal.target);
  Serial.print("  angle: ");
  while (MC.stepper_horizontal.state==1) {
      Serial.print(MC.stepper_horizontal.angle);
      Serial.print("  ");
      if (MC.stepper_horizontal.angle>MC.stepper_horizontal.target) {
          StepperMotor(PORT_STEPPER_HORIZONTAL, 1, 1, false, 40);
          MC.stepper_horizontal.angle--;
          delay(5);
      }
      else if (MC.stepper_horizontal.angle<MC.stepper_horizontal.target) {
          StepperMotor(PORT_STEPPER_HORIZONTAL, 0, 1, false, 40);
          MC.stepper_horizontal.angle++;
          delay(5);
      }
      else MC.stepper_horizontal.state+=1;
  }
  Serial.print("\n");
  StepperMotor(PORT_STEPPER_HORIZONTAL, 0, 0, true);

  //ServoMotor();
  if (MC.stepper_horizontal.state==2 && MC.stepper_gimbal.state==2 && MC.stepper_big_arm.state==2 && MC.stepper_small_arm.state==2) {
      //ServoPickup(MC.servo.state); //pickup or release
  }
  
}
