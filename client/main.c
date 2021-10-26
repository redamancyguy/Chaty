#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include "../server-mul/user.h"
#include "../server-mul/commondata.h"
#include "include/TreeMap.h"
short SERVER_PORT = 9999+0;

int client_fd;


void receive() {
    struct sockaddr_in src;
    socklen_t len = sizeof(struct sockaddr_in);
    int i=0;
    while (1) {
        printf("%d\n",i++);
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
//        puts(buff.message);  printf("Group : %d  |  ", buff.group);

        puts(buff.data);
    }
}

void Logout(struct CommonData buf, struct sockaddr *serverAddress) {
    socklen_t len = sizeof(struct sockaddr_in);
    buf.code = LOGOUT;
    sendto(client_fd, &buf, sizeof(struct CommonData), 0, serverAddress, len);
}

void Login(struct CommonData buf, struct sockaddr *serverAddress) {
    struct User user;
    memset(&user,0,sizeof(struct User));
    puts("input your username");
    scanf("%s", user.username);
    puts("input your password");
    scanf("%s", user.password);
    socklen_t len = sizeof(struct sockaddr_in);
    puts(user.username);
    puts(user.password);
    buf.code = LOGIN;
    memcpy(buf.data,&user,sizeof(struct User));
    sendto(client_fd, &buf, sizeof(struct CommonData), 0, serverAddress, len);
}

void Unregister(struct CommonData buf, struct sockaddr *serverAddress) {
    struct User user;
    memset(&user,0,sizeof(struct User));
    puts("input your username");
    scanf("%s", user.username);
    puts("input your password");
    scanf("%s", user.password);
    socklen_t len = sizeof(struct sockaddr_in);
    buf.code = UNREGISTER;
    memcpy(buf.data,&user,sizeof(struct User));
    sendto(client_fd, &buf, sizeof(struct CommonData), 0, serverAddress, len);
}

long long unsigned *randUnsignedLong(long long unsigned int length) { // remember to free the array
    long long unsigned *a = (long long unsigned *) malloc(sizeof(long long unsigned) * length);
    if (a == NULL) {
        return NULL;
    }
    for (long long unsigned i = 0; i < length; i++) {
        a[i] = i;
    }
    srand((unsigned int) time(NULL));
    long long unsigned temp;
    while (--length) {
        temp = rand() % (length + 1);
        long long unsigned t = a[length];
        a[length] = a[temp];
        a[temp] = t;
    }
    return a;
}
void RegisterTest(struct sockaddr *serverAddress) {
    struct CommonData buf;
    unsigned long *a = randUnsignedLong(1024);
    for(int i=0;i<100/2;i++){
        struct User user;
        memset(&user,0,sizeof(struct User));
        sprintf(user.username,"sunwenli%c%c",'a'+(a[i]%26),'a'+(a[i+1]%26));
        user.id = i;
        socklen_t len = sizeof(struct sockaddr_in);
        buf.code = REGISTER;
//        buf.group = 123;
        memcpy(buf.data,&user,sizeof(struct User));
        sendto(client_fd, &buf, sizeof(struct CommonData), 0, serverAddress, len);
        usleep(100000);
    }
    free(a);
}
void Register(struct CommonData buf, struct sockaddr *serverAddress) {
    struct User user;
    memset(&user,0,sizeof(struct User));
    puts("input your ID");
    scanf("%ld", &user.id);
    puts("input your email");
    scanf("%s", user.email);
    puts("input your username");
    scanf("%s", user.username);
    puts("input your password");
    scanf("%s", user.password);
    socklen_t len = sizeof(struct sockaddr_in);
    buf.code = REGISTER;
    memcpy(buf.data,&user,sizeof(struct User));
    sendto(client_fd, &buf, sizeof(struct CommonData), 0, serverAddress, len);
}


void Connect(struct CommonData buf, struct sockaddr *serverAddress) {
    socklen_t len = sizeof(struct sockaddr_in);
    memset(&buf,0,sizeof(struct CommonData));
    buf.code = CONNECT;
//    strcpy(buf.message, "connect");
    printf("connect %d\n",buf.code);
    sendto(client_fd, &buf, sizeof(struct CommonData), 0, serverAddress, len);
}

void Disconnect(struct CommonData buf, struct sockaddr *serverAddress) {
    socklen_t len = sizeof(struct sockaddr_in);
    buf.code = DISCONNECT;
//    strcpy(buf.message, "disconnect");
    sendto(client_fd, &buf, sizeof(struct CommonData), 0, serverAddress, len);
}

void SetGroup(struct CommonData buf, struct sockaddr *serverAddress, unsigned group) {
//    buf.group = group;
    Connect(buf, serverAddress);
}

void Chat(struct CommonData buf, struct sockaddr *serverAddress) {
    socklen_t len = sizeof(struct sockaddr_in);
    puts("INPUT");
    puts(buf.data);
    sendto(client_fd, &buf, sizeof(struct CommonData), 0, serverAddress, len);
}

void Email(struct CommonData buf, struct sockaddr *serverAddress) {
    socklen_t len = sizeof(struct sockaddr_in);
    puts("Input your email");
    scanf("%s",buf.data);
    buf.code = EMAIL;
    sendto(client_fd, &buf, sizeof(struct CommonData), 0, serverAddress, len);
}
void Change(struct CommonData buf, struct sockaddr *serverAddress) {
    socklen_t len = sizeof(struct sockaddr_in);
    puts("Input your username");
    scanf("%s",buf.data);
    puts("Input your code");
    scanf("%s",buf.data+20);
    puts("Input your new password");
    scanf("%s",buf.data+40);
    buf.code = CHANGE;
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
    struct CommonData buf;
    FlushStdin();
    pthread_t pid;
    pthread_create(&pid, NULL, (void *(*)(void *)) receive, NULL);
    Connect(buf, (struct sockaddr *) &serverAddress);
    TreeMap set = Tree_New();
//    RegisterTest(&serverAddress);
//    Email(buf,&serverAddress);
//    Change(buf,&serverAddress);
int i=0;
    while (1) {
        if(i > 1000){
            scanf("%s",buf.data);
        }
        printf("iii ; %d\n",i++);
        buf.code = i;
        Chat(buf, (struct sockaddr *) &serverAddress);
//        usleep(1);
        struct timespec pp;
        pp.tv_nsec = 100;
        pp.tv_sec = 0;
        nanosleep(&pp,&pp);
        continue;
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
//            unsigned int tempGroup = buf.group = (unsigned int) strtol(buf.data, &temp, 10);
//            SetGroup(buf, (struct sockaddr *) &serverAddress, tempGroup);
//            buf.group = tempGroup;
//            Tree_SetOrInsert(set,(void*)buf.group,(void*)time(NULL));
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
//                buf.group = i;
                Disconnect(buf, (struct sockaddr *) &serverAddress);
                printf("Disconnect group : %d\n",i);
            }
        }
    }
    Tree_Destroy(set);
    close(client_fd);
    return 0;
}



