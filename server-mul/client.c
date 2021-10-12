//
// Created by sunwenli on 2021/10/9.
//
#include <stdbool.h>
#include <string.h>
#include <malloc.h>
#include "client.h"
ConnectionTable TableNew(unsigned int capacity) {
    ConnectionTable table = (ConnectionTable_ *) malloc(sizeof(ConnectionTable_));
    if (table == NULL) {
        return NULL;
    }
    table->clients = (struct Client **) malloc(sizeof(struct Client *) * capacity);
    if (table->clients == NULL) {
        free(table);
        return NULL;
    }
    table->size = 0;
    table->capacity = capacity;
    memset(table->clients, 0, sizeof(struct Client *) * capacity);
    return table;
}

void TableClear(ConnectionTable table) {
    for (unsigned int i = 0; i < table->capacity; i++) {
        if (table->clients[i] != NULL) {
            free(table->clients[i]);
        }
    }
    table->size = 0;
}

void TableDestroy(ConnectionTable table) {
    for (unsigned int i = 0; i < table->capacity; i++) {
        if (table->clients[i] != NULL) {
            free(table->clients[i]);
        }
    }
    free(table->clients);
    free(table);
}

bool TableSet(ConnectionTable table, const struct Client *client) {
    unsigned int flag = client->address.sin_port % table->capacity;
    unsigned int doubleLength = table->capacity - flag < flag ? table->capacity - flag : flag;
    int i = 0;
    while (i < doubleLength) {
        if (table->clients[flag + i] == NULL) {
            table->clients[flag + i] = (struct Client *) malloc(sizeof(struct Client));
            memset(table->clients[flag + i],0,sizeof(struct Client));
            *table->clients[flag + i] = *client;
            table->size++;
            return true;
        } else if (table->clients[flag + i]->address.sin_addr.s_addr == client->address.sin_addr.s_addr
                   && table->clients[flag + i]->address.sin_port == client->address.sin_port) {
            *table->clients[flag + i] = *client;
            return true;
        }
        if (table->clients[flag - i] == NULL) {
            table->clients[flag - i] = (struct Client *) malloc(sizeof(struct Client));
            memset(table->clients[flag - i],0,sizeof(struct Client));
            *table->clients[flag - i] = *client;
            table->size++;
            return true;
        } else if (table->clients[flag - i]->address.sin_addr.s_addr == client->address.sin_addr.s_addr
                   && table->clients[flag - i]->address.sin_port == client->address.sin_port) {
            *table->clients[flag - i] = *client;
            return true;
        }
        i++;
    }
    if (flag - i == 0) {
        i = (int) (flag + i);
        while (i < table->capacity) {
            if (table->clients[i] == NULL) {
                table->clients[i] = (struct Client *) malloc(sizeof(struct Client));
                memset(table->clients[i],0,sizeof(struct Client));
                *table->clients[i] = *client;
                table->size++;
                return true;
            } else if (table->clients[i]->address.sin_addr.s_addr == client->address.sin_addr.s_addr
                       && table->clients[i]->address.sin_port == client->address.sin_port) {
                *table->clients[i] = *client;
                return true;
            }
            i++;
        }
    } else {
        i = (int) (flag - i - 1);
        while (i >= 0) {
            if (table->clients[i] == NULL) {
                table->clients[i] = (struct Client *) malloc(sizeof(struct Client));
                memset(table->clients[i],0,sizeof(struct Client));
                *table->clients[i] = *client;
                table->size++;
                return true;
            } else if (table->clients[i]->address.sin_addr.s_addr == client->address.sin_addr.s_addr
                       && table->clients[i]->address.sin_port == client->address.sin_port) {
                *table->clients[i] = *client;
                return true;
            }
            i--;
        }
    }
    return false;
}

struct Client *TableGet(ConnectionTable table, const struct Client *client) {
    unsigned int flag = client->address.sin_port % table->capacity;
    unsigned int doubleLength = table->capacity - flag < flag ? table->capacity - flag : flag;
    int i = 0;
    while (i < doubleLength) {
        if (table->clients[flag + i] != NULL
            && table->clients[flag + i]->address.sin_addr.s_addr == client->address.sin_addr.s_addr
            && table->clients[flag + i]->address.sin_port == client->address.sin_port) {
            return table->clients[flag + i];
        }
        if (table->clients[flag - i] != NULL
            && table->clients[flag - i]->address.sin_addr.s_addr == client->address.sin_addr.s_addr
            && table->clients[flag - i]->address.sin_port == client->address.sin_port) {
            return table->clients[flag - i];
        }
        i++;
    }
    if (flag - i == 0) {
        i = (int) (flag + i);
        while (i < table->capacity) {
            if (table->clients[i] != NULL
                && table->clients[i]->address.sin_addr.s_addr == client->address.sin_addr.s_addr
                && table->clients[i]->address.sin_port == client->address.sin_port) {
                return table->clients[i];
            }
            i++;
        }
    } else {
        i = (int) (flag - i - 1);
        while (i >= 0) {
            if (table->clients[i] != NULL
                && table->clients[i]->address.sin_addr.s_addr == client->address.sin_addr.s_addr
                && table->clients[i]->address.sin_port == client->address.sin_port) {
                return table->clients[i];
            }
            i--;
        }
    }
    return NULL;
}

bool TableErase(ConnectionTable table, const struct Client *client) {
    unsigned int flag = client->address.sin_port % table->capacity;
    unsigned int doubleLength = table->capacity - flag < flag ? table->capacity - flag : flag;
    int i = 0;
    while (i < doubleLength) {
        if (table->clients[flag + i] != NULL
            && table->clients[flag + i]->address.sin_addr.s_addr == client->address.sin_addr.s_addr
            && table->clients[flag + i]->address.sin_port == client->address.sin_port) {
            free(table->clients[flag + i]);
            table->clients[flag + i] = NULL;
            table->size--;
            return true;
        }
        if (table->clients[flag - i] != NULL
            && table->clients[flag - i]->address.sin_addr.s_addr == client->address.sin_addr.s_addr
            && table->clients[flag - i]->address.sin_port == client->address.sin_port) {
            free(table->clients[flag - i]);
            table->clients[flag - i] = NULL;
            table->size--;
            return true;
        }
        i++;
    }
    if (flag - i == 0) {
        i = (int) (flag + i);
        while (i < table->capacity) {
            if (table->clients[i] != NULL
                && table->clients[i]->address.sin_addr.s_addr == client->address.sin_addr.s_addr
                && table->clients[i]->address.sin_port == client->address.sin_port) {
                free(table->clients[i]);
                table->clients[i] = NULL;
                table->size--;
                return true;
            }
            i++;
        }
    } else {
        i = (int) (flag - i - 1);
        while (i >= 0) {
            if (table->clients[i] != NULL
                && table->clients[i]->address.sin_addr.s_addr == client->address.sin_addr.s_addr
                && table->clients[i]->address.sin_port == client->address.sin_port) {
                free(table->clients[i]);
                table->clients[i] = NULL;
                table->size--;
                return true;
            }
            i--;
        }
    }
    return false;
}
