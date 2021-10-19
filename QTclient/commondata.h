//
// Created by sunwenli on 2021/10/9.
//
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
};
struct CommonData {
    enum StatusCode code;
    unsigned int group;
    char message[64];
    char data[1024];
};
#endif //COMMONDATA_H
