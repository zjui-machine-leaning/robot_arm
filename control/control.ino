/**Note: the last version*/

//����ʵ�����������ù��������߷�ʽ
//����������Ĳ���д��һ���Ӻ����У���������λ���أ�������λ���ؿ�ʵ�ֲ������ͣת

//20190501 in Zijingang Campus

// �������
#include <Servo.h>    // ��������Servo.h��
#include <Arduino.h>


//define control signal port
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


//blueTooth params
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


// class
class motor_controler {
public:
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
    motor servo_picker;
    Servo servo;
    void check_init();
    void check_zero_state ();
    void setup();
    void Thread();
    motor_controler();

private:
    void StepperMotor(int PULPin, boolean DIR, int steps, bool ENA, int velocity=10);
    void ServoMotor(int velocity=10);
    void ServoPickup(int pick);
};

/**
enable printf as string output
*/
int serial_putc( char c, struct __file * )

{
    Serial.write( c );
    return c;
}

void printf_begin(void)
{
    fdevopen( &serial_putc, 0 );
}

class blueToothHandler {
private:
    char buff[40];
    bool recieved = false;
    motor_controler &motorControler;
public:
    bool readSerial();
    byte readoff(byte data, int wait_time);
    void readtill(byte data, int wait_time);
    blueToothHandler();
    int registerCallBack(motor_controler &mc);
};

