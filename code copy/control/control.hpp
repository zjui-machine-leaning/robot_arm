//
// Created by Anbang on 2019/6/28.
//

#ifndef CONTROL_HPP
#define CONTROL_HPP

//本次实验电机驱动采用共阴极接线方式
//将步进电机的参数写到一个子函数中，并加入限位开关，触动限位开关可实现步进电机停转

//20190501 in Zijingang Campus

// 舵机控制
#include <Servo.h>    // 声明调用Servo.h库
#include "io.hpp"
#include "blueTooth.hpp"
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
    blueToothHandler _blueToothHandler;
    void StepperMotor(int PULPin, boolean DIR, int steps, bool ENA, int velocity=10);
    void ServoMotor(int velocity=10);
    void ServoPickup(int pick);
};

#define motor_controler motor_controler;
#endif
