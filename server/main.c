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
int TIMEOUT = 7;
enum PrivilegesCode {
    SUPERUSER, USER, ANONYMOUS
};
typedef struct {
    socklen_t length;
    struct sockaddr_in address;
    enum PrivilegesCode privileges;
    long long time;
    char NickName[20];
} Client_, *Client;

enum StatusCode {
    ERROR = -2,
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
    Client *clients;
} ConnectionTable_, *ConnectionTable;

ConnectionTable TableNew(unsigned int capacity) {
    ConnectionTable table = malloc(sizeof(ConnectionTable_));
    if (table == NULL) {
        return NULL;
    }
    table->clients = malloc(sizeof(Client) * capacity);
    if (table->clients == NULL) {
        free(table);
        return NULL;
    }
    table->Size = 0;
    table->Capacity = capacity;
    memset(table->clients, 0, sizeof(Client) * capacity);
    return table;
}

void TableDestroy(ConnectionTable table) {
    for (int i = 0; i < table->Capacity; i++) {
        if (table->clients[i] != NULL) {
            free(table->clients[i]);
        }
    }
    free(table);
}

bool TableSet(ConnectionTable table, Client client) {
    unsigned int flag = client->address.sin_port % table->Capacity;
    unsigned int doubleLength = table->Capacity - flag < flag ? table->Capacity - flag : flag;
    int i = 0;
    while (i < doubleLength) {
        if (table->clients[flag + i] == NULL) {
            table->clients[flag + i] = malloc(sizeof(Client_));
            *table->clients[flag + i] = *client;
            table->Size++;
            return true;
        } else if (table->clients[flag + i]->address.sin_addr.s_addr == client->address.sin_addr.s_addr
                   && table->clients[flag + i]->address.sin_port == client->address.sin_port) {
            *table->clients[flag + i] = *client;
            return true;
        }
        if (table->clients[flag - i] == NULL) {
            table->clients[flag - i] = malloc(sizeof(Client_));
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
        i = (int)(flag + i);
        while (i < table->Capacity) {
            if (table->clients[i] == NULL) {
                table->clients[i] = malloc(sizeof(Client_));
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
        i = (int)(flag - i - 1);
        while (i >= 0) {
            if (table->clients[i] == NULL) {
                table->clients[i] = malloc(sizeof(Client_));
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

Client TableGet(ConnectionTable table, Client client) {
    unsigned int flag = client->address.sin_port % table->Capacity;
    unsigned int doubleLength = table->Capacity - flag < flag ? table->Capacity - flag : flag;
    int i = 0;
    while (i < doubleLength) {
        if (table->clients[flag + i] != NULL
            && table->clients[flag + i]->address.sin_addr.s_addr == client->address.sin_addr.s_addr
            && table->clients[flag + i]->address.sin_port == client->address.sin_port) {
            return table->clients[flag + i];
        }
        if (table->clients[flag - i] != NULL &&
            table->clients[flag - i]->address.sin_addr.s_addr == client->address.sin_addr.s_addr
            && table->clients[flag - i]->address.sin_port == client->address.sin_port) {
            return table->clients[flag - i];
        }
        i++;
    }
    if (flag - i == 0) {
        i = (int)(flag + i);
        while (i < table->Capacity) {
            if (table->clients[i] != NULL
                && table->clients[i]->address.sin_addr.s_addr == client->address.sin_addr.s_addr
                && table->clients[i]->address.sin_port == client->address.sin_port) {
                return table->clients[i];
            }
            i++;
        }
    } else {
        i = (int)(flag - i - 1);
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

bool TableErase(ConnectionTable table, Client client) {
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
        if (table->clients[flag - i] != NULL &&
            table->clients[flag - i]->address.sin_addr.s_addr == client->address.sin_addr.s_addr
            && table->clients[flag - i]->address.sin_port == client->address.sin_port) {
            free(table->clients[flag - i]);
            table->clients[flag - i] = NULL;
            table->Size--;
            return true;
        }
        i++;
    }
    if (flag - i == 0) {
        i = (int)(flag + i);
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
        i = (int)(flag - i - 1);
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
    int serverFileDescriptor;
    struct sockaddr_in serverAddress;

    serverFileDescriptor = socket(AF_INET, SOCK_DGRAM, 0); //AF_INET:IPV4;SOCK_DGRAM:UDP
    if (serverFileDescriptor < 0) {
        printf("create socket fail!\n");
        return -1;
    }

    memset(&serverAddress, 0, sizeof(serverFileDescriptor));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY; //IP地址，需要进行网络序转换，INADDR_ANY：本地地址
//    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY); //IP地址，需要进行网络序转换，INADDR_ANY：本地地址
    serverAddress.sin_port = htons(SERVER_PORT);  //端口号，需要网络序转换
    if (bind(serverFileDescriptor, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
        printf("socket bind fail!\n");
        return -1;
    }

    ConnectionTable table = TableNew(65536);
    if(table == NULL){
        puts("create table failed !");
        return -1;
    }
    Client_ clientBuf;
    struct CommonData buf;
    clientBuf.length = sizeof(clientBuf.address);
    while (true) {
        long int count = recvfrom(serverFileDescriptor, &buf, sizeof(struct CommonData), 0,
                                  (struct sockaddr *) &clientBuf.address, &clientBuf.length);
        if (count == -1) {
            printf("receive data fail!\n");
            return 0;
        }
        printf("%hhu.", *(char *) (&clientBuf.address.sin_addr.s_addr));
        printf("%hhu.", *((char *) (&clientBuf.address.sin_addr.s_addr) + 1));
        printf("%hhu.", *((char *) (&clientBuf.address.sin_addr.s_addr) + 2));
        printf("%hhu:", *((char *) (&clientBuf.address.sin_addr.s_addr) + 3));
        printf("%d---->", clientBuf.address.sin_port);
        printf(":%d %s \n", buf.Code, buf.Message);
        printf("size: %d\n", table->Size);
        Client client = TableGet(table, &clientBuf);
        if (client == NULL) {
            puts("1");
            if (buf.Code == CONNECT) {
                puts("2");
                strcpy(clientBuf.NickName, "Unnamed");
                clientBuf.time = time(NULL);
                TableSet(table, &clientBuf);
                buf.Code = CONNECT;
                strcpy(buf.Message, "Server : Connected !");
                strcpy(buf.Data, "");
                sendto(serverFileDescriptor, &buf, sizeof(struct CommonData), 0,
                       (struct sockaddr *) &clientBuf.address, clientBuf.length);
            }else{
                strcpy(buf.Message, "Server : Disconnected !");
                strcpy(buf.Data, "");
                sendto(serverFileDescriptor, &buf, sizeof(struct CommonData), 0,
                       (struct sockaddr *) &clientBuf.address, clientBuf.length);

                puts("3");
            }

        } else{
            if(time(NULL) - client->time > TIMEOUT){
                buf.Code = DISCONNECT;
                strcpy(buf.Message, "Server : time out !");
                strcpy(buf.Data, "");
                sendto(serverFileDescriptor, &buf, sizeof(struct CommonData), 0,
                       (struct sockaddr *) &clientBuf.address, clientBuf.length);
                TableErase(table, client);
                continue;
            }
            client->time = time(NULL);
            if (buf.Code == DISCONNECT) {
                puts("4");
                buf.Code = DISCONNECT;
                strcpy(buf.Message, "Server : Disconnected !");
                strcpy(buf.Data, "");
                sendto(serverFileDescriptor, &buf, sizeof(struct CommonData), 0,
                       (struct sockaddr *) &clientBuf.address, clientBuf.length);
                TableErase(table, &clientBuf);
            } else if (buf.Code == RENAME) {
                puts("5");
                strcpy(TableGet(table, &clientBuf)->NickName, buf.Data);
                strcpy(buf.Message, "Server : Set username successful !");
                strcpy(buf.Data, "");
                sendto(serverFileDescriptor, &buf, sizeof(struct CommonData), 0,
                       (struct sockaddr *) &clientBuf.address, clientBuf.length);
            } else if (buf.Code == CHAT) {
                puts("6");
                sprintf(buf.Message, "From %hhu.%hhu.%hhu.%hhu:%d Name:%s",
                        *((char *) (&clientBuf.address.sin_addr.s_addr) + 0),
                        *((char *) (&clientBuf.address.sin_addr.s_addr) + 1),
                        *((char *) (&clientBuf.address.sin_addr.s_addr) + 2),
                        *((char *) (&clientBuf.address.sin_addr.s_addr) + 3),
                        clientBuf.address.sin_port,
                        TableGet(table, &clientBuf)->NickName);
                for (int i = 0; i < table->Capacity; i++) {
                    if (table->clients[i] != NULL) {
                        clientBuf = *table->clients[i];
                        if (time(NULL) - clientBuf.time > TIMEOUT) {
                            TableErase(table, table->clients[i]);
                            continue;
                        }
                        sendto(serverFileDescriptor, &buf, sizeof(struct CommonData), 0,
                               (struct sockaddr *) &clientBuf.address, clientBuf.length);
                    }
                }
            } else {
                puts("7");
                buf.Code = UNKNOWN;
                strcpy(buf.Message, "unknown");
                strcpy(buf.Data, "");
                sendto(serverFileDescriptor, &buf, sizeof(struct CommonData), 0,
                       (struct sockaddr *) &clientBuf.address, clientBuf.length);
            }
        }
    }
    TableDestroy(table);
    close(serverFileDescriptor);
    return 0;
}