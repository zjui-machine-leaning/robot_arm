//本次实验电机驱动采用共阴极接线方式
//将步进电机的参数写到一个子函数中，并加入限位开关，触动限位开关可实现步进电机停转

//20190501 in Zijingang Campus

// 舵机控制
#include <Servo.h>    // 声明调用Servo.h库
#include <SoftwareSerial.h>


//define control signal port\
//use PORT_STEPPER_XXXX+1 and PORT_STEPPER_XXXX+2 to get the other two port
#define PORT_STEPPER_BIG_ARM 8;
#define PORT_STEPPER_SMALL_ARM 12;
#define PORT_STEPPER_GIMBAL 16;
#define PORT_STEPPER_HORIZONTAL 20;
#define PORT_SERVO 24;

//define micro motion switch
#define SWITCH_STEPPER_BIG_ARM 30;
#define SWITCH_STEPPER_SMALL_ARM 31;
#define SWITCH_STEPPER_GIMBAL 32;
#define SWITCH_STEPPER_HORIZONTAL 33;
#define SWITCH_SERVO 34;


// 创建一个舵机对象
Servo servo;

// structure
struct motor_controler {
  struct motor{
    int state = 0;
    int angle = 0;
    int target = 0;
  };
  int safe = 0;
  motor stepper_big_arm;
  motor stepper_small_arm;
  motor stepper_gimbal;
  motor stepper_horizontal;
  motor servo;
};

/* function bluetoothsetup 
 *  setup bluetooth
 *  should be called at setup()
 *  
 *  inputs: const int rxpin, const int txpin //Define the pins used for receiving
 *  outputs: bluetooth with type SoftwareSerial
 *  
 *  
 *  
 *  
 */
SoftwareSerial bluetoothsetup(const int rxpin, const int txpin){
  
  //Connect the Bluetooth module
  SoftwareSerial bluetooth(rxpin, txpin);
  //Initialize Serial for debugging purposes
  Serial.begin(9600);
  Serial.println("Serial ready");
  //Initialize the bluetooth
  bluetooth.begin(9600);
  Serial.println("Bluetooth ready");
  return bluetooth;
}



