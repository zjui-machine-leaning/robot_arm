//±¾´ÎÊµÑéµç»úÇý¶¯²ÉÓÃ¹²Òõ¼«½ÓÏß·½Ê½
//½«²½½øµç»úµÄ²ÎÊýÐ´µ½Ò»¸ö×Óº¯ÊýÖÐ£¬²¢¼ÓÈëÏÞÎ»¿ª¹Ø£¬´¥¶¯ÏÞÎ»¿ª¹Ø¿ÉÊµÏÖ²½½øµç»úÍ£×ª

//20190501 in Zijingang Campus

// ¶æ»ú¿ØÖÆ
#include <Servo.h>    // ÉùÃ÷µ÷ÓÃServo.h¿â
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

// ´´½¨Ò»¸ö¶æ»ú¶ÔÏó
Servo myServo;


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
      mc.stepper_gimbal.angle = 0;
      mc.stepper_gimbal.state = 1;
      StepperMotor(PORT_STEPPER_GIMBAL, true, 2, true);
    } else {
      StepperMotor(PORT_STEPPER_GIMBAL, true, 2, false);
    }
    
    if (digitalRead(SWITCH_STEPPER_HORIZONTAL)==HIGH) {
      mc.stepper_horizontal.angle = 0;
      mc.stepper_horizontal.state = 1;
      StepperMotor(PORT_STEPPER_HORIZONTAL, true, 2, true);
    } else {
      StepperMotor(PORT_STEPPER_HORIZONTAL, true, 2, false);
    }
    
    if (digitalRead(SWITCH_STEPPER_SMALL_ARM)==HIGH) {
      mc.stepper_horizontal.angle = 0;
      mc.stepper_horizontal.state = 1;
      StepperMotor(PORT_STEPPER_HORIZONTAL, true, 2, true);
    } else {
      StepperMotor(PORT_STEPPER_HORIZONTAL, true, 2, false);
    }
  }
}
 
// ²½½øµç»ú×Óº¯Êý
// º¯Êý£ºStepperMotor    ¹¦ÄÜ£º¿ØÖÆ²½½øµç»úÊÇ·ñÍÑ»ú¡¢·½Ïò¡¢²½Êý
// ²ÎÊý£ºDIRPin---·½Ïò¿ØÖÆµÄpin
//      ENA---ÍÑ»ú×´Ì¬£¬trueÎªÍÑ»ú (updated on 20190430)
//      DIR---·½Ïò¿ØÖÆ
//      steps---²½½øµÄ²½Êý£¬ÈôstepsÎª0£¬Ôòµç»úÉÏµçµç´ÅËøËÀ£¬²»×ª
//      velocity----TBD
// ÎÞ·µ»ØÖµ

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

// ¶æ»úÉè¶¨½Ç¶È×Óº¯Êý
// º¯Êý£ºServoMotor     ¹¦ÄÜ£ºÉèÖÃ¶æ»úµÄ½Ç¶È
// ²ÎÊý£ºtarget-----ÓûÒªÈÃ¶æ»ú×ªµ½µÄ½Ç¶È
//       current-----µ±Ç°¶æ»úµÄÎ»ÖÃ
// ÎÞ·µ»ØÖµ
// WARNING!!!!¡¡
// current±äÁ¿Ò»¶¨²»ÄÜËæ±ãÂÒÀ´£¬·ñÔò¿ÉÄÜ¶æ»ú×ªµÄÌ«ÃÍÔì³É»úÐµ½á¹¹µÄËð»µ

void ServoMotor(int target, int current, int velocity=10)
{
  if (target >= current){
    for(int pos = current; pos<=target; pos++) {    // Ã¿´Î1¡ã                   
       myServo.write(pos);        // Ð´½Ç¶Èµ½¶æ»ú     
       delay(15);                // ÑÓÊ±15msÈÃ¶æ»ú×ªµ½Ö¸¶¨Î»ÖÃ
    } 
  } else {
    for(int pos = current; pos>=target; pos--) {    // Ã¿´Î1¡ã                   
       myServo.write(pos);        // Ð´½Ç¶Èµ½¶æ»ú     
       delay(15);                // ÑÓÊ±15msÈÃ¶æ»ú×ªµ½Ö¸¶¨Î»ÖÃ
    }
  }
}


// ½ðÊô¼Ð×Óº¯Êý
// º¯Êý£ºPickUp    ¹¦ÄÜ£º×¥ÆðÎïÆ·
// parameter: pick --- Îª0Ê±×¥Æð£¬²»Îª0Ôò·ÅÏÂ
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


