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

short SERVER_PORT = 9999;
int TIMEOUT = 300;
enum PrivilegesCode {
    SUPERUSER, USER, ANONYMOUS
};
struct Client {
    socklen_t length;
    struct sockaddr_in address;
    enum PrivilegesCode privileges;
    long long time;
    char NickName[20];
};

enum StatusCode {
    ERROR = -3,
    SHUTDOWN = -2,
    DISCONNECT = -1,
    UNKNOWN = 0,
    CONNECT = 1,
    CHAT = 2,
    RENAME = 3,
};
struct CommonData {
    enum StatusCode Code;
    char Message[64];
    char Data[1024];
};
typedef struct {
    unsigned int Size;
    unsigned int Capacity;
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
    table->Size = 0;
    table->Capacity = capacity;
    memset(table->clients, 0, sizeof(struct Client *) * capacity);
    return table;
}

void TableClear(ConnectionTable table) {
    for (unsigned int i = 0; i < table->Capacity; i++) {
        if (table->clients[i] != NULL) {
            free(table->clients[i]);
        }
    }
    table->Size = 0;
}

void TableDestroy(ConnectionTable table) {
    for (unsigned int i = 0; i < table->Capacity; i++) {
        if (table->clients[i] != NULL) {
            free(table->clients[i]);
        }
    }
    free(table->clients);
    free(table);
}

