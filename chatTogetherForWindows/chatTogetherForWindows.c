#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS


#include <stdio.h>
#include <sys/types.h>
#include <winsock2.h>
#include<WS2tcpip.h>
#include <string.h>
#include <windows.h>
#include <time.h>
#include <process.h>
#pragma comment(lib,"ws2_32.lib")
#define SERVER_PORT 9999
#define BUFF_LEN 512
#define SERVER_IP "172.0.5.182"
#include <minwindef.h>
enum PrivilegesCode {
    SUPERUSER = 1,
    USER = 2,
    ANONYMOUS = 3
};
struct Client {
    socklen_t length;
    struct sockaddr_in address;
    enum PrivilegesCode privileges;
    long long time;
    char NickName[20];
};

enum StatusCode {
    ERROR__ = -3,
    SHUTDOWN = -2,
    DISCONNECT = -1,
    UNKNOWN = 0,
    CONNECT = 1,
    CHAT = 2,
    RENAME = 3,
};
struct CommonData {
    unsigned int group;
    enum StatusCode Code;
    char Message[64];
    char Data[1024];
};
int client_fd;
socklen_t len = sizeof(struct sockaddr_in);
struct sockaddr_in src;
void receive() {
    struct CommonData buff;
    while (1) {
        struct CommonData buf;
        memset(&buf, 0, sizeof(struct CommonData));
        if (recvfrom(client_fd, (char*)&buf, sizeof(struct CommonData), 0, (struct sockaddr*)&src, &len) == -1) {
            continue;
            Sleep(100);
        }
        printf("Group : %d  |  ", buf.group);
        puts(buf.Message);
        puts(buf.Data);
    }
}

int main(int argc, char* argv[])

{
    const char* p = "Connect again       :reconnect\n"
                    "Disconnect to server:disconnect\n"
                    "Change nickname     :set username yourNickname\n"
                    "Change group        :set group groupNUm(from 0-1023)\n"
                    "Exit the program    :exit\n";

    //初始化WSA
    WORD sockVersion = MAKEWORD(2, 2);
    WSADATA wsaData;
    if (WSAStartup(sockVersion, &wsaData) != 0)
    {
        return 0;
    }
    struct CommonData buf;
    struct sockaddr_in ser_addr;

    client_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_fd < 0)
    {
        printf("create socket fail!\n");
        return -1;
    }

    memset(&ser_addr, 0, sizeof(ser_addr));
    ser_addr.sin_family = AF_INET;
    //ser_addr.sin_addr.s_addr = inet_addr("0.0.0.0");  //注意网络序转换
    ser_addr.sin_addr.s_addr = inet_addr("172.27.213.226");  //注意网络序转换
   // ser_addr.sin_addr.s_addr = inet_addr("39.104.209.232");  //注意网络序转换
    ser_addr.sin_port = htons(SERVER_PORT);  //注意网络序转换
    unsigned dwChildId;
    _beginthreadex(NULL, 0, (_beginthreadex_proc_type)&receive, NULL, 0, &dwChildId);
    buf.Code = CONNECT;
    puts("input your group number (0 ~ 1023)");
    while (1) {
        scanf("%u", &buf.group);
        if (buf.group < 1024) {
            break;
        }
        puts("Wrong group number");
    }
    printf("group : %d\n", buf.group);
    strcpy(buf.Message, "connect");
    sendto(client_fd, (char*) & buf, sizeof(struct CommonData), 0, (struct sockaddr*)&ser_addr, len);
    while (1) {
        strcpy(buf.Message, "");
        strcpy(buf.Data, "");
        scanf("%[^\n]*?", buf.Data);
        if (strcmp(buf.Data, "exit") == 0) {
            break;
        }
        else if (strcmp(buf.Data, "help") == 0) {
            puts(p);
        }
        else if (strncmp(buf.Data, "set username", 12) == 0) {
            buf.Code = RENAME;
            strcpy(buf.Data, buf.Data + 12);
        }
        else if (strncmp(buf.Data, "set group", 9) == 0) {
            strcpy(buf.Data, buf.Data + 9);
            if ((unsigned int)atoi(buf.Data) > 1023) {
                puts("Wrong group number");
                continue;
            }
            buf.group = (unsigned int)atoi(buf.Data);
        }
        else if (strcmp(buf.Data, "disconnect") == 0) {
            buf.Code = DISCONNECT;
        }
        else if (strcmp(buf.Data, "reconnect") == 0) {
            buf.Code = CONNECT;
        }
        else {
            buf.Code = CHAT;
        }
        sendto(client_fd, (char*) & buf, sizeof(struct CommonData), 0, (struct sockaddr*)&ser_addr, len);
        getchar();
    }
    buf.Code = DISCONNECT;
    strcpy(buf.Message, "disconnect");
    sendto(client_fd, (char*)&buf, sizeof(struct CommonData), 0, (struct sockaddr*)&ser_addr, len);
    closesocket(client_fd);
    return 0;
}