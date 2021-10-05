#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define SERVER_PORT 9999
#define BUFF_LEN 512
#define SERVER_IP "172.0.5.182"

enum PrivilegesCode{
    SUPERUSER = 1,
    USER = 2,
    ANONYMOUS = 3
};
struct Client{
    socklen_t length;
    struct sockaddr_in address;
    enum PrivilegesCode privileges;
    long long time;
    char NickName[20];
};

enum StatusCode {
    ERROR = -2,
    DISCONNECT = -1,
    UNKNOWN = 0,
    CONNECT = 1,
    CHAT = 2,
    RENAME = 3,
};
struct CommonData{
    enum StatusCode Code;
    char Message[64];
    char Data[1024];
};
int client_fd;
socklen_t len = sizeof(struct sockaddr_in);
struct sockaddr_in src;
struct CommonData buf;
void receive(){
    while(1){
        recvfrom(client_fd, &buf, sizeof(struct CommonData), 0, (struct sockaddr*)&src, &len);  //接收来自server的信息
        puts(buf.Message);
        puts(buf.Data);
        strcpy(buf.Message,"");
        strcpy(buf.Data,"");
    }
}
int main(int argc, char* argv[])
{

    struct sockaddr_in ser_addr;

    client_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(client_fd < 0)
    {
        printf("create socket fail!\n");
        return -1;
    }

    memset(&ser_addr, 0, sizeof(ser_addr));
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_addr.s_addr = inet_addr("0.0.0.0");  //注意网络序转换
//    ser_addr.sin_addr.s_addr = inet_addr("39.104.209.232");  //注意网络序转换
    ser_addr.sin_port = htons(SERVER_PORT);  //注意网络序转换


    buf.Code = CONNECT;
    strcpy(buf.Message,"connect");
    sendto(client_fd, &buf, sizeof(struct CommonData), 0, (struct sockaddr*)&ser_addr, len);
    pthread_t pid;
    pthread_create(&pid, NULL, (void *(*)(void *)) receive, NULL);
    while(1)
    {
        strcpy(buf.Message,"");
        strcpy(buf.Data,"");
        scanf("%[^\n]*?",buf.Data);
        if(strcmp(buf.Data,"exit") == 0){
            break;
        }
        if(strcmp(buf.Data,"set username") == 0){
            buf.Code = RENAME;
            puts("input your username(except blank !)");
            scanf("%s",buf.Data);
            sendto(client_fd, &buf, sizeof(struct CommonData), 0, (struct sockaddr*)&ser_addr, len);
            continue;
        }
        if(strcmp(buf.Data,"reconnect") == 0){
            buf.Code = CONNECT;
            sendto(client_fd, &buf, sizeof(struct CommonData), 0, (struct sockaddr*)&ser_addr, len);
            continue;
        }
        buf.Code = CHAT;
        sendto(client_fd, &buf, sizeof(struct CommonData), 0, (struct sockaddr*)&ser_addr, len);
        getchar();
    }
    buf.Code = DISCONNECT;
    strcpy(buf.Message,"disconnect");
    sendto(client_fd, &buf, sizeof(struct CommonData), 0, (struct sockaddr*)&ser_addr, len);
    close(client_fd);
    return 0;
}