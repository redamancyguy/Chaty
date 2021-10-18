//
// Created by sunwenli on 2021/10/17.
//

#ifndef CLIENT_USER_H
#define CLIENT_USER_H
enum Privileges {
    SUPERUSER, USER, ANONYMOUS
};
struct User {
    unsigned long id;
    enum Privileges privileges;
    char username[20];
    char password[20];
    char email[20];
};
#endif //CLIENT_USER_H
