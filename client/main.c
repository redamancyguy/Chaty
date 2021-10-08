#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

#define SERVER_PORT 9999

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
    enum StatusCode Code;
    unsigned int group;
    char Message[64];
    char Data[1024];
};
int client_fd;


void receive() {
    struct sockaddr_in src;
    socklen_t len = sizeof(struct sockaddr_in);
    while (1) {
        struct CommonData buff;
        recvfrom(client_fd, &buff, sizeof(struct CommonData), 0, (struct sockaddr *) &src, &len);  //接收来自server的信息
        printf("Group : %d  |  ", buff.group);
        puts(buff.Message);
        puts(buff.Data);
    }
}

void Connect(struct CommonData buf, struct sockaddr *ser_addr) {
    socklen_t len = sizeof(struct sockaddr_in);
    buf.Code = CONNECT;
    strcpy(buf.Message, "connect");
    sendto(client_fd, &buf, sizeof(struct CommonData), 0, ser_addr, len);
}

void Disconnect(struct CommonData buf, struct sockaddr *ser_addr) {
    socklen_t len = sizeof(struct sockaddr_in);
    buf.Code = DISCONNECT;
    strcpy(buf.Message, "disconnect");
    sendto(client_fd, &buf, sizeof(struct CommonData), 0, ser_addr, len);
}

void SetGroup(struct CommonData buf, struct sockaddr *ser_addr, unsigned group) {
    Disconnect(buf, ser_addr);
    buf.group = group;
    Connect(buf, ser_addr);
}

void Chat(struct CommonData buf, struct sockaddr *ser_addr) {
    socklen_t len = sizeof(struct sockaddr_in);
    buf.Code = CHAT;
    sendto(client_fd, &buf, sizeof(struct CommonData), 0, ser_addr, len);
}

void SetNickName(struct CommonData buf, struct sockaddr *ser_addr) {
    socklen_t len = sizeof(struct sockaddr_in);
    buf.Code = RENAME;
    sendto(client_fd, &buf, sizeof(struct CommonData), 0, ser_addr, len);
}

int main(int argc, char *argv[]) {
    struct CommonData buf;
    const char *p = "Connect again       :reconnect\n"
                    "Disconnect to server:disconnect\n"
                    "Change nickname     :set username yourNickname\n"
                    "Change group        :set group groupNUm(from 0-1023)\n"
                    "Exit the program    :exit\n";
    struct sockaddr_in ser_addr;
    client_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_fd < 0) {
        printf("create socket fail!\n");
        return -1;
    }

    memset(&ser_addr, 0, sizeof(ser_addr));
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_addr.s_addr = inet_addr("0.0.0.0");  //注意网络序转换
//    ser_addr.sin_addr.s_addr = inet_addr("39.104.209.232");  //注意网络序转换
    ser_addr.sin_port = htons(SERVER_PORT);  //注意网络序转换
    puts("input your group number (0 ~ 1023)");
    while (1) {
        if (scanf("%u", &buf.group) > 0) {
            if (buf.group > 1023) {
                puts("Wrong group number");
                continue;
            }
            break;
        }
        puts("Please input a num !");
        int ch;
        while ((ch = getchar())) {
            if (ch == '\n') {
                break;
            }
        }
    }

    pthread_t pid;
    pthread_create(&pid, NULL, (void *(*)(void *)) receive, NULL);
    Connect(buf, (struct sockaddr *) &ser_addr);

    while (1) {
        strcpy(buf.Message, "");
        strcpy(buf.Data, "");
        scanf("%[^\n]*?", buf.Data);
        if (strcmp(buf.Data, "exit") == 0) {
            Disconnect(buf, (struct sockaddr *) &ser_addr);
            break;
        } else if (strcmp(buf.Data, "help") == 0) {
            puts(p);
        } else if (strncmp(buf.Data, "set nickname", 12) == 0) {
            strcpy(buf.Data, buf.Data + 12);
            SetNickName(buf, (struct sockaddr *) &ser_addr);
        } else if (strncmp(buf.Data, "set group", 9) == 0) {
            strcpy(buf.Data, buf.Data + 9);
            if ((unsigned int) atoi(buf.Data) > 1023) {
                puts("Wrong group number");
                continue;
            }
            SetGroup(buf, (struct sockaddr *) &ser_addr, (unsigned int) atoi(buf.Data));
            buf.group = (unsigned int) atoi(buf.Data);
        } else if (strcmp(buf.Data, "disconnect") == 0) {
            Disconnect(buf, (struct sockaddr *) &ser_addr);
        } else if (strcmp(buf.Data, "connect") == 0) {
            Connect(buf, (struct sockaddr *) &ser_addr);
        } else {
            Chat(buf, (struct sockaddr *) &ser_addr);
        }
        getchar();
    }
    Disconnect(buf, (struct sockaddr *) &ser_addr);
    close(client_fd);
    return 0;
}



