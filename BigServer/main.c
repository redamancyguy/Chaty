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
#include "DataStructure/Hash.h"
#include "Database/User.h"
#include "packageFrom/communication.h"
#include "packageFrom/message.h"
#include "Client/Client.h"
#include "packageFrom/BufQueue.h"
#include "DataStructure/Queue.h"
#include "Clients.h"

unsigned int TIMEOUT = 3000;
unsigned int groupSize = 1024;

unsigned int backboneThreadNumber = 4;
int k = 20;
short serverPORT = 9999;

struct BackboneTran {
    pthread_mutex_t mutex;
    struct BufQueue *queue;
    pthread_mutex_t Mutex;
    Queue Queue;
};

int serverFileDescriptor;

pthread_mutex_t databaseMutex;

int iii = 0;
int iiii = 0;
int iiiii = 0;
struct Clients *AllClients;

_Noreturn void *Handle(struct BackboneTran *tran) {
//    pthread_mutex_t *Mutex = tran->Mutex;
//    Queue Queue = tran->Queue;
//    while (true) {
//        if (pthread_mutex_trylock(Mutex) == 0) {
//            struct Message messages[10];
//            int length = 0;
//            for (int i = 0; i < 10 && !IsEmptyQueue(Queue); i++) {
//                messages[length++] = *(struct Message *) FrontQueue(Queue);
//                PopQueue(Queue);
//            }
//            pthread_mutex_unlock(Mutex);
//            for (int i = 0; i < length; i++) {
////                printf("iii : %d\n", iii++);
//                switch (messages[i].data.code) {
////                    case LOGIN:
////                    case REGISTER:{  //before login
////                        struct User user;
////                        memcpy(&user,message.data.data,sizeof(struct User));
////                        if(GetUserPlaceByUsername(user.username) == -1){
////
////                        }else{
////
////                        }
////                        break;
////                    }
////                    case CHANGE:
////                    case LOGOUT:
////                    case UNREGISTER:{ //after login
////                        break;
////                    }
////                    case CHAT: {
////                        break;
////                    }
//                    default: {
////                        puts("unknown message");
//                        break;
//                    }
//                }
//            }
//        } else {
//            usleep(1000);
//        }
//    }
}

_Noreturn void *Convert(struct BackboneTran *tran) {
    pthread_mutex_t *mutex = &tran->mutex;
    struct BufQueue *queue = tran->queue;
    pthread_mutex_t *Mutex = &tran->Mutex;
    Queue Queue = tran->Queue;
    while (true) {
        pthread_mutex_lock(mutex);
        if (!BufQueueIsEmpty(queue)) {
            printf("%ld iiii : %d\n", pthread_self(), iiii++);
            BufQueuePop(queue);
//            if (pthread_mutex_trylock(mutex) == 0) {
//                do {
//                    printf("%ld iiii : %d\n",pthread_self(), iiii++);
//                    BufQueuePop(queue);
//                } while (!BufQueueIsEmpty(queue));
//                pthread_mutex_unlock(mutex);
//            }else{
//                usleep(1000);
//            }
            pthread_mutex_unlock(mutex);
        } else {
            pthread_mutex_unlock(mutex);
            usleep(1000);
        }
//        pthread_mutex_lock(Mutex);
//        if (!BufQueueIsEmpty(queue)) {
//            if (pthread_mutex_trylock(mutex) == 0) {
//                do {
//                    struct Message *temp = (struct Message *) malloc(sizeof(struct Message));
//                    *temp = BufQueueFront(queue)->message;
//                    if (!PushQueue(Queue, (void *) temp)) {
//                        free(temp);
//                        break;
//                    }
//                    printf("%ld iiii : %d\n",pthread_self(), iiii++);
//                    BufQueuePop(queue);
//                } while (!BufQueueIsEmpty(queue));
//                pthread_mutex_unlock(mutex);
//            }else{
//                usleep(1000);
//            }
//        } else {
//            usleep(1000);
//        }
//        pthread_mutex_unlock(Mutex);
    }
}

_Noreturn void *GetMessage(struct BackboneTran *tran) {
    pthread_mutex_t *mutex = &tran->mutex;
    struct BufQueue *queue = tran->queue;
    while (true) {
        struct DataBuf *temp = BufQueueBack(queue);
        long long count = recvfrom(serverFileDescriptor, &temp->message.data, sizeof(struct DataBuf),
                                   0, (struct sockaddr *) &temp->message.address, &temp->message.len);
        switch (count) {
            case -1: {
                perror("Receive data fail");
                break;
            }
            case sizeof(struct CommunicationData): {
                pthread_mutex_lock(mutex);
                while (BufQueueIsFull(queue)) {
                    pthread_mutex_unlock(mutex);
                    usleep(1000);
                    pthread_mutex_lock(mutex);
                }
                BufQueuePush(queue);
                printf("%ld iiiii : %d\n", pthread_self(), iiiii++);
                pthread_mutex_unlock(mutex);
                break;
            }
            default: {
                puts("Invalid data package");
                break;
            }
        }
    }
}

