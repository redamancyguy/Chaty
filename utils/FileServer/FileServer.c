//
// Created by sunwenli on 2021/11/1.
//


#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include "FileServer.h"

void *sendToClient(void *pointer) {
    int clientFD = (int) (long long) pointer;
    long long fileSize = 0;
    char fileName[128] = {0};
    char buffer[64 * 1024] = {0};
    read(clientFD, buffer, sizeof(long long) + sizeof(fileName)); //main info:(fileSize+fileName)
    strncpy(fileName, buffer + sizeof(long long), sizeof(fileName));
    sprintf(buffer, "files/%s", fileName);
    int file = open(buffer, O_RDWR);
    fileSize = lseek(file, 0, SEEK_END);
    memcpy(buffer, &fileSize, sizeof(long long));
    lseek(file, 0, SEEK_SET);
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        long long ret;
        if ((ret = read(file, buffer, sizeof(buffer))) <= 0) {
            break;
        }
        write(clientFD, buffer, ret);
    }
    close(file);
    close(clientFD);
    return NULL;
}

void *receiveFromClient(void *pointer) {
    int clientFD = (int) (long long) pointer;
    char fileSize[64] = {0};
    char fileName[128] = {0};
    char buffer[64 * 1024] = {0};
    read(clientFD, buffer, sizeof(fileSize) + sizeof(fileName)); //main info:(fileSize+fileName)
    memcpy(&fileSize, buffer, sizeof(fileSize));
    strncpy(fileName, buffer + sizeof(fileSize), sizeof(fileName));
    printf("%s\n", fileSize);
    printf("%s\n", fileName);
    sprintf(buffer, "files/%s", fileName);
    int file = open(buffer, O_RDWR | O_CREAT | O_TRUNC, 0666);
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        long long ret;
        if ((ret = read(clientFD, buffer, sizeof(buffer))) <= 0) {
            break;
        }
        write(file, buffer, ret);
    }
    close(file);
    close(clientFD);
    return NULL;
}

void fileServer() {
    short PORT = 10000 + 2;
    int tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_socket == -1) {
        perror("create socket file Description failed\n");
        return;
    }
    struct sockaddr_in local_address = {0};
    local_address.sin_family = AF_INET;
    local_address.sin_port = htons(PORT);
    local_address.sin_addr.s_addr = INADDR_ANY;//让系统自动检测本地网卡IP并绑定
    if (bind(tcp_socket, (struct sockaddr *) &local_address, sizeof(local_address)) == -1) {
        perror("bind  failed\n");
        return;
    }
    if (listen(tcp_socket, 5) == -1) {
        perror("listen is fail\n");
        return;
    }
    struct sockaddr_in client_address = {0};
    socklen_t len = sizeof(client_address);
    int new_socket;
    puts("file server !");
    while (1) {
        new_socket = accept(tcp_socket, (struct sockaddr *) &client_address, &len);
        if (new_socket < 0) {
            perror("accept error\n");
            continue;
        }
        printf("client connected [%s:%d]\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
        char flag[8];
        long long ret = read(new_socket, flag, sizeof(flag));
        printf("result %lld\n", ret);
        ret = read(new_socket, flag, sizeof(flag));
        printf("result %lld\n", ret);
        for (int i = 0; i < 8; i++) {
            printf("%c\n", flag[i]);
        }
        write(new_socket, "OK", 2);
        if (ret < 0) {
            perror("receive failed");
        }
        if (strcmp(flag, "true") == 0) {
            puts("YES");
            pthread_t thread;
            if (pthread_create(&thread, NULL, (void *(*)(void *)) receiveFromClient, (void *) (long long) new_socket) !=
                0) {
                exit(-7);
            }
        } else if (strcmp(flag, "false") == 0) {
            puts("NO");
            pthread_t thread;
            if (pthread_create(&thread, NULL, (void *(*)(void *)) sendToClient, (void *) (long long) new_socket) != 0) {
                exit(-7);
            }
        } else {
            puts("Unknown");
        }
        close(new_socket);
    }
    close(tcp_socket);
}
