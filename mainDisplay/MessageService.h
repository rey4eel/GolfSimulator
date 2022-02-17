#ifndef MessageService_h
#define MessageService_h

#include "Arduino.h"
#include "Message.h"


class MessageService {
    public:
        MessageService();
        char* responseMsgBuff;
        char* buildRequest(MessageType messageType, char* body);
        char* buildResponse(MessageType messageType, char* body);
        bool sendRequest(char* message);
        void startListner();
        Message parseResponse(String message);
        char* enumToString(MessageType messageType);
};

#endif