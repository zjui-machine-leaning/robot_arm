#ifndef BLUE_TOOTH_HANDLER_HPP
#define BLUE_TOOTH_HANDLER_HPP
#include "./control.hpp"
#include "io.hpp"
#include <Arduino.h>


class blueToothHandler {
private:
    char buff[40];
    bool recieved = false;
    motor_controler &motorControler;
public:
    bool readSerial();
    blueToothHandler();
    int registerCallBack();
}
#define blueToothHandler blueToothHandler;


#endif
