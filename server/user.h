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
    char username[20];
    char password[20];
    char email[20];
};

long GetUserCount();

struct User *GetUserByPlace(unsigned int place);
struct User *GetUserByUserName(char *nickname);
bool SetUserByPlace(struct User *user,unsigned int place);
bool RemoveUserByPlace(unsigned int place);
bool InsertUserByPlace(struct User *user,unsigned int place);

#endif //USER_H
