//
// Created by 86134 on 2019/6/28.
//
#include "blueTooth.hpp"
#include "control.hpp"

bool blueToothHandler::readSerial() {
    recieved = false;
    while (!Serial.available()){
        delay(wait_time);
    }
    while (!recieved) {
        buff = Serial.readBytes(buff,40);
        for (int t=0; i<40; i++) {
            char ch = buff[i];
            if (ch=='X' && i<=28 && buff[i+1]=='S') {
                int temp = 0;
                motorControler.stepper_big_arm.target = (temp | buff[i+2]<<4) | buff[i+3];
                temp = 0;
                motorControler.stepper_small_arm.target = (temp | buff[i+3]<<4) | buff[i+5];
                temp = 0;
                motorControler.stepper_gimbal.target = (temp | buff[i+6]<<4) | buff[i+7];
                temp = 0;
                motorControler.stepper_horizontal.target = (temp | buff[i+8]<<4) | buff[i+9];
                temp = 0;
                motorControler.servo.target = (temp | buff[i+10]<<4) | buff[i+11];
                Serial.println("%d, %d, %d, %d, %d", motorControler.stepper_big_arm.target, motorControler.stepper_small_arm.target , motorControler.stepper_gimbal.target, motorControler.stepper_horizontal.target, motorControler.servo_picker.target);
                recieved = true;
                break;
            }
        }
    }
    return recieved;
}

blueToothHandler::blueToothHandler():recieved(false) {
    printf("BlueThooth is ready!!!\n");
}

int blueToothHandler::registerCallBack(motor_controler &mc) {
    motorControler = mc;
}
