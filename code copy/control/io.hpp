//
// Created by 86134 on 2019/6/28.
//

#ifndef CONTROL_IO_HPP
#define CONTROL_IO_HPP
#include <Arduino.h>

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


#endif //CONTROL_IO_H