// À¶ÑÀÄ£¿é½ÓÊÕº¯Êý
// º¯Êý£ºBlueToothReceiver
// ²ÎÊý£ºTBD
// ·µ»ØÖµ£ºTBD
void BlueToothReceiver(motor_controler mc)
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
    mc.stepper_big_arm.angle = data_final[0];
    mc.stepper_small_arm.angle = data_final[1];
    mc.stepper_gimbal.angle = data_final[2];
    mc.stepper_horizontal.angle = data_final[3];
    mc.servo.angle = data_final[4];
    mc.stepper_big_arm.state = 0;
    mc.stepper_small_arm.state = 0;
    mc.stepper_gimbal.state = 0;
    mc.stepper_horizontal.state = 0;
    mc.servo.state = 0;
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


// ·µ»ØËùÓÐµç»úµÄ³õÊ¼Î»ÖÃ
// º¯Êý£ºBackToOriginPosition()
// ²ÎÊý£ºTBD
// ·µ»ØÖµ£ºTBD
// Õâ¸öº¯Êý½«ËùÓÐµç»úµ÷»ØËûÃÇµÄ³õÊ¼Î»ÖÃ
void BackToOriginPosition()
{
    // TODO by CJB
}


void setup()
{
  // ²½½øµç»ú
  // 1ÊÇdir 2ÊÇpul 3ÊÇena
  // big_arm
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

  // ¶æ»ú
  myServo.attach(53);  // ½«Òý½Å9ÉÏµÄ¶æ»úÓëÉùÃ÷µÄ¶æ»ú¶ÔÏóÁ¬½ÓÆðÀ´
 
  //check_zero_state(MC);
  Serial.begin(9600);
};



void loop()
{
  // ÎÒÃÇ¼Ù¶¨Õâ¸öº¯Êý¿ÉÒÔ½«À¶ÑÀÄ£¿é´«À´µÄÐÅÏ¢Åª½øÎÒÃÇÔ¤ÏÈÉè¶¨µÄstructÀïÃæ
  BlueToothReceiver(MC);

  // Õâ¸öº¯Êý½«ËùÓÐmotorµ÷»Ø³õÊ¼×´Ì¬
  //BackToOriginPosition();
  
  // MC£ºÊµÀý»¯µÄmotorcontroler£¬¹ØÓÚÆä³õÊ¼»¯£¬²Î¼ûvoid setup
  // Í¬ÑùµØ£¬ÎÒ²»Ì«È·¶¨ CJB dl µÄ½Ó¿ÚÈçºÎ£¬´Ë´¦ÔÝÊ±Áô¿Õ¡££¨ÁíÍâ£¬È«¾Ö±äÁ¿²»ºÃÂð¡­¡­ÎÒ¾õµÃÅª¸östructureÊÇ»­ÉßÌí×ã£©
  int gimbal_target = 60;//MC.gimbal.target;
  
  // ÎªÁË·½±ãdebugÎÒÃÇÃ¿´ÎÖ»¶¯Ò»¸ömotor
  // FUTURE TO DO: ÈÃËûÃÇÒ»¿é×ª£¡£¨ËäÈ»Ð§ÂÊÃ»ÓÐ¸Ä±äµ«ÏÔµÃ¸üÒ»Ìå»¯ÁË£©
  
  // ×¢£º¾ßÌåÊäÈëÊÓreceiverµÄÊý¾Ý¶ø¶¨¡­¡­ÎÒ²»Çå³þÀ¶ÑÀ»á´«»ØÊ²Ã´ÑùµÄÊý¾Ý£¬Òò´Ë±¾¾äÎÞ·¨¾ßÌå¾ö¶¨
  /*
  StepperMotor(PORT_STEPPER_GIMBAL, 0, 100, false);
  // TODO
  StepperMotor(PORT_STEPPER_BIG_ARM, 0, 100, false);
  StepperMotor(PORT_STEPPER_SMALL_ARM, 0, 100, false);
  StepperMotor(PORT_STEPPER_HORIZONTAL, 0, 100, false);
  
  // pickup()£ºÖ¸»Ó¼Ð×Ó½«ÎïÌå¼ÐÆðÀ´¡£²ÎÊýÎª1»ò0£¬¾ßÌåËµÃ÷Çë²Î¼ûÏÂ·½º¯Êý¶¨Òå¡£
  ServoPickup(0);
  delay(1000);
  ServoPickup(1);
  delay(1000);
  
  // 
  */
  delay(500);
}
