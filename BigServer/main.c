#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <pthread.h>
//#include "DataStructure/Queue.h"
#include "DataStructure/Hash.h"
#include "Database/User.h"
#include "packageFrom/communication.h"
#include "packageFrom/message.h"
#include "Client/Client.h"
#include "packageFrom/messageQueue.h"

unsigned int TIMEOUT = 3000;
unsigned int groupSize = 1024;

unsigned int backboneThreadNumber = 24;
short serverPORT = 9999;

struct BackboneTran {
    pthread_mutex_t *mutex;
    struct MessageQueue *queue;
};

int serverFileDescriptor;


pthread_mutex_t databaseMutex;

_Noreturn void *GetMessage(struct BackboneTran *tran) {
    struct DataBuf {
        struct Message message;
        char others[1024];
    };
    pthread_mutex_t *mutex = tran->mutex;
    struct MessageQueue *queue = tran->queue;
    while (true) {
        struct DataBuf dataBuf;
        long long count = recvfrom(serverFileDescriptor, &dataBuf.message.data, sizeof(struct CommunicationData)+1024, 0,
                                  (struct sockaddr *) &dataBuf.message.address, &dataBuf.message.len);
        switch (count) {
            case -1: {
                perror("Receive data fail");
                break;
            }
            case sizeof(struct CommunicationData): {
                pthread_mutex_lock(mutex);
                if(!MessageQueueIsFull(queue)){
                    MessageQueuePush(queue,&dataBuf.message);
                }
                pthread_mutex_unlock(mutex);
                break;
            }
            default:
                perror("Invalid data package");
                break;
        }
    }
}

int main() {
    for (int ii = 0; ii < 1; ii++) {
//        if(ii%100 == 0)
//        printf("%d\n",ii);
        serverFileDescriptor = socket(AF_INET, SOCK_DGRAM, 0);
        if (serverFileDescriptor < 0) {
            perror("Create socket fail!");
            return -1;
        }
        struct sockaddr_in serverAddress;
        memset(&serverAddress, 0, sizeof(struct sockaddr_in));
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
        serverAddress.sin_port = htons(serverPORT);
        if (bind(serverFileDescriptor, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
            perror("Socket bind fail");
            close(serverFileDescriptor);
            return -2;
        }
        if (pthread_mutex_init(&databaseMutex, NULL)) {
            perror("mutex init failed");
            return -1;
        }

        struct BackboneTran *backboneTrans = (struct BackboneTran *) malloc(sizeof(struct BackboneTran) * backboneThreadNumber);
        pthread_t *GetThreads = (pthread_t *) malloc(sizeof(pthread_t) * backboneThreadNumber);
        for (int i = 0; i < backboneThreadNumber; i++) {
            pthread_attr_t attr = {0};
            pthread_attr_init(&attr);
            struct sched_param sched = {99};
            pthread_attr_setschedpolicy(&attr, SCHED_RR);
            pthread_attr_setschedparam(&attr, &sched);
            if ((backboneTrans[i].queue = MessageQueueNew(1024)) == NULL) {
                perror("create queue failed");
                exit(-1);
            }
            if ((backboneTrans[i].mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t))) == NULL) {
                perror("create mutex failed");
                exit(-1);
            }
            if (pthread_mutex_init(backboneTrans[i].mutex, NULL) != 0) {
                perror("Init mutex failed");
                exit(-1);
            }
            if (pthread_create(&GetThreads[i], NULL, (void *(*)(void *)) GetMessage, &backboneTrans[i]) != 0) {
                perror("create thread failed");
                exit(-1);
            }
        }









//        getchar();
//        getchar();
//        getchar();
        SHUTDOWN:
        for (int i = 0; i < backboneThreadNumber; i++) {
            pthread_cancel(GetThreads[i]);
            if (pthread_join(GetThreads[i], NULL) != 0) {
                perror("detach thread failed");
                exit(-1);
            }
            if (pthread_mutex_destroy(backboneTrans[i].mutex) != 0) {
                perror("Destroy mutex failed");
            }
            free(backboneTrans[i].mutex);
            MessageQueueDestroy(backboneTrans[i].queue);
        }
        free(GetThreads);
        free(backboneTrans);
        usleep(100);
        close(serverFileDescriptor);
    }
    return 0;
}