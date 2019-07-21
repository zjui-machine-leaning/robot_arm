
#include "blueTooth.hpp"
#include "control.hpp"
#include "io.hpp"
//本次实验电机驱动采用共阴极接线方式
//将步进电机的参数写到一个子函数中，并加入限位开关，触动限位开关可实现步进电机停转

//20190501 in Zijingang Campus

// 舵机控制
#include <Servo.h>    // 声明调用Servo.h库
#include <Arduino.h>

void setup()
{
  Serial.begin(9600);
  motorControler motorControler = motor_controler();
  motorControler.setup();
}

void loop()
{
  motorControler.check_zero_state();
  motorControler.Thread();
}
