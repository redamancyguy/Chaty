//
// Created by sunwenli on 2021/11/16.
//
#include <malloc.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include "Clients.h"

static unsigned NUM = 65536*1024;
struct Client *ClientNew() {
    Tree tree = TreeNew();
    if (tree == NULL) {
        return NULL;
    }
    struct Client *client = (struct Client *) malloc(sizeof(struct Client));
    if (client == NULL) {
        return NULL;
    }
    client->groups = tree;
    client->time = time(NULL);
    return client;
}

void ClientDestroy(struct Client *client) {
    TreeDestroy(client->groups);
    free(client);
}

Clients ClientsNew() {
    return calloc(sizeof(void *), NUM);
}

void ClientsDestroy(Clients clients) {
    for (int i = 0; i < NUM; i++) {
        if (clients[i] != NULL) {
            ClientDestroy(clients[i]);
        }
    }
    free(clients);
}

int ClientsInsert(Clients clients, unsigned id) {
    if(id>=NUM){
        return -1;
    }
    if(clients[id] != NULL){
        return 1;
    }
    if(((clients[id] = ClientNew())) == NULL){
        return -1;
    }
    return 0;
}

int ClientsDelete(Clients clients, unsigned id) {
    if(id>=NUM){
        return -1;
    }
    if (clients[id] != NULL) {
        ClientDestroy(clients[id]);
        clients[id] = NULL;
        return 0;
    }
    return 1;
}

struct Client *ClientsGet(Clients clients, unsigned id) {
    if(id>=NUM){
        return NULL;
    }
    return clients[id];
}




