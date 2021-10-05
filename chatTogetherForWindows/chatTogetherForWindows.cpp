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
    UNKNOWN = -2,
    ERROR_ = -1,
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
int client_fd;
socklen_t len = sizeof(struct sockaddr_in);
struct sockaddr_in src;
void receive() {
    struct CommonData buff;
    while (1) {
        recvfrom(client_fd, (char*)&buff, sizeof(struct CommonData), 0, (struct sockaddr*)&src, &len);  //接收来自server的信息
        puts(buff.Message);
        puts(buff.Data);
    }
}

int main(int argc, char* argv[])

{


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



    ser_addr.sin_addr.s_addr = inet_addr("39.104.209.232");  //注意网络序转换
    ser_addr.sin_port = htons(SERVER_PORT);  //注意网络序转换


    buf.Code = CONNECT;
    strcpy(buf.Message, "connect");
    sendto(client_fd, (char*)&buf, sizeof(struct CommonData), 0, (struct sockaddr*)&ser_addr, len);
    //pthread_t pid;
    //pthread_create(&pid, NULL, (void* (*)(void*)) receive, NULL);

    //_beginthreade(receive,0,NULL);
    unsigned dwChildId;
    _beginthreadex(NULL, 0, (_beginthreadex_proc_type)&receive, NULL, 0, &dwChildId);
    while (1)
    {
        strcpy(buf.Message, "");
        strcpy(buf.Data, "");
        scanf("%[^\n]*?", buf.Data);
        buf.Code = CHAT;
        if (strcmp(buf.Data, "exit") == 0) {
            break;
        }
        if (strcmp(buf.Data, "set username") == 0) {
            buf.Code = SETUSERNAME;
            puts("input your username(except blank !)");
            scanf("%s", buf.Data);
            sendto(client_fd, (char*)&buf, sizeof(struct CommonData), 0, (struct sockaddr*)&ser_addr, len);
            continue;
        }
        sendto(client_fd, (char*)&buf, sizeof(struct CommonData), 0, (struct sockaddr*)&ser_addr, len);
        getchar();
    }
    buf.Code = DISCONNECT;
    strcpy(buf.Message, "disconnect");
    sendto(client_fd, (char*)&buf, sizeof(struct CommonData), 0, (struct sockaddr*)&ser_addr, len);
    closesocket(client_fd);
    return 0;
}