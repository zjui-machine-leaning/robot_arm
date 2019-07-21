#include "control.hpp"
#include "io.hpp"

/**
 * @brief motorController initializer
 */
motor_controler::motor_controler() {
    _blueToothHandler = blueToothHandler();
}


/**
 * @brief setup a motor_controler
 */
void motor_controler::setup()
{
    _blueToothHandler.registerCallBack(*this);  //register bluetooth callback
    printf_begin(); //enable printf
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
    servo.attach(PORT_SERVO);  // 将引脚9上的舵机与声明的舵机对象连接起来

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

// 步进电机子函数
// 函数：StepperMotor    功能：控制步进电机是否脱机、方向、步数
// 参数：DIRPin---方向控制的pin
//      ENA---脱机状态，true为脱机 (updated on 20190430)
//      DIR---方向控制
//      steps---步进的步数，若steps为0，则电机上电电磁锁死，不转
//      velocity----TBD
// 无返回值

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
    // 我们假定这个函数可以将蓝牙模块传来的信息弄进我们预先设定的struct里面
    BlueToothReceiver();

    // 这个函数将所有motor调回初始状态
    BackToOriginPosition();

    // 同样地，我不太确定 CJB dl 的接口如何，此处暂时留空。（另外，全局变量不好吗……我觉得弄个structure是画蛇添足）
    //int gimbal_target = 10;

    // 为了方便debug我们每次只动一个motor
    // FUTURE TO DO: 让他们一块转！（虽然效率没有改变但显得更一体化了）
    */
    //read blueTooth
    if (!_blueToothHandler.readSerial()){
        printf("Warning: cannot connect to blueTooth!!!\n");
        return;
    }
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


// 舵机设定角度子函数
// 函数：ServoMotor     功能：设置舵机的角度
// 无返回值
// WARNING!!!!　
// current变量一定不能随便乱来，否则可能舵机转的太猛造成机械结构的损坏

void motor_controler::ServoMotor(int velocity=10){
    int pos=0;
    if (servo_picker.target >= servo_picker.angle){
        for(; servo_picker.angle<=servo_picker.target; servo_picker.angle++) {    // 每次1°
            servo.write(servo_picker.angle);        // 写角度到舵机
            delay(15);                // 延时15ms让舵机转到指定位置
        }
    } else {
        for(; servo_picker.angle>=servo_picker.target; servo_picker.angle--) {    // 每次1°
            servo.write(servo_picker.angle);        // 写角度到舵机
            delay(15);                // 延时15ms让舵机转到指定位置
        }
    }
    servo.state += 1;
}


// 金属夹子函数
// 函数：PickUp    功能：抓起物品
// parameter: pick --- 为0时抓起，不为0则放下
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


// 返回所有电机的初始位置
// 函数：BackToOriginPosition()
// 参数：TBD
// 返回值：TBD
// 这个函数将所有电机调回他们的初始位置
void BackToOriginPosition()
{
    ;// TODO by CJB
}