byte blueToothHandler::readoff(byte data, int wait_time) {
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

void blueToothHandler::readtill(byte data, int wait_time) {
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

bool blueToothHandler::readSerial() {
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
    int recieved = false;
    if (num==5) {
      motorControler.stepper_big_arm.target = data_final[0];
      motorControler.stepper_small_arm.target = data_final[1];
      motorControler.stepper_gimbal.target = data_final[2];
      motorControler.stepper_horizontal.target = data_final[3];
      motorControler.servo_picker.target = data_final[4];
      printf("%d, %d, %d, %d, %d", motorControler.stepper_big_arm.target, motorControler.stepper_small_arm.target , motorControler.stepper_gimbal.target, motorControler.stepper_horizontal.target, motorControler.servo_picker.target);
      recieved = true;
    } else {
      recieved = false;
    }
    return recieved;
  }
}

blueToothHandler::blueToothHandler():recieved(false) {
    printf("BlueThooth is ready!!!\n");
}

int blueToothHandler::registerCallBack(motor_controler &mc) {
    motorControler = mc;
}

/**
 * @brief motorController initializer
 */
motor_controler::motor_controler() {
  
}


/**
 * @brief setup a motor_controler
 */
void motor_controler::setup()
{
    //set output pins
    //the output pins need to connect with resistor with resistance larger than 1M
    for (int i=2;i<14;i++) {
        pinMode(i,OUTPUT);
    }
    //set input pins, PULLUP
    for (int i=30;i<35;i++) {
        pinMode (i, INPUT_PULLUP);
    }

    //servo setup
    servo.attach(PORT_SERVO);  // ������9�ϵĶ���������Ķ��������������

}


/**
* @fun_c check whether each motor is at the initial position
*/
void motor_controler::check_init() {
    if (digitalRead(SWITCH_STEPPER_BIG_ARM)==HIGH) {
        stepper_big_arm.angle = 0;
    }
    if (digitalRead(SWITCH_STEPPER_GIMBAL)==HIGH) {
        stepper_gimbal.angle = 0;
    }
    if (digitalRead(SWITCH_STEPPER_HORIZONTAL)==HIGH) {
        stepper_horizontal.angle = 0;
    }
    if (digitalRead(SWITCH_STEPPER_SMALL_ARM)==HIGH) {
        stepper_small_arm.angle = 0;
    }
}

// ��������Ӻ���
// ������StepperMotor    ���ܣ����Ʋ�������Ƿ��ѻ������򡢲���
// ������DIRPin---������Ƶ�pin
//      ENA---�ѻ�״̬��trueΪ�ѻ� (updated on 20190430)
//      DIR---�������
//      steps---�����Ĳ�������stepsΪ0�������ϵ�����������ת
//      velocity----TBD
// �޷���ֵ

void motor_controler::StepperMotor(int PULPin, boolean DIR, int steps, bool ENA, int velocity=10)
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

/**
* @fun_c check whether each motor reach the initial position
* if not, make each motor set at the initial position
* if it is, lock that motor, change the state to 1
*/
void motor_controler::check_zero_state () {
    stepper_big_arm.state = 0;
    stepper_gimbal.state=0;
    stepper_small_arm.state=0;
    stepper_horizontal.state=0;
    while (!(stepper_big_arm.state==1 && stepper_gimbal.state==1 && stepper_horizontal.state==1 && stepper_small_arm.state==1)) {
        //start from initial position, make every motor stop when touch the microswitch and denote this position by 0
        if (digitalRead(SWITCH_STEPPER_BIG_ARM)==HIGH) {
            stepper_big_arm.angle = 0;
            stepper_big_arm.state = 1;
            StepperMotor(PORT_STEPPER_BIG_ARM, true, 0, true);
        } else {
            StepperMotor(PORT_STEPPER_BIG_ARM, true, 2, false);
        }
        if (digitalRead(SWITCH_STEPPER_GIMBAL)==HIGH) {
            stepper_gimbal.angle=0;
            stepper_gimbal.state=1;
            StepperMotor(PORT_STEPPER_GIMBAL, true, 0, true);
        } else {
            StepperMotor(PORT_STEPPER_GIMBAL, true, 2, false);
        }
        if (digitalRead(SWITCH_STEPPER_SMALL_ARM)==HIGH) {
            stepper_small_arm.angle=0;
            stepper_small_arm.state=1;
            StepperMotor(PORT_STEPPER_SMALL_ARM, true, 0, true);
        } else {
            StepperMotor(PORT_STEPPER_SMALL_ARM, true, 2, false);
        }
        if (digitalRead(SWITCH_STEPPER_HORIZONTAL)==HIGH) {
            stepper_horizontal.angle=0;
            stepper_horizontal.state=1;
            StepperMotor(PORT_STEPPER_HORIZONTAL, true, 0, true);
        } else {
            StepperMotor(PORT_STEPPER_HORIZONTAL, true, 2, false);
        }
        delay(10);
        printf("check_zero_state: %d, %d, %d, %d\n", stepper_big_arm.state, stepper_gimbal.state, stepper_horizontal.state, stepper_small_arm.state);
    }
}


void motor_controler::Thread()
{
    /*
    // ���Ǽٶ�����������Խ�����ģ�鴫������ϢŪ������Ԥ���趨��struct����
    BlueToothReceiver();

    // �������������motor���س�ʼ״̬
    BackToOriginPosition();

    // ͬ���أ��Ҳ�̫ȷ�� CJB dl �Ľӿ���Σ��˴���ʱ���ա������⣬ȫ�ֱ��������𡭡��Ҿ���Ū��structure�ǻ������㣩
    //int gimbal_target = 10;

    // Ϊ�˷���debug����ÿ��ֻ��һ��motor
    // FUTURE TO DO: ������һ��ת������ȻЧ��û�иı䵫�Եø�һ�廯�ˣ�
    */
    
    safe = 0;    //not in safe mode
    while (safe==0) {
        if (stepper_big_arm.state==1 || stepper_big_arm.state==3) {
            while (stepper_big_arm.angle>stepper_big_arm.target) {
                printf("%d, %d\n",stepper_big_arm.angle,stepper_big_arm.target);
                StepperMotor(PORT_STEPPER_BIG_ARM, 1, 1, false);
                stepper_big_arm.angle--;
                delay(15);
            }
            while (stepper_big_arm.angle<stepper_big_arm.target) {
                printf("%d, %d\n",stepper_big_arm.angle,stepper_big_arm.target);
                StepperMotor(PORT_STEPPER_BIG_ARM, 0, 1, false);
                stepper_big_arm.angle++;
                delay(15);
            }
            stepper_big_arm.state+=1;
        } else {
            StepperMotor(PORT_STEPPER_BIG_ARM, 0, 0, true);
            delay(15);
        }

        if (stepper_small_arm.state==1 || stepper_small_arm.state==3) {
            while (stepper_small_arm.angle>stepper_small_arm.target) {
                printf("%d, %d\n",stepper_big_arm.angle,stepper_big_arm.target);
                StepperMotor(PORT_STEPPER_SMALL_ARM, 1, 1, false);
                stepper_small_arm.angle--;
                delay(15);
            }
            while (stepper_small_arm.angle<stepper_small_arm.target) {
                printf("%d, %d\n",stepper_big_arm.angle,stepper_big_arm.target);
                StepperMotor(PORT_STEPPER_SMALL_ARM, 0, 1, false);
                stepper_small_arm.angle++;
                delay(15);
            }
            stepper_small_arm.state+=1;
        } else {
            StepperMotor(PORT_STEPPER_SMALL_ARM, 0, 0, true);
            delay(15);
        }

        if (stepper_gimbal.state==1 || stepper_gimbal.state==3) {
            while (stepper_gimbal.angle>stepper_gimbal.target) {
                printf("%d, %d\n",stepper_big_arm.angle,stepper_big_arm.target);
                StepperMotor(PORT_STEPPER_GIMBAL, 1, 1, false);
                stepper_gimbal.angle--;
                delay(15);
            }
            while (stepper_gimbal.angle<stepper_gimbal.target) {
                printf("%d, %d\n",stepper_big_arm.angle,stepper_big_arm.target);
                StepperMotor(PORT_STEPPER_GIMBAL, 0, 1, false);
                stepper_gimbal.angle++;
                delay(15);
            }
            stepper_gimbal.state+=1;
        } else {
            StepperMotor(PORT_STEPPER_GIMBAL, 0, 0, true);
            delay(15);
        }

        if (stepper_horizontal.state==1 || stepper_horizontal.state==3) {
            while (stepper_horizontal.angle>stepper_horizontal.target) {
                printf("%d, %d\n",stepper_big_arm.angle,stepper_big_arm.target);
                StepperMotor(PORT_STEPPER_HORIZONTAL, 1, 1, false);
                stepper_horizontal.angle--;
                delay(15);
            }
            while (stepper_horizontal.angle<stepper_horizontal.target) {
                printf("%d, %d\n",stepper_big_arm.angle,stepper_big_arm.target);
                StepperMotor(PORT_STEPPER_HORIZONTAL, 0, 1, false);
                stepper_horizontal.angle++;
                delay(15);
            }
            stepper_horizontal.state+=1;
        } else {
            StepperMotor(PORT_STEPPER_HORIZONTAL, 0, 0, true);
            delay(15);
        }
        ServoMotor();
        if (stepper_horizontal.state==2 && stepper_gimbal.state==2 && stepper_big_arm.state==2 && stepper_small_arm.state==2 && servo_picker.state==2) {
            ServoPickup(1); //pickup
        }
        if (stepper_horizontal.state==4 && stepper_gimbal.state==4 && stepper_big_arm.state==4 && stepper_small_arm.state==4 && servo_picker.state==4) {
            ServoPickup(0); //release
        }
    }
    delay(100);
}


// ����趨�Ƕ��Ӻ���
// ������ServoMotor     ���ܣ����ö���ĽǶ�
// �޷���ֵ
// WARNING!!!!��
// current����һ���������������������ܶ��ת��̫����ɻ�е�ṹ����

void motor_controler::ServoMotor(int velocity=10){
    int pos=0;
    if (servo_picker.target >= servo_picker.angle){
        for(; servo_picker.angle<=servo_picker.target; servo_picker.angle++) {    // ÿ��1��
            servo.write(servo_picker.angle);        // д�Ƕȵ����
            delay(15);                // ��ʱ15ms�ö��ת��ָ��λ��
        }
    } else {
        for(; servo_picker.angle>=servo_picker.target; servo_picker.angle--) {    // ÿ��1��
            servo.write(servo_picker.angle);        // д�Ƕȵ����
            delay(15);                // ��ʱ15ms�ö��ת��ָ��λ��
        }
    }
    servo_picker.state += 1;
}


// �������Ӻ���
// ������PickUp    ���ܣ�ץ����Ʒ
// parameter: pick --- Ϊ0ʱץ�𣬲�Ϊ0�����
void motor_controler::ServoPickup(int pick){
    if (pick == 0){
        for (int i = 180; i > 0; i--){
            servo.write(i);
            delay(15);
        }
    } else {
        for (int i = 0; i<180; i++){
            servo.write(i);
            delay(15);
        }
    }
}


// �������е���ĳ�ʼλ��
// ������BackToOriginPosition()
// ������TBD
// ����ֵ��TBD
// ������������е���������ǵĳ�ʼλ��
void BackToOriginPosition()
{
    ;// TODO by CJB
}


blueToothHandler _blueToothHandler;
motor_controler motorControler;

void setup()
{
  Serial.begin(9600);
  printf_begin(); //enable printf 
  motorControler.setup();
  //motorControler.check_zero_state();
  _blueToothHandler.registerCallBack(motorControler);
}

void loop()
{
  //read blueTooth
  //printf("try bluetooth connect\n");
  while (!_blueToothHandler.readSerial()){
      printf("Warning: cannot connect to blueTooth!!!\n");
  }
  //motorControler.Thread();
}