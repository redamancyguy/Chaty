//
// Created by sunwenli on 2021/10/9.
//
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdbool.h>
#include "user.h"

#ifndef CLIENT_H
#define CLIENT_H
enum ClientStatus{
    LoggedIN,
    UnLoggedIN,
};
struct Client {
    socklen_t length;
    struct sockaddr_in address;
    struct User *user;
    char nickname[20];
    enum ClientStatus status;
    long long time;
};
typedef struct {
    unsigned int size;
    unsigned int capacity;
    struct Client **clients;
} ConnectionTable_, *ConnectionTable;


ConnectionTable TableNew(unsigned int capacity);

void TableClear(ConnectionTable table);

void TableDestroy(ConnectionTable table);

bool TableSet(ConnectionTable table, const struct Client *client);

struct Client *TableGet(ConnectionTable table, const struct Client *client);

bool TableErase(ConnectionTable table, const struct Client *client);

#endif //CLIENT_H
