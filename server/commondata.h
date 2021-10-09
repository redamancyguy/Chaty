//
// Created by sunwenli on 2021/10/9.
//

#ifndef COMMONDATA_H
#define COMMONDATA_H
enum StatusCode {
    ERROR,
    EXIT,
    DISCONNECT,
    UNKNOWN,
    CONNECT,
    CHAT,
    RENAME,
    LOGIN,
    LOGOUT,
    REGISTER,
    UNREGISTER,
};
struct CommonData {
    enum StatusCode code;
    unsigned int group;
    char message[64];
    char data[1024];
};

#endif //COMMONDATA_H
