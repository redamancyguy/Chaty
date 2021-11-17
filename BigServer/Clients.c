//
// Created by sunwenli on 2021/11/16.
//
#include <malloc.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include "Clients.h"

struct Block {
    pthread_rwlock_t rwlock;
    unsigned int num;
    struct Client *clients[65536];
};
struct Main {
    pthread_rwlock_t rwlock;
    struct Block *blocks[65536];
};

struct Client *ClientNew() {
    Tree tree = TreeNew();
    if (tree == NULL) {
        return NULL;
    }
    struct Client *client = (struct Client *) malloc(sizeof(struct Client));
    if (client == NULL) {
        TreeDestroy(tree);
        return NULL;
    }
//    if(pthread_rwlock_init(&client->rwlock,NULL)!=0){
//        free(client);
//        TreeDestroy(tree);
//        return NULL;
//    }
    client->groups = tree;
    client->time = time(NULL);
    return client;
}

void ClientDestroy(struct Client *client) {
//    pthread_rwlock_destroy(&client->rwlock);
    TreeDestroy(client->groups);
    free(client);
}

Clients ClientsNew() {
    struct Main *main = calloc(sizeof(struct Main), 1);
    if (pthread_rwlock_init(&main->rwlock, NULL) != 0) {
        free(main);
        return NULL;
    }
    return main;
}

void ClientsDestroy(Clients clients) {
    for (int i = 0; i < 65536; i++) {
        if (clients->blocks[i] == NULL) {
            continue;
        }
        struct Block *block = clients->blocks[i];
        for (int j = 0; j < 65536; j++) {
            if (block->clients[j] == NULL) {
                continue;
            }
            ClientDestroy(block->clients[j]);
        }
        pthread_rwlock_destroy(&block->rwlock);
        free(block);
    }
    pthread_rwlock_destroy(&clients->rwlock);
    free(clients);
}

int ClientsInsert(Clients clients, unsigned id) {
    unsigned short index[2];
    memcpy(&index, &id, sizeof(unsigned));
    pthread_rwlock_rdlock(&clients->rwlock);
    if (clients->blocks[index[1]] == NULL) {
        pthread_rwlock_unlock(&clients->rwlock);
        pthread_rwlock_wrlock(&clients->rwlock);
        puts("write main");
        if ((clients->blocks[index[1]] = calloc(sizeof(struct Block), 1)) == NULL) {
            pthread_rwlock_unlock(&clients->rwlock);
            return -1;
        }
        if (pthread_rwlock_init(&clients->blocks[index[1]]->rwlock, NULL) != 0) {
            free(clients->blocks[index[1]]);
            clients->blocks[index[1]] = NULL;
            pthread_rwlock_unlock(&clients->rwlock);
            return -1;
        }
    }
    pthread_rwlock_unlock(&clients->rwlock);
    pthread_rwlock_rdlock(&clients->rwlock);
    pthread_rwlock_wrlock(&clients->blocks[index[1]]->rwlock);
    if (clients->blocks[index[1]]->clients[index[0]] == NULL) {
        if ((clients->blocks[index[1]]->clients[index[0]] = ClientNew()) == NULL) {
            pthread_rwlock_unlock(&clients->rwlock);
            pthread_rwlock_unlock(&clients->blocks[index[1]]->rwlock);
            return -1;
        }
        ++clients->blocks[index[1]]->num;
        pthread_rwlock_unlock(&clients->rwlock);
        pthread_rwlock_unlock(&clients->blocks[index[1]]->rwlock);
        return 0;
    }
    pthread_rwlock_unlock(&clients->rwlock);
    pthread_rwlock_rdlock(&clients->blocks[index[1]]->rwlock);
    return 1;
}

int ClientsDelete(Clients clients, unsigned id) {
    puts("1");
    unsigned short index[2];
    memcpy(&index, &id, sizeof(unsigned));
    pthread_rwlock_rdlock(&clients->rwlock);
    if (clients->blocks[index[1]] == NULL) {
        puts("2");
        pthread_rwlock_unlock(&clients->rwlock);
        return -1;
    }
    puts("3");
    pthread_rwlock_rdlock(&clients->blocks[index[1]]->rwlock);
    if (clients->blocks[index[1]]->clients[index[0]] == NULL) {
        puts("4");
        pthread_rwlock_unlock(&clients->rwlock);
        pthread_rwlock_unlock(&clients->blocks[index[1]]->rwlock);
        return -1;
    }
    puts("5");
    pthread_rwlock_unlock(&clients->blocks[index[1]]->rwlock);
    pthread_rwlock_wrlock(&clients->blocks[index[1]]->rwlock);
    ClientDestroy(clients->blocks[index[1]]->clients[index[0]]);
    clients->blocks[index[1]]->clients[index[0]] = NULL;
    if (--clients->blocks[index[1]]->num == 0) {
        pthread_rwlock_unlock(&clients->blocks[index[1]]->rwlock);
        free(clients->blocks[index[1]]);
        clients->blocks[index[1]] = NULL;
        pthread_rwlock_unlock(&clients->rwlock);
    } else {
        puts("6");
        pthread_rwlock_unlock(&clients->rwlock);
        pthread_rwlock_unlock(&clients->blocks[index[1]]->rwlock);
    }
    puts("7");
    return 0;
}

struct Client *ClientsGet(Clients clients, unsigned id) {
    struct Client *result = NULL;
    unsigned short index[2];
    memcpy(&index, &id, sizeof(unsigned));
    pthread_rwlock_rdlock(&clients->rwlock);
    if (clients->blocks[index[1]] == NULL) {
        pthread_rwlock_unlock(&clients->rwlock);
        return NULL;
    }
    pthread_rwlock_rdlock(&clients->blocks[index[1]]->rwlock);
    result = clients->blocks[index[1]]->clients[index[0]];
    pthread_rwlock_unlock(&clients->rwlock);
    pthread_rwlock_unlock(&clients->blocks[index[1]]->rwlock);
    return result;
}




