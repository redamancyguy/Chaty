//
// Created by sunwenli on 2021/10/9.
//
#include "client.h"
#ifndef COMMONDATA_H
#define COMMONDATA_H
enum StatusCode {
    EXIT = -3,
    DISCONNECT = -2,
    ERROR = -1,
    UNKNOWN = 0,
    CHAT = 1,
    CONNECT = 2,
    RENAME = 3,
    LOGIN = 4,
    LOGOUT = 5,
    REGISTER = 6,
    UNREGISTER = 7,
};
struct CommonData {
    enum StatusCode code;
    unsigned int group;
    char message[64];
    char data[1024];
};
struct Message{
    struct Client client;
    struct CommonData data;
};
#endif //COMMONDATA_H