int main() {
    for (int ii = 0; ii < 1000; ii++, usleep(100)) {
        printf("%d\n", ii);
        serverFileDescriptor = socket(AF_INET, SOCK_DGRAM, 0);
        if (serverFileDescriptor < 0) {
            perror("Create socket fail!");
            exit(-1);
        }
        struct sockaddr_in serverAddress;
        memset(&serverAddress, 0, sizeof(struct sockaddr_in));
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
        serverAddress.sin_port = htons(serverPORT);
        if (bind(serverFileDescriptor, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
            perror("Socket bind fail");
            close(serverFileDescriptor);
            exit(-1);
        }
        if (pthread_mutex_init(&databaseMutex, NULL)) {
            perror("mutex init failed");
            exit(-1);
        }
        AllClients = ClientsNew();
        struct BackboneTran *backboneTrans = (struct BackboneTran *)
                malloc(sizeof(struct BackboneTran) * backboneThreadNumber);
        pthread_t *GetThreads = (pthread_t *) malloc(sizeof(pthread_t) * backboneThreadNumber);
        pthread_t *ConcertThreads = (pthread_t *) malloc(sizeof(pthread_t) * backboneThreadNumber);
        pthread_t *HandleThreads = (pthread_t *) malloc(sizeof(pthread_t) * backboneThreadNumber * k);
        for (int i = 0; i < backboneThreadNumber; i++) {
            if ((backboneTrans[i].queue = BufQueueNew(1024 * 32)) == NULL) {
                perror("create queue failed");
                exit(-1);
            }
            if ((backboneTrans[i].Queue = NewQueue()) == NULL) {
                perror("create linkQueue failed");
                exit(-1);
            }
            if (pthread_mutex_init(&backboneTrans[i].mutex, NULL) != 0) {
                perror("Init mutex failed");
                exit(-1);
            }
            if (pthread_mutex_init(&backboneTrans[i].Mutex, NULL) != 0) {
                perror("Init linkQueueMutex failed");
                exit(-1);
            }
            pthread_attr_t attr;
            pthread_attr_init(&attr);
            struct sched_param sched = {99};
            pthread_attr_setschedpolicy(&attr, SCHED_RR);
            pthread_attr_setschedparam(&attr, &sched);
            if (pthread_create(&GetThreads[i], &attr, (void *(*)(void *)) GetMessage, &backboneTrans[i]) != 0) {
                perror("create GetThread failed");
                exit(-1);
            }
            sched.sched_priority = 98;
            pthread_attr_setschedpolicy(&attr, SCHED_RR);
            pthread_attr_setschedparam(&attr, &sched);
            if (pthread_create(&ConcertThreads[i], &attr, (void *(*)(void *)) Convert, &backboneTrans[i]) != 0) {
                perror("create ConvertThread failed");
                exit(-1);
            }
            for (int j = 0; j < k; j++) {
                if (pthread_create(&HandleThreads[i * k + j], NULL,
                                   (void *(*)(void *)) Handle, &backboneTrans[i]) != 0) {
                    perror("create HandleThread failed");
                    exit(-1);
                }
            }
        }


        getchar();
        getchar();
        getchar();


        for (int i = 0; i < backboneThreadNumber; i++) {
            pthread_cancel(ConcertThreads[i]);
            if (pthread_join(ConcertThreads[i], NULL) != 0) {
                perror("detach thread failed");
                exit(-1);
            }
            pthread_cancel(GetThreads[i]);
            if (pthread_join(GetThreads[i], NULL) != 0) {
                perror("detach thread failed");
                exit(-1);
            }
            for (int j = 0; j < k; j++) {
                pthread_cancel(HandleThreads[i * k + j]);
                if (pthread_join(HandleThreads[i * k + j], NULL) != 0) {
                    perror("detach thread failed");
                    exit(-1);
                }
            }
            pthread_mutex_destroy(&backboneTrans[i].mutex);
            pthread_mutex_destroy(&backboneTrans[i].Mutex);
            BufQueueDestroy(backboneTrans[i].queue);
            DestroyQueue(backboneTrans[i].Queue);
        }
        if (pthread_mutex_destroy(&databaseMutex)) {
            perror("mutex delete failed");
            exit(-1);
        }
        free(GetThreads);
        free(backboneTrans);
        ClientsDestroy(AllClients);
        close(serverFileDescriptor);
    }
    return 0;
}