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
    ERROR = -1,
    DISCONNECT = 0,
    CONNECT = 1,
    CHAT = 2,
    SETUSERNAME = 3
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
            table->Size++;
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
            table->Size++;
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
                table->Size++;
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
                table->Size++;
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
    Client_ client;
    struct CommonData buf;
    while (true) {
        client.length = sizeof(client.address);
        long int count = recvfrom(serverFileDescriptor, &buf, sizeof(struct CommonData), 0,
                                  (struct sockaddr *) &client.address, &client.length);  //recvfrom是拥塞函数，没有数据就一直拥塞
        if (count == -1) {
            printf("receive data fail!\n");
            return 0;
        }
        if (Get(table, &client) == NULL) {
            strcpy(client.NickName,"Unnamed");
            Set(table, &client);
            Get(table, &client)->time = time(NULL);
            strcpy(buf.Message,"Server : Connected !");
            sendto(serverFileDescriptor, &buf, sizeof(struct CommonData), 0,
                   (struct sockaddr *) &client.address, client.length);
            continue;
        }
        printf("%d.", *(char *) (&client.address.sin_addr.s_addr));
        printf("%d.", *((char *) (&client.address.sin_addr.s_addr) + 1));
        printf("%d.", *((char *) (&client.address.sin_addr.s_addr) + 2));
        printf("%d:", *((char *) (&client.address.sin_addr.s_addr) + 3));
        printf("%d---->", client.address.sin_port);
        printf(":%s\n", buf.Message);
        printf("size : %d\n", table->Size);
        if (buf.Code == DISCONNECT) {
            Erase(table, &client);
            continue;
        }
        if (buf.Code == SETUSERNAME) {
            strcpy(Get(table, &client)->NickName, buf.Data);
            continue;
        }
        if (buf.Code == CHAT) {
            sprintf(buf.Message,"From %d.%d.%d.%d:%d Name:%s", *((char *) (&client.address.sin_addr.s_addr) + 0),
                    *((char *) (&client.address.sin_addr.s_addr) + 1),
                    *((char *) (&client.address.sin_addr.s_addr) + 2),
                    *((char *) (&client.address.sin_addr.s_addr) + 3),
                    client.address.sin_port,
                    Get(table, &client)->NickName);
            for (int i = 0; i < MAXPORTNUM; i++) {
                if (table->clients[i] != NULL) {
                    client = *table->clients[i];
                    if (time(NULL) - client.time > TIMEOUT) {
                        Erase(table, table->clients[i]);
                        continue;
                    }
                    sendto(serverFileDescriptor, &buf, sizeof(struct CommonData), 0,
                           (struct sockaddr *) &client.address, client.length);
                }
            }
            strcpy(buf.Message, "");
            strcpy(buf.Data, "");
        }
    }
    Destroy(table);
    close(serverFileDescriptor);
    return 0;
}