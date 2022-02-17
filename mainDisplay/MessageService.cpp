#include "Message.h"
#include "MessageService.h"
#include "Arduino.h"

MessageService::MessageService() {
//    Serial1.begin(9600);
    responseMsgBuff = (char *) malloc(14);
}

char* MessageService::buildRequest(MessageType messageType, char* body) {
    char * buffer = (char *) malloc(14);
    sprintf(buffer,"RQ|%s|%s|", enumToString(messageType), body);
    return buffer;
}

char* MessageService::buildResponse(MessageType messageType, char* body) {
    char * buffer = (char *) malloc(14);
    sprintf(buffer,"RS|%s|%s|",enumToString(messageType), body);
    return buffer;
}

Message MessageService::parseResponse(String message) {
    Message *msg = new Message();
    if (message.length() == messageLength) {
        msg->success = true;
        msg->msgType = MessageType::KA;
        return *msg;
    } else {
        msg->success = false;
        return *msg;
    }
}

char* MessageService::enumToString(MessageType messageType){
      switch(messageType){
         case ST:
            return "ST";
         case IO:
            return "IO";
        case PS:
            return "PS";
        case KA:
            return "KA";
         default:
            return "ER";
      }
}

 bool MessageService::sendRequest(char* message) {
       Serial1.write(message);
}

void MessageService::startListner() {
    int count = 0;
    while (Serial1.available()) {
        delay(2);  //delay to allow byte to arrive in input buffer
        responseMsgBuff[0] = Serial.read();
        count++;
        Serial.print("ResponseMessage:");
        Serial.println(responseMsgBuff);
    }
}
