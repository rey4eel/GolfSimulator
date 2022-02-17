#ifndef Message_h
#define Message_h

#include "Arduino.h"

enum MessageType {
    ST,
    IO,
    PS,
    KA
};

const int messageLength = 13 + 1;

class Message {
    public:
        bool success;
        MessageType msgType;
        byte feederState; 
        byte middleState; 
        byte teeState; 
        byte feederSensor; 
        byte middleSensor; 
        byte teeSensor; 
};

#endif
