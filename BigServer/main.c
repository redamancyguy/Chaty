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

unsigned int TIMEOUT = 3000;
unsigned int groupSize = 1024;

unsigned int backboneThreadNumber = 4;
short serverPORT = 9999;

struct BackboneTran {
    pthread_mutex_t *mutex;
    struct BufQueue *queue;
    pthread_mutex_t *Mutex;
    Queue linkQueue; //link to deal with
};

int serverFileDescriptor;

pthread_mutex_t databaseMutex;

int iii = 0;

Hash users[65536];
struct UserClient {
    struct Client client;
    struct Message message;
};

_Noreturn void *Handle(struct BackboneTran *tran) {
    pthread_mutex_t *Mutex = tran->Mutex;
    Queue Queue = tran->linkQueue;
    while (true) {
        if (pthread_mutex_trylock(Mutex) == 0) {
            struct Message messages[10];
            int length = 0;
            for (int i = 0; i < 10 && !IsEmptyQueue(Queue); i++) {
                printf("%d\n",iii++);
                messages[length++] = *(struct Message *) FrontQueue(Queue);
                PopQueue(Queue);
            }
            pthread_mutex_unlock(Mutex);
            for (int i = 0; i < length; i++) {
                switch (messages[i].data.code) {
//                    case LOGIN:
//                    case REGISTER:{  //before login
//                        struct User user;
//                        memcpy(&user,message.data.data,sizeof(struct User));
//                        if(GetUserPlaceByUsername(user.username) == -1){
//
//                        }else{
//
//                        }
//                        break;
//                    }
//                    case CHANGE:
//                    case LOGOUT:
//                    case UNREGISTER:{ //after login
//                        break;
//                    }
//                    case CHAT: {
//                        break;
//                    }
                    default:
//                        puts("unknown message");
                        break;
                }
            }
        } else {
            usleep(1000);
        }
    }
}

_Noreturn void *Convert(struct BackboneTran *tran) {
    pthread_mutex_t *mutex = tran->mutex;
    struct BufQueue *queue = tran->queue;
    pthread_mutex_t *Mutex = tran->Mutex;
    Queue Queue = tran->linkQueue;
    while (true) {
        pthread_mutex_lock(Mutex);
        if (!BufQueueIsEmpty(queue)) {
            if (pthread_mutex_trylock(mutex) == 0) {
                do {
                    struct Message *temp = (struct Message *) malloc(sizeof(struct Message));
                    *temp = BufQueueFront(queue)->message;
                    PushQueue(Queue, (void *) temp);
                    BufQueuePop(queue);
                } while (!BufQueueIsEmpty(queue));
                pthread_mutex_unlock(mutex);
            }
        } else {
            usleep(1000);
        }
        pthread_mutex_unlock(Mutex);
    }
}

_Noreturn void *GetMessage(struct BackboneTran *tran) {
    pthread_mutex_t *mutex = tran->mutex;
    struct BufQueue *queue = tran->queue;
    while (true) {
        while (BufQueueIsFull(queue)) {
            usleep(1000);
        }
        struct DataBuf *temp = BufQueueFront(queue);
        long long count = recvfrom(serverFileDescriptor, &temp->message.data, sizeof(struct CommunicationData) + 1024,
                                   0, (struct sockaddr *) &temp->message.address, &temp->message.len);
        switch (count) {
            case -1: {
                perror("Receive data fail");
                break;
            }
            case sizeof(struct CommunicationData): {
                pthread_mutex_lock(mutex);
                BufQueuePush(queue);
                pthread_mutex_unlock(mutex);
                break;
            }
            default:
                puts("Invalid data package");
                break;
        }
    }
}

int main() {
    // test bufqueue
//    struct BufQueue *qu = BufQueueNew(10);
//    for(int i=0;i<11;i++){
//        struct DataBuf temp;
//        temp.message.len = i;
//        if(!BufQueueIsFull(qu)){
//            *BufQueuePush(qu) = temp;
//        }else{
//            printf("full %d\n",i);
//        }
//    }
//
//    for(int i=0;i<5;i++){
//        printf("len : %d\n",BufQueuePop(qu)->message.len);
//    }
//
//
//    for(int i=0;i<11;i++){
//        struct DataBuf temp;
//        temp.message.len = i;
//        if(!BufQueueIsFull(qu)){
//            *BufQueuePush(qu) = temp;
//            printf("fullll %d\n",i);
//        }else{
//        }
//    }
//
//    while(!BufQueueIsEmpty(qu)){
//        printf("len : %d\n",BufQueueFront(qu)->message.len);
//        BufQueuePop(qu);
//    }
//    return 0;
    for (int ii = 0; ii < 1000; ii++, usleep(100)) {
        if (ii % 100 == 0)
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
        for (int i = 0; i < 65536; i++) {
            users[i] = HashNew(1024);
            if (users[i] == NULL) {
                perror("create hash failed");
                exit(-1);
            }
        }
        struct BackboneTran *backboneTrans = (struct BackboneTran *) malloc(
                sizeof(struct BackboneTran) * backboneThreadNumber);
        pthread_t *GetThreads = (pthread_t *) malloc(sizeof(pthread_t) * backboneThreadNumber);
        pthread_t *ConcertThreads = (pthread_t *) malloc(sizeof(pthread_t) * backboneThreadNumber);
        int k = 20;
        pthread_t *HandleThreads = (pthread_t *) malloc(sizeof(pthread_t) * backboneThreadNumber*k);
        for (int i = 0; i < backboneThreadNumber; i++) {
            if ((backboneTrans[i].queue = BufQueueNew(1024)) == NULL) {
                perror("create queue failed");
                exit(-1);
            }
            if ((backboneTrans[i].linkQueue = NewQueue()) == NULL) {
                perror("create linkQueue failed");
                exit(-1);
            }
            if ((backboneTrans[i].mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t))) == NULL) {
                perror("create mutex failed");
                exit(-1);
            }
            if ((backboneTrans[i].Mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t))) == NULL) {
                perror("create linkQueueMutex failed");
                exit(-1);
            }
            if (pthread_mutex_init(backboneTrans[i].mutex, NULL) != 0) {
                perror("Init mutex failed");
                exit(-1);
            }
            if (pthread_mutex_init(backboneTrans[i].Mutex, NULL) != 0) {
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
            for(int j=0;j<k;j++){
                if (pthread_create(&HandleThreads[i*k+j], NULL, (void *(*)(void *))Handle , &backboneTrans[i]) != 0) {
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
            pthread_mutex_destroy(backboneTrans[i].mutex);
            pthread_mutex_destroy(backboneTrans[i].Mutex);
            free(backboneTrans[i].mutex);
            free(backboneTrans[i].Mutex);
            BufQueueDestroy(backboneTrans[i].queue);
            DestroyQueue(backboneTrans[i].linkQueue);
        }
        if (pthread_mutex_destroy(&databaseMutex)) {
            perror("mutex delete failed");
            exit(-1);
        }
        free(GetThreads);
        free(backboneTrans);
        for (int i = 0; i < 65536; i++) {
            HashDestroy(users[i]);
        }
        close(serverFileDescriptor);
    }
    return 0;
}