bool TableSet(ConnectionTable table, const struct Client *client) {
    unsigned int flag = client->address.sin_port % table->Capacity;
    unsigned int doubleLength = table->Capacity - flag < flag ? table->Capacity - flag : flag;
    int i = 0;
    while (i < doubleLength) {
        if (table->clients[flag + i] == NULL) {
            table->clients[flag + i] = (struct Client *) malloc(sizeof(struct Client));
            *table->clients[flag + i] = *client;
            table->Size++;
            return true;
        } else if (table->clients[flag + i]->address.sin_addr.s_addr == client->address.sin_addr.s_addr
                   && table->clients[flag + i]->address.sin_port == client->address.sin_port) {
            *table->clients[flag + i] = *client;
            return true;
        }
        if (table->clients[flag - i] == NULL) {
            table->clients[flag - i] = (struct Client *) malloc(sizeof(struct Client));
            *table->clients[flag - i] = *client;
            table->Size++;
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
        while (i < table->Capacity) {
            if (table->clients[i] == NULL) {
                table->clients[i] = (struct Client *) malloc(sizeof(struct Client));
                *table->clients[i] = *client;
                table->Size++;
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
                table->Size++;
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
    unsigned int flag = client->address.sin_port % table->Capacity;
    unsigned int doubleLength = table->Capacity - flag < flag ? table->Capacity - flag : flag;
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
        while (i < table->Capacity) {
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
    unsigned int flag = client->address.sin_port % table->Capacity;
    unsigned int doubleLength = table->Capacity - flag < flag ? table->Capacity - flag : flag;
    int i = 0;
    while (i < doubleLength) {
        if (table->clients[flag + i] != NULL
            && table->clients[flag + i]->address.sin_addr.s_addr == client->address.sin_addr.s_addr
            && table->clients[flag + i]->address.sin_port == client->address.sin_port) {
            free(table->clients[flag + i]);
            table->clients[flag + i] = NULL;
            table->Size--;
            return true;
        }
        if (table->clients[flag - i] != NULL
            && table->clients[flag - i]->address.sin_addr.s_addr == client->address.sin_addr.s_addr
            && table->clients[flag - i]->address.sin_port == client->address.sin_port) {
            free(table->clients[flag - i]);
            table->clients[flag - i] = NULL;
            table->Size--;
            return true;
        }
        i++;
    }
    if (flag - i == 0) {
        i = (int) (flag + i);
        while (i < table->Capacity) {
            if (table->clients[i] != NULL
                && table->clients[i]->address.sin_addr.s_addr == client->address.sin_addr.s_addr
                && table->clients[i]->address.sin_port == client->address.sin_port) {
                free(table->clients[i]);
                table->clients[i] = NULL;
                table->Size--;
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
                table->Size--;
                return true;
            }
            i--;
        }
    }
    return false;
}


int main(int argc, char *argv[]) {
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
        return -2;
    }
    puts("Bind successfully");
    ConnectionTable table = TableNew(groupSize);
    if (table == NULL) {
        puts("Create table failed");
        return -3;
    }
    printf("Create group successfully and group Size is : %d\n", groupSize);
    puts("Turn on successfully");
    struct Client clientBuf;
    struct CommonData buf;
    clientBuf.length = sizeof(clientBuf.address);
    while (true) {
        long int count = recvfrom(serverFileDescriptor, &buf, sizeof(struct CommonData), 0,
                                  (struct sockaddr *) &clientBuf.address, &clientBuf.length);
        if (count == -1) {
            puts("Receive data fail");
            return -4;
        }
        printf("count: %ld\n", count);
        printf("%hhu.", *(char *) (&clientBuf.address.sin_addr.s_addr));
        printf("%hhu.", *((char *) (&clientBuf.address.sin_addr.s_addr) + 1));
        printf("%hhu.", *((char *) (&clientBuf.address.sin_addr.s_addr) + 2));
        printf("%hhu:", *((char *) (&clientBuf.address.sin_addr.s_addr) + 3));
        printf("%d---->", clientBuf.address.sin_port);
        printf("\t Code : %d\n", buf.Code);
        printf("size: %d\n", table->Size);
        struct Client *client = TableGet(table, &clientBuf);
        if (client == NULL) {
            if (buf.Code == CONNECT) {
                strcpy(clientBuf.NickName, "Unnamed");
                clientBuf.time = time(NULL);
                if (!TableSet(table, &clientBuf)) {
                    buf.Code = ERROR;
                    strcpy(buf.Message, "Server : Connect Unsuccessfully");
                    strcpy(buf.Data, "");
                    sendto(serverFileDescriptor, &buf, sizeof(struct CommonData), 0,
                           (struct sockaddr *) &clientBuf.address, clientBuf.length);
                    continue;
                }
                strcpy(buf.Message, "Server : Connected");
                strcpy(buf.Data, "");
                sendto(serverFileDescriptor, &buf, sizeof(struct CommonData), 0,
                       (struct sockaddr *) &clientBuf.address, clientBuf.length);
            } else {
                strcpy(buf.Message, "Server : Disconnected");
                strcpy(buf.Data, "");
                sendto(serverFileDescriptor, &buf, sizeof(struct CommonData), 0,
                       (struct sockaddr *) &clientBuf.address, clientBuf.length);
            }

        } else {
            if (time(NULL) - client->time > TIMEOUT) {
                strcpy(buf.Message, "Server : Time out");
                strcpy(buf.Data, "");
                sendto(serverFileDescriptor, &buf, sizeof(struct CommonData), 0,
                       (struct sockaddr *) &clientBuf.address, clientBuf.length);
                TableErase(table, client);
                continue;
            }
            client->time = time(NULL);
            if (buf.Code == SHUTDOWN) {
                break;
            }
            if (buf.Code == DISCONNECT) {
                strcpy(buf.Message, "Server : Disconnect successfully");
                strcpy(buf.Data, "");
                sendto(serverFileDescriptor, &buf, sizeof(struct CommonData), 0,
                       (struct sockaddr *) &clientBuf.address, clientBuf.length);
                TableErase(table, &clientBuf);
            } else if (buf.Code == RENAME) {
                strcpy(TableGet(table, &clientBuf)->NickName, buf.Data);
                strcpy(buf.Message, "Server : Set username successfully");
                strcpy(buf.Data, "");
                sendto(serverFileDescriptor, &buf, sizeof(struct CommonData), 0,
                       (struct sockaddr *) &clientBuf.address, clientBuf.length);
            } else if (buf.Code == CHAT) {
                sprintf(buf.Message, "From %hhu.%hhu.%hhu.%hhu:%d NickName:%s",
                        *((char *) (&clientBuf.address.sin_addr.s_addr) + 0),
                        *((char *) (&clientBuf.address.sin_addr.s_addr) + 1),
                        *((char *) (&clientBuf.address.sin_addr.s_addr) + 2),
                        *((char *) (&clientBuf.address.sin_addr.s_addr) + 3),
                        clientBuf.address.sin_port,
                        client->NickName);
                for (int i = 0; i < table->Capacity; i++) {
                    if (table->clients[i] != NULL) {
                        if (time(NULL) - table->clients[i]->time > TIMEOUT) {
                            free(table->clients[i]);
                            table->clients[i] = NULL;
                            table->Size--;
                            continue;
                        }
                        sendto(serverFileDescriptor, &buf, sizeof(struct CommonData), 0,
                               (struct sockaddr *) &table->clients[i]->address, table->clients[i]->length);
                    }
                }
            } else {
                buf.Code = UNKNOWN;
                strcpy(buf.Message, "Unknown");
                strcpy(buf.Data, "");
                sendto(serverFileDescriptor, &buf, sizeof(struct CommonData), 0,
                       (struct sockaddr *) &clientBuf.address, clientBuf.length);
            }
        }
    }
    puts("Shut down successfully");
    TableDestroy(table);
    close(serverFileDescriptor);
    return 0;
}
