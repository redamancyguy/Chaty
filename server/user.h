//
// Created by sunwenli on 2021/10/8.
//
#include <stdbool.h>
#ifndef USER_H
#define USER_H

enum PrivilegesCode {
    SUPERUSER, USER, ANONYMOUS
};
struct User {
    unsigned int id;
    char nickname[20];
    char password[20];
    char email[20];
};

long GetUserCount();

struct User *GetUserById(unsigned int id);
struct User *GetUserByNickName(char *nickname);
bool SetUserById(struct User *user);



#endif //USER_H
