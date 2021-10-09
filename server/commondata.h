//
// Created by sunwenli on 2021/10/9.
//

#ifndef COMMONDATA_H
#define COMMONDATA_H
enum StatusCode {
    ERROR = -3,
    EXIT = -2,
    DISCONNECT = -1,
    UNKNOWN = 0,
    CONNECT = 1,
    CHAT = 2,
    RENAME = 3,
};
struct CommonData {
    enum StatusCode code;
    unsigned int group;
    char message[64];
    char data[1024];
};

#endif //COMMONDATA_H