void setup()
{
  // 步进电机
  // 1是dir 2是pul 3是ena
  // big_arm
  //Set the pin to put power out
  SoftwareSerial bluetooth = bluetoothsetup(0, 1);
  /* TODO */
  pinMode (2, OUTPUT);
  pinMode (3, OUTPUT);
  pinMode (4, OUTPUT);
  
  pinMode (6, OUTPUT);
  pinMode (7, OUTPUT);
  pinMode (8, OUTPUT);
  
  pinMode (10, OUTPUT);
  pinMode (11, OUTPUT);
  pinMode (12, OUTPUT);

  pinMode (14, OUTPUT);
  pinMode (15, OUTPUT);
  pinMode (16, OUTPUT);

  // 舵机
  servo.attach(53);  // 将引脚9上的舵机与声明的舵机对象连接起来
  
  struct motor_controler {
    struct motor{
      int state = 0;
      int angle = 0;
    };
  int safe = 0;
  motor stepper_big_arm;
  motor stepper_small_arm;
  motor stepper_gimbal;
  motor stepper_horizontal;
  motor servo;
};

/*
 * function receiveByte
 * return the data between the StartKey and the EndKey
 * 
 * inputs: SoftwareSerial bluetooth, byte StartKey, byte EndKey
 * outputs byte that we recieved or 0 for failure
 * 
 * 
 */

byte receiveByte (SoftwareSerial bluetooth, byte StartKey, byte EndKey){
  // if not available, return 0 for failure
  if (!bluetooth.available()) {
    return 0;
  }
   byte buff = bluetooth.read();
   // when we cannot get the StartKey, keep recieving
   while (StartKey != buff){
     buff = bluetooth.read();
     delay(1); // I do not know whether this step is necessary
     bluetooth.write(buff); // send response
   }
   byte data = buff;
   while (EndKey != buff){
      buff = bluetooth.read();
      data = buff;
      delay(1); // I do not know whether this step is necessary
      bluetooth.write(data); // send response
   }
   return data;
  }

/*
 * function receiveData
 * 
 * inputs: SoftwareSerial bluetooth, byte StartByte, byte InterStart, 
 *         byte InterEnd, byte EndByte, int WaitTime
 * outputs: an int array with array[0] is the total length of the array
 * 
 * attentions:
 * 1) the data would be sent from the computer in the following order:
 * StartByte - Number - InterStart - First Data - InterEnd - 
 * InterStart - Second Data - InterEnd - ... - InterEnd - EndByte
 * 2) the computer would not send the second data, until it recieve the response
 */


int * receiveData(SoftwareSerial bluetooth, byte StartByte, byte InterStart, byte InterEnd, byte EndByte){
  //Check for new data
  byte buff = receiveByte(bluetooth, StartByte, InterStart);
  if(buff == 0) {
    Serial.println("Num Receive Failed");
    int fai[1];
    fai[0] = 0;
    return fai;
  }
  int arr[buff+1];
  for (byte i = 0; i < buff ; i++){
    buff = receiveByte(bluetooth, InterStart, InterEnd);
    if(buff == 0) {
      Serial.println((int)i, "th Receive Failed");
      arr[0] = 0;
      break;
    }
    if (buff == EndByte) {
      Serial.println((int)i, "End Unexpectedly at");
      arr[0] = i+1;
      break;
    }
    arr[i+1] = (int)buff;
    buff = receiveByte(bluetooth, InterEnd, InterEnd);
  }
  return arr;
}

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
while (!(mc.servo.state&&mc.stepper_big_arm.state&&mc.stepper_gimbal.state&&mc.stepper_horizontal.state&&mc.stepper_small_arm.state)) {
if (digitalRead(SWITCH_SERVO)==HIGH) {
mc.servo.angle = 0;
mc.servo.state = 1;
StepperMotor(PORT_SERVO, true, 2, true);
} else {
StepperMotor(PORT_SERVO, true, 2, false);
}
if (digitalRead(SWITCH_STEPPER_BIG_ARM)==HIGH) {
mc.stepper_big_arm.angle = 0;
mc.stepper_big_arm.state = 1;
StepperMotor(PORT_STEPPER_BIG_ARM, true, 2, true);
} else {
StepperMotor(PORT_STEPPER_BIG_ARM, true, 2, false);
}
if (digitalRead(SWITCH_STEPPER_GIMBAL)==HIGH) {
mc.stepper_gi



}
 

void loop()
{
  // 我们假定这个函数可以将蓝牙模块传来的信息弄进我们预先设定的struct里面
  BlueToothReceiver();

  // 这个函数将所有motor调回初始状态
  BackToOriginPosition();
  
  // MC：实例化的motorcontroler，关于其初始化，参见void setup
  // 同样地，我不太确定 CJB dl 的接口如何，此处暂时留空。（另外，全局变量不好吗……我觉得弄个structure是画蛇添足）
  int gimbal_target = MC.gimbal.target;
  
  // 为了方便debug我们每次只动一个motor
  // FUTURE TO DO: 让他们一块转！（虽然效率没有改变但显得更一体化了）
  
  // 注：具体输入视receiver的数据而定……我不清楚蓝牙会传回什么样的数据，因此本句无法具体决定
  StepperMotor(PORT_STEPPER_GIMBAL, 0, 100, false);
  // TODO
  StepperMotor(PORT_STEPPER_BIG_ARM, 0, 100, false);
  StepperMotor(PORT_STEPPER_SMALL_ARM, 0, 100, false);
  StepperMotor(PORT_STEPPER_HORIZONTAL, 0, 100, false);
  
  // pickup()：指挥夹子将物体夹起来。参数为1或0，具体说明请参见下方函数定义。
  Pickup(0);
  delay(1000);
  Pickup(1);
  delay(1000);
  
  // 
  
}
 
// 步进电机子函数
// 函数：StepperMotor    功能：控制步进电机是否脱机、方向、步数
// 参数：DIRPin---方向控制的pin
//      ENA---脱机状态，true为脱机 (updated on 20190430)
//      DIR---方向控制
//      steps---步进的步数，若steps为0，则电机上电电磁锁死，不转
//      velocity----TBD
// 无返回值

void StepperMotor(int PULPin, boolean DIR, int steps, bool ENA, int velocity=10)
{
  int DIRPin = PULPin + 1;
  int ENAPin = PULPin + 2;
  digitalWrite(ENAPin, ENA);
  digitalWrite(DIRPin, DIR);
  for (int i = 0; i < steps; i++) //Forward XXXX steps
  {
    digitalWrite(PULPin, HIGH);
    delayMicroseconds(50);
    digitalWrite(PULPin, LOW);
    delayMicroseconds(50);
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
    for(pos = current; pos<=target; pos++) {    // 每次1°                   
       servo.write(pos);        // 写角度到舵机     
       delay(15);                // 延时15ms让舵机转到指定位置
    } 
  } else {
    for(pos = current; pos>=target; pos--) {    // 每次1°                   
       servo.write(pos);        // 写角度到舵机     
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
      myservo.write(i);
      delay(15);
    }
  } else {
    for (int i = 0; i<180; i++){
      myservo.write(i);
      delay(15);
    }
  }
}


// 蓝牙模块接收函数
// 函数：BlueToothReceiver
// 参数：TBD
// 返回值：TBD
void BlueToothReceiver()
{
    // TODO by CJB
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
