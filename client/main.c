#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include "commondata.h"
#include "include/TreeMap.h"
short SERVER_PORT = 9999;

int client_fd;


void receive() {
    struct sockaddr_in src;
    socklen_t len = sizeof(struct sockaddr_in);
    int i=0;
    while (1) {
//        printf("%d\n",i++);
        struct CommonData buff;
        recvfrom(client_fd, &buff, sizeof(struct CommonData), 0, (struct sockaddr *) &src, &len);
        time_t now;
        struct tm *nowTime;
        time(&now);
        nowTime = localtime(&now);
        char time[32];
        strcpy(time, asctime(nowTime));
        time[strlen(time) - 1] = '\0';
        printf("time : %s | ", time);
        printf("Group : %d  |  ", buff.group);
        puts(buff.message);
        puts(buff.data);
        if(buff.code == EXIT){
            break;
        }
    }
}

void Logout(struct CommonData buf, struct sockaddr *serverAddress) {
    socklen_t len = sizeof(struct sockaddr_in);
    buf.code = LOGOUT;
    sendto(client_fd, &buf, sizeof(struct CommonData), 0, serverAddress, len);
}

void Login(struct CommonData buf, struct sockaddr *serverAddress) {
    puts("input your username");
    scanf("%s", buf.message);
    puts("input your password");
    scanf("%s", buf.data);
    socklen_t len = sizeof(struct sockaddr_in);
    buf.code = LOGIN;
    sendto(client_fd, &buf, sizeof(struct CommonData), 0, serverAddress, len);
}

void Unregister(struct CommonData buf, struct sockaddr *serverAddress) {
    puts("input your username");
    scanf("%s", buf.message);
    puts("input your password");
    scanf("%s", buf.data);
    socklen_t len = sizeof(struct sockaddr_in);
    buf.code = UNREGISTER;
    sendto(client_fd, &buf, sizeof(struct CommonData), 0, serverAddress, len);
}

void Register(struct CommonData buf, struct sockaddr *serverAddress) {
    puts("input your username");
    scanf("%s", buf.message);
    puts("input your password");
    scanf("%s", buf.data);
    socklen_t len = sizeof(struct sockaddr_in);
    buf.code = REGISTER;
    sendto(client_fd, &buf, sizeof(struct CommonData), 0, serverAddress, len);
}

void Connect(struct CommonData buf, struct sockaddr *serverAddress) {
    socklen_t len = sizeof(struct sockaddr_in);
    buf.code = CONNECT;
    strcpy(buf.message, "connect");
    sendto(client_fd, &buf, sizeof(struct CommonData), 0, serverAddress, len);
}

void Disconnect(struct CommonData buf, struct sockaddr *serverAddress) {
    socklen_t len = sizeof(struct sockaddr_in);
    buf.code = DISCONNECT;
    strcpy(buf.message, "disconnect");
    sendto(client_fd, &buf, sizeof(struct CommonData), 0, serverAddress, len);
}

void SetGroup(struct CommonData buf, struct sockaddr *serverAddress, unsigned group) {
    buf.group = group;
    Connect(buf, serverAddress);
}

void Chat(struct CommonData buf, struct sockaddr *serverAddress) {
    socklen_t len = sizeof(struct sockaddr_in);
    buf.code = CHAT;
    sendto(client_fd, &buf, sizeof(struct CommonData), 0, serverAddress, len);
}

void SetNickName(struct CommonData buf, struct sockaddr *serverAddress) {
    socklen_t len = sizeof(struct sockaddr_in);
    buf.code = RENAME;
    sendto(client_fd, &buf, sizeof(struct CommonData), 0, serverAddress, len);
}
void FlushStdin(){
    int ch;
    while ((ch = getchar())) {
        if (ch == '\n') {
            break;
        }
    }
}
int main(int argc, char *argv[]) {
    unsigned int groupNumber = 1024;
    for (int i = 0; i < argc; i++) {
        puts(argv[i]);
    }
    puts("====================================");
    struct CommonData buf;
    const char *p = "Connect again       :reconnect\n"
                    "Disconnect to server:disconnect\n"
                    "Change nickname     :set username yourNickname\n"
                    "Change group        :set group groupNUm(from 0-1023)\n"
                    "Login by account    :login\n"
                    "Logout              :logout\n"
                    "Register a account  :register\n"
                    "Unregister a account:unregister\n"
                    "Exit the program    :exit\n";
    struct sockaddr_in serverAddress;
    client_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_fd < 0) {
        printf("create socket fail!\n");
        return -1;
    }

    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("0.0.0.0");
//    serverAddress.sin_addr.s_addr = inet_addr("39.104.209.232");
    serverAddress.sin_port = htons(SERVER_PORT);
    puts("input your group number (0 ~ 1023)");
    while (1) {
        if (scanf("%u", &buf.group) > 0) {
            break;
        }
        FlushStdin();
        puts("Please input a num !");
    }
    FlushStdin();
    pthread_t pid;
    pthread_create(&pid, NULL, (void *(*)(void *)) receive, NULL);
    Connect(buf, (struct sockaddr *) &serverAddress);
    TreeMap set = Tree_New();
    Tree_Insert(set,(void*)buf.group,(void*)time(NULL));
    while (1) {
        strcpy(buf.message, "");
        strcpy(buf.data, "");
        if (scanf("%[^\n]*?", buf.data) < 0) { break; }
        if (strcmp(buf.data, "exit") == 0) {
            Disconnect(buf, (struct sockaddr *) &serverAddress);
            break;
        } else if (strcmp(buf.data, "help") == 0) {
            puts(p);
        } else if (strncmp(buf.data, "set nickname", 12) == 0) {
            strcpy(buf.data, buf.data + 12);
            SetNickName(buf, (struct sockaddr *) &serverAddress);
        } else if (strcmp(buf.data, "login") == 0) {
            Login(buf, (struct sockaddr *) &serverAddress);
        } else if (strcmp(buf.data, "logout") == 0) {
            Logout(buf, (struct sockaddr *) &serverAddress);
        } else if (strcmp(buf.data, "register") == 0) {
            Register(buf, (struct sockaddr *) &serverAddress);
        } else if (strcmp(buf.data, "unregister") == 0) {
            Unregister(buf, (struct sockaddr *) &serverAddress);
        } else if (strncmp(buf.data, "set group", 9) == 0) {
            char *temp;
            strcpy(buf.data, buf.data + 9);
            unsigned int tempGroup = buf.group = (unsigned int) strtol(buf.data, &temp, 10);
            SetGroup(buf, (struct sockaddr *) &serverAddress, tempGroup);
            buf.group = tempGroup;
            Tree_SetOrInsert(set,(void*)buf.group,(void*)time(NULL));
        } else if (strcmp(buf.data, "disconnect") == 0) {
            Disconnect(buf, (struct sockaddr *) &serverAddress);
        } else if (strcmp(buf.data, "connect") == 0) {
            Connect(buf, (struct sockaddr *) &serverAddress);
        } else {
            Chat(buf, (struct sockaddr *) &serverAddress);
        }
        getchar();
    }
    for(int i=0;i<groupNumber;i++){
        if(Tree_ContainKey(set,(void*)i)){
            if(time(NULL) - (long)Tree_Get(set,(void*)i) < 300){
                buf.group = i;
                Disconnect(buf, (struct sockaddr *) &serverAddress);
                printf("Disconnect group : %d\n",i);
            }
        }
    }
    Tree_Destroy(set);
    close(client_fd);
    return 0;
}



