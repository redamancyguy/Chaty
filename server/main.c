#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include "user.h"
#include "client.h"
#include "commondata.h"

int main(int argc, char *argv[]) {
    short SERVER_PORT = 9999;
    int TIMEOUT = 300;
    unsigned int groupNumber = 1024;
    unsigned int groupSize = 1024;
    for (int i = 0; i < argc; i++) {
        if(argv[i][0] == '-'){
            if(strncmp(argv[i]+1,"port",4) == 0){
                char *temp;
                SERVER_PORT = (short)strtol(argv[i]+5,&temp,10);
            }
            else if(strncmp(argv[i]+1,"groupSize",9) == 0){
                char *temp;
                groupSize = (unsigned int)strtol(argv[i]+10,&temp,10);
            }
            else if(strncmp(argv[i]+1,"groupNumber",11) == 0){
                char *temp;
                groupNumber = (unsigned int)strtol(argv[i]+12,&temp,10);
            }
            else if(strncmp(argv[i]+1,"timeOut",7) == 0){
                char *temp;
                TIMEOUT = (int)strtol(argv[i]+8,&temp,10);
            }
        }
        puts(argv[i]);
    }
    printf("serverPort : %d\n",SERVER_PORT);
    printf("groupSize : %d\n",groupSize);
    printf("groupNumber : %d\n",groupNumber);
    printf("TIMEOUT : %d\n",TIMEOUT);
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
    ConnectionTable tables[groupNumber];
    memset(tables, 0, sizeof(ConnectionTable) * groupNumber);
    for (unsigned int i = 0; i < groupNumber; i++) {
        tables[i] = TableNew(groupSize);
        if (tables[i] == NULL) {
            puts("Create table failed");
            for (unsigned int j = 0; j < i; j++) {
                TableDestroy(tables[j]);
            }
            close(serverFileDescriptor);
            return -3;
        }
    }
    puts("Create Group table successfully");
    printf("The size of each group is : %d\n", groupSize);
    printf("There are %d groups\n", groupNumber);
    puts("Turn on successfully");
    struct DataBuf {
        enum StatusCode code;
        unsigned int group;
        char message[64];
        char data[1024];
        char others[1024];
    };
    struct Client clientBuf;
    struct User userBuf;
    struct DataBuf dataBuf;
    clientBuf.length = sizeof(clientBuf.address);
    while (true) {
        long int count = recvfrom(serverFileDescriptor, &dataBuf, sizeof(struct DataBuf), 0,
                                  (struct sockaddr *) &clientBuf.address, &clientBuf.length);
        if (count == -1) {
            puts("Receive data fail");
            break;
        } else if (count != sizeof(struct CommonData)) {
            continue;
        }
        if (dataBuf.group >= groupNumber) {
            strcpy(dataBuf.message,"Server : Wrong group");
            sendto(serverFileDescriptor, &dataBuf, sizeof(struct CommonData), 0,
                   (struct sockaddr *) &clientBuf.address, clientBuf.length);
            goto PRINT;
        }
        ConnectionTable table = tables[dataBuf.group];
        struct Client *client = TableGet(table, &clientBuf);
        if (client == NULL) {
            if (dataBuf.code == CONNECT) {
                strcpy(clientBuf.nickname, "Unnamed");
                clientBuf.time = time(NULL);
                clientBuf.status = UnLoggedIN;
                if (!TableSet(table, &clientBuf)) {
                    dataBuf.code = ERROR;
                    strcpy(dataBuf.message, "Server : This group is full");
                } else {
                    strcpy(dataBuf.message, "Server : Connect successfully");
                }
            } else {
                strcpy(dataBuf.message, "Server : You haven't connected yet");
            }

        } else {
            if (time(NULL) - client->time > TIMEOUT) {
                strcpy(dataBuf.message, "Server : Time out");
                TableErase(table, client);
            } else {
                client->time = time(NULL);
                if (dataBuf.code == CHAT) {
                    if (client->status == LoggedIN) {
                        sprintf(dataBuf.message, "Status : %s | NickName:%s", "Logged in", client->nickname);
                    } else {
                        sprintf(dataBuf.message, "Status : %s | NickName:%s", "Not logged in", client->nickname);
                    }
                    for (int i = 0; i < table->capacity; i++) {
                        if (table->clients[i] != NULL) {
                            if (time(NULL) - table->clients[i]->time > TIMEOUT) {
                                free(table->clients[i]);
                                table->clients[i] = NULL;
                                table->size--;
                                continue;
                            }
                            sendto(serverFileDescriptor, &dataBuf, sizeof(struct CommonData), 0,
                                   (struct sockaddr *) &table->clients[i]->address, table->clients[i]->length);
                        }
                    }
                    goto PRINT;
                } else if (dataBuf.code == RENAME) {
                    strcpy(TableGet(table, &clientBuf)->nickname, dataBuf.data);
                    sprintf(dataBuf.message, "Server : Set username (Name:%s) successfully", dataBuf.data);
                } else if (dataBuf.code == DISCONNECT) {
                    TableErase(table, &clientBuf);
                    strcpy(dataBuf.message, "Server : Disconnect successfully");
                } else if (dataBuf.code == LOGIN) {
                    if (GetUserByUserName(&userBuf, dataBuf.message) != -1) {
                        if (strcmp(userBuf.password, dataBuf.data) == 0) {
                            client->status = LoggedIN;
                            strcpy(dataBuf.message, "Server : Login successfully");
                        } else {
                            strcpy(dataBuf.message, "Server : Wrong password");
                        }
                    } else {
                        strcpy(dataBuf.message, "Server : None username");
                    }
                } else if (dataBuf.code == LOGOUT) {
                    client->status = UnLoggedIN;
                    strcpy(dataBuf.message, "Server : Logout successfully");
                } else if (dataBuf.code == REGISTER) {
                    strcpy(userBuf.username, dataBuf.message);
                    strcpy(userBuf.password, dataBuf.data);
                    if (GetUserByUserName(&userBuf, dataBuf.message) == -1) {
                        if (SetUserByPlace(&userBuf, GetUserCount())) {
                            strcpy(dataBuf.message, "Server : Register successfully");
                        } else {
                            strcpy(dataBuf.message, "Server : Register unsuccessfully");
                        }
                    } else {
                        strcpy(dataBuf.message, "Server : Duplicate username");
                    }
                } else if (dataBuf.code == UNREGISTER) {
                    long temp = GetUserByUserName(&userBuf, dataBuf.message);
                    if (temp == -1) {
                        strcpy(dataBuf.message, "Server : None username");
                    } else {
                        if (strcmp(userBuf.password, dataBuf.data) == 0) {
                            if (RemoveUserByPlace(temp) != -1) {
                                strcpy(dataBuf.message, "Server : Unregister successfully");
                            } else {
                                strcpy(dataBuf.message, "Server : Unregister unsuccessfully");
                            }
                        } else {
                            strcpy(dataBuf.message, "Server : Wrong password");
                        }
                    }
                } else {
                    dataBuf.code = UNKNOWN;
                    strcpy(dataBuf.message, "Unknown");
                }
            }
        }
        strcpy(dataBuf.data, "");
        sendto(serverFileDescriptor, &dataBuf, sizeof(struct CommonData), 0,
               (struct sockaddr *) &clientBuf.address, clientBuf.length);
        PRINT:
        printf("%hhu.", *(char *) (&clientBuf.address.sin_addr.s_addr));
        printf("%hhu.", *((char *) (&clientBuf.address.sin_addr.s_addr) + 1));
        printf("%hhu.", *((char *) (&clientBuf.address.sin_addr.s_addr) + 2));
        printf("%hhu:", *((char *) (&clientBuf.address.sin_addr.s_addr) + 3));
        printf("%d", clientBuf.address.sin_port);
        printf("\t Code : %d\tGroup : %d\t", dataBuf.code, dataBuf.group);
        printf("size: %d\n", dataBuf.group<groupNumber?table->size:-1);
    }
    puts("Shutdown server successfully");
    for (unsigned int i = 0; i < groupNumber; i++) {
        TableDestroy(tables[i]);
    }
    puts("Delete table successfully");
    close(serverFileDescriptor);
    puts("Close socket successfully");
    return 0;
}
