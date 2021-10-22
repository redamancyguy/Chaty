//
// Created by sunwenli on 2021/10/8.
//
#include <stdbool.h>
#ifndef USER_H
#define USER_H

enum Privileges {
    SUPERUSER, USER, ANONYMOUS
};
struct User {
    unsigned long id;
    enum Privileges privileges;
    char username[20];
    char password[20];
    char telephone[20];
    char email[20];
};
long GetUserCount();
long GetUserReadyPlaceByUsername(char *username);
long GetUserPlaceByUsername(char *username);
long GetUserByPlace(struct User *user,unsigned long place);
long GetUserByUserName(struct User *user,char *username);
long SetUserByPlace(struct User *user,unsigned long place);
long RemoveUserByPlace(unsigned long place);
long InsertUserByPlace(struct User *user,unsigned long place);
int CLearUsers();
void Show();
#endif //USER_H
