//
// Created by sunwenli on 2021/10/9.
//
#include "client.h"
#ifndef COMMONDATA_H
#define COMMONDATA_H
enum StatusCode {
    UNREGISTER = -5,
    LOGOUT = -4,
    EXIT = -3,
    DISCONNECT = -2,
    ERROR = -1,
    UNKNOWN = 0,
    CHAT = 1,
    CONNECT = 2,
    RENAME = 3,
    LOGIN = 4,
    REGISTER = 5,
    EMAIL = 6,
    CHANGE = 7
};
struct CommonData {
    enum StatusCode code;
    char data[1024];
};
struct Message{
    struct Client client;
    struct CommonData data;
};
#endif //COMMONDATA_H
