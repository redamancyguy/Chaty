#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

#include <semaphore.h>
pthread_mutex_t mutex;
void receive() {
    while (1) {

        usleep(1000000);
        pthread_mutex_lock(&mutex);//上锁,
        puts("son");
        pthread_mutex_unlock(&mutex);
    }
}

int main() {
    if(pthread_mutex_init(&mutex,NULL))
    {
        printf("初始化用于线程间同步的信号量失败\n");
    }
    pthread_t pid;
    pthread_create(&pid, NULL, (void *(*)(void *)) receive, NULL);
    while (1) {
        usleep(1000000);
        pthread_mutex_lock(&mutex);
        puts("father");
        usleep(1000000);
        pthread_mutex_unlock(&mutex);
    }
    if(pthread_mutex_destroy(&mutex)) {
        printf("销毁互斥锁失败\n");
    }
        printf("Hello, World!\n");
    return 0;
}
