#include <stdio.h>
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

short SERVER_PORT = 9999;
int TIMEOUT = 300;

int main(int argc, char *argv[]) {
//    struct User *user = malloc(sizeof(struct User));
//    memset(user,0,sizeof(struct User));
//    for(int i=0;i<5;i++){
//        user->id = 100*i;
//        strcpy(user->nickname,"sunwenli");
//        strcpy(user->username,"sunwenli");
//        strcpy(user->password,"zxc.cf.1213");
//        SetUserByPlace(user,i);
//    }
//    user->id=999;
//    InsertUserByPlace(user,2);
//    free(user);
////    printf("bool : %d\n",RemoveUserByPlace(4));
////    printf("bool : %d\n",RemoveUserByPlace(2));
//    for(int i=0,count =(int)GetUserCount();i<count;i++){
//        user = GetUserByPlace(i);
//        if(user != NULL){
//            printf("%d\n",user->id);
//            printf("%s\n",user->username);
//            printf("%s\n",user->nickname);
//            printf("%s\n",user->password);
//            free(user);
//        }
//    }


//    return 0;
    for (int i = 0; i < argc; i++) {
        puts(argv[i]);
    }
    puts("====================================");
    unsigned int groupNumber = 1024;
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
    ConnectionTable tables[groupNumber];
    memset(tables, 0, sizeof(ConnectionTable) * groupNumber);
    for (unsigned int i = 0; i < 1024; i++) {
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
        if (buf.group >= groupNumber) {
            puts("");
            goto PRINT;
        }
        ConnectionTable table = tables[buf.group];
        struct Client *client = TableGet(table, &clientBuf);
        if (client == NULL) {
            if (buf.code == CONNECT) {
                strcpy(clientBuf.user.nickname, "Unnamed");
                clientBuf.time = time(NULL);
                clientBuf.status = UnLoggedIN;
                if (!TableSet(table, &clientBuf)) {
                    buf.code = ERROR;
                    strcpy(buf.message, "Server : This group is full");
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
                if (buf.code == CHAT) {
                    char temp[16];
                    client->status == LoggedIN ? strcpy(temp, "Logged in") : strcpy(temp, "Not logged in");
                    sprintf(buf.message, "Status : %s | NickName:%s", temp, client->user.nickname);
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
                    goto PRINT;
                } else if (buf.code == RENAME) {
                    strcpy(TableGet(table, &clientBuf)->user.nickname, buf.data);
                    sprintf(buf.message, "Server : Set username (Name:%s) successfully", buf.data);
                } else if (buf.code == LOGIN) {
                    struct User *user = GetUserByUserName(buf.message);
                    if (user != NULL) {
                        if (strcmp(user->password, buf.data) == 0) {
                            client->status = LoggedIN;
                            strcpy(buf.message, "Server : Login successfully");
                        } else {
                            strcpy(buf.message, "Server : Wrong password");
                        }
                        free(user);
                    } else {
                        strcpy(buf.message, "Server : None username");
                    }
                } else if (buf.code == REGISTER) {
                    struct User *user = (struct User *)malloc(sizeof(struct User));
                    if (user != NULL) {
                        strcpy(user->username,buf.message);
                        strcpy(user->password,buf.data);
                        user->id = GetUserCount();
                        printf("id : %d\n",user->id);
                        if (SetUserByPlace(user,user->id)) {
                            strcpy(buf.message, "Server : Register successfully");
                        } else {
                            strcpy(buf.message, "Server : Register unsuccessfully");
                        }
                        free(user);
                    } else {
                        buf.code = ERROR;
                        strcpy(buf.message, "Server : ERROR");
                    }
                } else if (buf.code == DISCONNECT) {
                    TableErase(table, &clientBuf);
                    strcpy(buf.message, "Server : Disconnect successfully");
                } else {
                    buf.code = UNKNOWN;
                    strcpy(buf.message, "Unknown");
                }
            }
        }
        strcpy(buf.data, "");
        sendto(serverFileDescriptor, &buf, sizeof(struct CommonData), 0,
               (struct sockaddr *) &clientBuf.address, clientBuf.length);
        PRINT:
        printf("%hhu.", *(char *) (&clientBuf.address.sin_addr.s_addr));
        printf("%hhu.", *((char *) (&clientBuf.address.sin_addr.s_addr) + 1));
        printf("%hhu.", *((char *) (&clientBuf.address.sin_addr.s_addr) + 2));
        printf("%hhu:", *((char *) (&clientBuf.address.sin_addr.s_addr) + 3));
        printf("%d", clientBuf.address.sin_port);
        printf("\t Code : %d\tGroup : %d\t", buf.code, buf.group);
        printf("size: %d\n", table->size);
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
