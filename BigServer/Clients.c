//
// Created by sunwenli on 2021/10/25.
//
#include <malloc.h>
#include "Clients.h"


struct Clients *ClientsNew(){
    struct Clients *clients = (struct Clients*) malloc(sizeof(struct Clients));
    if(clients == NULL){
        perror("create users failed");
        return NULL;
    }
    for (int i = 0; i < 65536; i++) {
        clients->clients[i] = HashNew(1024);
        if (clients->clients[i] == NULL) {
            for(int j=0;j<i;j++){
                HashDestroy(clients->clients[j]);
            }
            return NULL;
        }
    }
    return clients;
}

void ClientsDestroy(struct Clients *clients){
    for (int i = 0; i < 65536; i++) {
        HashDestroy(clients->clients[i]);
    }
    free(clients);
}

bool ClientsInsert(struct Clients *clients,struct sockaddr_in address,struct Client *client){
    Hash hash = clients->clients[address.sin_port];
    return HashInsert(hash,(void*)(unsigned long long)address.sin_addr.s_addr,(void*)client);
}

struct Client *ClientGet(struct Clients *clients,struct sockaddr_in address){
    Hash hash = clients->clients[address.sin_port];
    return (struct Client *)HashGet(hash,(void*)(unsigned long long)address.sin_addr.s_addr);
}

bool ClientSet(struct Clients *clients,struct sockaddr_in address,struct Client *client){
    Hash hash = clients->clients[address.sin_port];
    return HashSet(hash,(void*)(unsigned long long)address.sin_addr.s_addr,(void*)client);
}
bool ClientErase(struct Clients *clients,struct sockaddr_in address){
    Hash hash = clients->clients[address.sin_port];
    return HashErase(hash,(void*)(unsigned long long)address.sin_addr.s_addr);
}

