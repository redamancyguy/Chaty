#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <time.h>
#include "user.h"
short SERVER_PORT = 9999;
int TIMEOUT = 300;

struct Client {
    socklen_t length;
    struct sockaddr_in address;
    struct User user;
    long long time;
};

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
typedef struct {
    unsigned int size;
    unsigned int capacity;
    struct Client **clients;
} ConnectionTable_, *ConnectionTable;

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


int main(int argc, char *argv[]) {
    for(int i=0;i<argc;i++){
        puts(argv[i]);
    }
    unsigned int groupNum = 1024;
    unsigned int groupSize = 1024;
    int serverFileDescriptor;
    struct sockaddr_in serverAddress;
    serverFileDescriptor = socket(AF_INET, SOCK_DGRAM, 0); //AF_INET:IPV4;SOCK_DGRAM:UDP
    if (serverFileDescriptor < 0) {
        puts("Create socket fail!");
        return -1;
    }
    puts("Create socket successfully");
    memset(&serverAddress, 0, sizeof(serverFileDescriptor));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(SERVER_PORT);
    if (bind(serverFileDescriptor, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
        puts("Socket bind fail");
        close(serverFileDescriptor);
        return -2;
    }
    puts("Bind successfully");
    ConnectionTable tables[groupNum];
    memset(tables,0,sizeof(ConnectionTable)*groupNum);
    for (unsigned int i = 0; i < 1024; i++) {
        tables[i] = TableNew(groupSize);
        if (tables[i] == NULL) {
            puts("Create table failed");
            for (unsigned int j = 0; j < i; j++){
                TableDestroy(tables[j]);
            }
            close(serverFileDescriptor);
            return -3;
        }
    }
    printf("Create Group table successfully \nThe size of each group is : %d\n", groupSize);
    puts("Turn on successfully");
    struct Client clientBuf;
    struct DataBuf {
        enum StatusCode code;
        unsigned int group;
        char message[64];
        char data[1024];
        char others[1024];
    };
    struct DataBuf buf;
    clientBuf.length = sizeof(clientBuf.address);
    while (true) {
        long int count = recvfrom(serverFileDescriptor, &buf, sizeof(struct DataBuf), 0,
                                  (struct sockaddr *) &clientBuf.address, &clientBuf.length);
        if (count == -1) {
            puts("Receive data fail");
            break;
        } else if (count != sizeof(struct CommonData)) {
            puts("Invalid data package");
            continue;
        }
        printf("%hhu.", *(char *) (&clientBuf.address.sin_addr.s_addr));
        printf("%hhu.", *((char *) (&clientBuf.address.sin_addr.s_addr) + 1));
        printf("%hhu.", *((char *) (&clientBuf.address.sin_addr.s_addr) + 2));
        printf("%hhu:", *((char *) (&clientBuf.address.sin_addr.s_addr) + 3));
        printf("%d", clientBuf.address.sin_port);
        printf("\t Code : %d\tGroup : %d\t", buf.code, buf.group);
        if (buf.group >= groupSize) {
            puts("");
            continue;
        }
        ConnectionTable table = tables[buf.group];
        printf("size: %d\n", table->size);
        struct Client *client = TableGet(table, &clientBuf);
        if (client == NULL) {
            if (buf.code == CONNECT) {
                strcpy(clientBuf.user.nickname, "Unnamed");
                clientBuf.time = time(NULL);
                if (!TableSet(table, &clientBuf)) {
                    buf.code = ERROR;
                    strcpy(buf.message, "Server : Connect unsuccessfully");
                } else {
                    strcpy(buf.message, "Server : Connect successfully");
                }
            } else {
                strcpy(buf.message, "Server : You haven't connected yet");
            }

        } else {
            if (time(NULL) - client->time > TIMEOUT) {
                strcpy(buf.message, "Server : Time out");
                TableErase(table, client);
            } else {
                client->time = time(NULL);
                if (buf.code == EXIT) {
                    break;
                } else if (buf.code == DISCONNECT) {
                    TableErase(table, &clientBuf);
                    strcpy(buf.message, "Server : Disconnect successfully");
                } else if (buf.code == RENAME) {
                    strcpy(TableGet(table, &clientBuf)->user.nickname, buf.data);
                    sprintf(buf.message, "Server : Set username (Name:%s) successfully", buf.data);
                } else if (buf.code == CHAT) {
                    sprintf(buf.message, "From %hhu.%hhu.%hhu.%hhu:%d NickName:%s",
                            *((char *) (&clientBuf.address.sin_addr.s_addr) + 0),
                            *((char *) (&clientBuf.address.sin_addr.s_addr) + 1),
                            *((char *) (&clientBuf.address.sin_addr.s_addr) + 2),
                            *((char *) (&clientBuf.address.sin_addr.s_addr) + 3),
                            clientBuf.address.sin_port,
                            client->user.nickname);
                    for (int i = 0; i < table->capacity; i++) {
                        if (table->clients[i] != NULL) {
                            if (time(NULL) - table->clients[i]->time > TIMEOUT) {
                                free(table->clients[i]);
                                table->clients[i] = NULL;
                                table->size--;
                                continue;
                            }
                            sendto(serverFileDescriptor, &buf, sizeof(struct CommonData), 0,
                                   (struct sockaddr *) &table->clients[i]->address, table->clients[i]->length);
                        }
                    }
                    continue;
                } else {
                    buf.code = UNKNOWN;
                    strcpy(buf.message, "Unknown");
                }
            }
        }
        strcpy(buf.data, "");
        sendto(serverFileDescriptor, &buf, sizeof(struct CommonData), 0,
               (struct sockaddr *) &clientBuf.address, clientBuf.length);
    }
    puts("Shutdown server successfully");
    for (unsigned int i = 0; i < 1024; i++) {
        TableDestroy(tables[i]);
    }
    puts("Delete table successfully");
    close(serverFileDescriptor);
    puts("Close socket successfully");
    return 0;
}
