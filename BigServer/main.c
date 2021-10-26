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

unsigned int backboneThreadNumber = 4;
int k = 10;
short serverPORT = 9999;

struct BackboneTran {
    struct BufQueue *queue;
    Queue Queue;
};

int serverFileDescriptor;

pthread_mutex_t databaseMutex;

struct Clients *AllClients;

_Noreturn void *Clear(void *pointer) {
    const int TimOut = 5;
    const int Pace = 5;
    while (true) {
        sleep(Pace);
        for (int i = 0; i < 65536; i++) {
            struct Hash_Iterator it = NewHash_Iterator(AllClients->clients[i]);
            HashLock(AllClients->clients[i]);
            while (true) {
                struct Client *p = NextHash_Iterator(&it);
                if(p==NULL){
                    break;
                }
                if (time(NULL) - p->time > TimOut) {
                    HashErase(AllClients->clients[i], (void*)(unsigned long long)p->address.sin_addr.s_addr);
                    free(p);
                }
            }
            HashUnlock(AllClients->clients[i]);
        }
    }
}

_Noreturn void *Handle(struct BackboneTran *tran) {
    Queue Queue = tran->Queue;
    while (true) {
        if (TryLockQueue(Queue) == 0) {
            struct Message messages[1000];
            int length = 0;
            for (int i = 0; i < 1000 && !IsEmptyQueue(Queue); i++) {
                struct Message *temp = (struct Message *) FrontQueue(Queue);
                messages[length++] = *temp;
                free(temp);
                PopQueue(Queue);
            }
            UnlockQueue(Queue);
            for (int i = 0; i < length; i++) {
                struct Message message = messages[i];
                switch (message.data.code) {
                    case TOUCH: {
                        struct Client *client = ClientGet(AllClients, message.address);
                        if (client != NULL) {
                            client->time = time(NULL);
                            strcpy(message.data.data, "Server : YES");
                        } else {
                            strcpy(message.data.data, "Server : NO");
                        }
                        break;
                    }
                    case LOGIN: {
                        struct User userBuf;
                        memcpy(&userBuf, &message.data.data, sizeof(struct User));
                        long place = GetUserPlaceByUsername(userBuf.username);
                        if (place == -1) {
                            strcpy(message.data.data, "Server : None username");
                        } else {
                            struct User temp;
                            GetUserByPlace(&temp, place);
                            if (strcmp(temp.password, userBuf.password) == 0) {
                                struct Client *client = (struct Client *) malloc(sizeof(struct Client));
                                client->user = userBuf;
                                client->address = message.address;
                                client->length = message.length;
                                client->time = time(NULL);
                                HashLock(AllClients->clients[i]);
                                if (ClientsInsert(AllClients, message.address, client)) {
                                    strcpy(message.data.data, "Server : Login successfully");
                                } else {
                                    free(client);
                                    strcpy(message.data.data, "Server : You're already logged in");
                                }
                                HashUnlock(AllClients->clients[i]);
                            } else {
                                strcpy(message.data.data, "Server : Wrong password");
                            }
                        }
                        break;
                    }
                    case LOGOUT: {
                        struct Client *client = ClientGet(AllClients, message.address);
                        if (client != NULL) {
                            HashLock(AllClients->clients[i]);
                            ClientErase(AllClients, message.address);
                            free(client);
                            HashUnlock(AllClients->clients[i]);
                            strcpy(message.data.data, "Server : Logout successfully");
                        } else {
                            strcpy(message.data.data, "Server : You haven't logged in yet");
                        }
                        break;
                    }
                    case REGISTER: {
                        struct User userBuf;
                        memcpy(&userBuf, message.data.data, sizeof(struct User));
                        if (GetUserPlaceByUsername(userBuf.username) == -1) {
                            pthread_mutex_lock(&databaseMutex);
                            long place = GetUserReadyPlaceByUsername(userBuf.username);
                            userBuf.id = place;
                            if (InsertUserByPlace(&userBuf, place) != -1) {
                                strcpy(message.data.data, "Server : Register successfully");
                            } else {
                                strcpy(message.data.data, "Server : Register by error");
                            }
                            pthread_mutex_unlock(&databaseMutex);
                        } else {
                            strcpy(message.data.data, "Server : Duplicate username");
                        }
                        break;
                    }
                    case UNREGISTER: {
                        struct User userBuf;
                        memcpy(&userBuf, message.data.data, sizeof(struct User));
                        pthread_mutex_lock(&databaseMutex);
                        long place = GetUserPlaceByUsername(userBuf.username);
                        if (place == -1) {
                            strcpy(message.data.data, "Server : None username");
                        } else {
                            struct User userTemp;
                            GetUserByPlace(&userTemp, place);
                            if (strcmp(userBuf.password, userTemp.password) == 0) {
                                if (RemoveUserByPlace(place) == place) {
                                    strcpy(message.data.data, "Server : Unregister successfully");
                                } else {
                                    strcpy(message.data.data, "Server : Unregister by error");
                                }
                            } else {
                                strcpy(message.data.data, "Server : Wrong password");
                            }
                        }
                        pthread_mutex_unlock(&databaseMutex);
                        break;
                    }
                    case CHANGE: {
                        break;
                    }
                    case CHAT: {
                        break;
                    }
                    default: {
                        strcpy(message.data.data, "Server : unknown");
                        break;
                    }
                }
                sendto(serverFileDescriptor, &message.data, sizeof(struct CommunicationData), 0,
                       (struct sockaddr *) &message.address, message.length);
            }
        } else {
            usleep(1000);
        }
    }
}

_Noreturn void *Convert(struct BackboneTran *tran) {
    struct BufQueue *queue = tran->queue;
    Queue Queue = tran->Queue;
    while (true) {
        LockQueue(Queue);
        if (!BufQueueIsEmpty(queue)) {
            if (BufQueueTryLock(queue) == 0) {
                for (int i = 0; i < 1000 && !BufQueueIsEmpty(queue); i++) {
                    struct Message *temp = (struct Message *) malloc(sizeof(struct Message));
                    *temp = BufQueueFront(queue)->message;
                    if (!PushQueue(Queue, (void *) temp)) {
                        free(temp);
                        break;
                    }
                    BufQueuePop(queue);
                }
                BufQueueUnlock(queue);
            } else {
                usleep(1000);
            }
        } else {
            usleep(1000);
        }
        UnlockQueue(Queue);
    }
}

_Noreturn void *GetMessage(struct BackboneTran *tran) {
    struct BufQueue *queue = tran->queue;
    while (true) {
        struct DataBuf *temp = BufQueueBack(queue);
        long long count = recvfrom(serverFileDescriptor, &temp->message.data, sizeof(struct DataBuf),
                                   0, (struct sockaddr *) &temp->message.address, &temp->message.length);
        switch (count) {
            case -1: {
                perror("Receive data fail");
                break;
            }
            case sizeof(struct CommunicationData): {
                BufQueueLock(queue);
                while (BufQueueIsFull(queue)) {
                    BufQueueUnlock(queue);
                    usleep(1000);
                    BufQueueLock(queue);
                }
                BufQueuePush(queue);
                BufQueueUnlock(queue);
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
        pthread_t clearThread;
        if (pthread_create(&clearThread, NULL, (void *(*)(void *)) Clear, NULL) != 0) {
            perror("create ClearThread failed");
            exit(-1);
        }

        getchar();
        getchar();
        getchar();

        pthread_cancel(clearThread);
        if (pthread_join(clearThread, NULL) != 0) {
            perror("join ClearThread failed");
            exit(-1);
        }

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