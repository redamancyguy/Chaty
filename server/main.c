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

#define SERVER_PORT 9999
#define MAXPORTNUM 65536
#define TIMEOUT 300
enum PrivilegesCode {
    SUPERUSER = 1,
    USER = 2,
    ANONYMOUS = 3
};
typedef struct {
    socklen_t length;
    struct sockaddr_in address;
    enum PrivilegesCode privileges;
    long long time;
    char NickName[20];
} Client_, *Client;

enum StatusCode {
    UNKNOWN = -2,
    ERROR = -1,
    DISCONNECT = 0,
    CONNECT = 1,
    CHAT = 2,
    SETUSERNAME = 3,
            OK = 4
};
struct CommonData {
    enum StatusCode Code;
    char Message[64];
    char Data[1024];
};
typedef struct {
    unsigned int Size;
    Client clients[MAXPORTNUM];
} ConnectionTable_, *ConnectionTable;

ConnectionTable NewTable() {
    ConnectionTable table = malloc(sizeof(ConnectionTable_));
    if (table == NULL) {
        return NULL;
    }
    memset(table->clients, 0, sizeof(Client) * MAXPORTNUM);
    table->Size = 0;
    return table;
}

void Destroy(ConnectionTable table) {
    for (int i = 0; i < MAXPORTNUM; i++) {
        if (table->clients[i] != NULL) {
            free(table->clients[i]);
        }
    }
    free(table);
}

bool Set(ConnectionTable table, Client client) {
    int flag = client->address.sin_port % MAXPORTNUM;
    int doubleLength = MAXPORTNUM - flag < flag ? MAXPORTNUM - flag : flag;
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
        i = flag + i;
        while (i < MAXPORTNUM) {
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
        i = flag - i - 1;
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

Client Get(ConnectionTable table, Client client) {
    int flag = client->address.sin_port % MAXPORTNUM;
    int doubleLength = MAXPORTNUM - flag < flag ? MAXPORTNUM - flag : flag;
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
        i = flag + i;
        while (i < MAXPORTNUM) {
            if (table->clients[i] != NULL
                && table->clients[i]->address.sin_addr.s_addr == client->address.sin_addr.s_addr
                && table->clients[i]->address.sin_port == client->address.sin_port) {
                return table->clients[i];
            }
            i++;
        }
    } else {
        i = flag - i - 1;
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

bool Erase(ConnectionTable table, Client client) {
    int flag = client->address.sin_port % MAXPORTNUM;
    int doubleLength = MAXPORTNUM - flag < flag ? MAXPORTNUM - flag : flag;
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
        i = flag + i;
        while (i < MAXPORTNUM) {
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
        i = flag - i - 1;
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

    ConnectionTable table = NewTable();
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
        printf("size: %d\n",table->Size);
        if (Get(table, &clientBuf) == NULL) {
            if(buf.Code != CONNECT){
                continue;
            }
            strcpy(clientBuf.NickName, "Unnamed");
            clientBuf.time = time(NULL);
            Set(table, &clientBuf);
            buf.Code = CONNECT;
            strcpy(buf.Message, "Server : Connected !");
            strcpy(buf.Data, "");
            sendto(serverFileDescriptor, &buf, sizeof(struct CommonData), 0,
                   (struct sockaddr *) &clientBuf.address, clientBuf.length);
        } else if (buf.Code == DISCONNECT) {
            buf.Code = DISCONNECT;
            strcpy(buf.Message, "Server : Disconnected !");
            strcpy(buf.Data, "");
            sendto(serverFileDescriptor, &buf, sizeof(struct CommonData), 0,
                   (struct sockaddr *) &clientBuf.address, clientBuf.length);
            Erase(table, &clientBuf);
        } else if (buf.Code == SETUSERNAME) {
            strcpy(Get(table, &clientBuf)->NickName, buf.Data);
            buf.Code = OK;
            strcpy(buf.Message, "Server : Set username successful !");
            strcpy(buf.Data, "");
            sendto(serverFileDescriptor, &buf, sizeof(struct CommonData), 0,
                   (struct sockaddr *) &clientBuf.address, clientBuf.length);
        } else if (buf.Code == CHAT) {
            sprintf(buf.Message, "From %hhu.%hhu.%hhu.%hhu:%d Name:%s", *((char *) (&clientBuf.address.sin_addr.s_addr) + 0),
                    *((char *) (&clientBuf.address.sin_addr.s_addr) + 1),
                    *((char *) (&clientBuf.address.sin_addr.s_addr) + 2),
                    *((char *) (&clientBuf.address.sin_addr.s_addr) + 3),
                    clientBuf.address.sin_port,
                    Get(table, &clientBuf)->NickName);
            for (int i = 0; i < MAXPORTNUM; i++) {
                if (table->clients[i] != NULL) {
                    clientBuf = *table->clients[i];
                    if (time(NULL) - clientBuf.time > TIMEOUT) {
                        Erase(table, table->clients[i]);
                        continue;
                    }
                    sendto(serverFileDescriptor, &buf, sizeof(struct CommonData), 0,
                           (struct sockaddr *) &clientBuf.address, clientBuf.length);
                }
            }
        } else {
            buf.Code = UNKNOWN;
            strcpy(buf.Message, "unknown");
            strcpy(buf.Data, "");
            sendto(serverFileDescriptor, &buf, sizeof(struct CommonData), 0,
                   (struct sockaddr *) &clientBuf.address, clientBuf.length);
        }
    }
    Destroy(table);
    close(serverFileDescriptor);
    return 0;
}