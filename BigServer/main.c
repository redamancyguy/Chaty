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
#include "DataStructure/ArrayList.h"
#include "DataStructure/Hash.h"
#include "DataStructure/Queue.h"
#include "DataStructure/Stack.h"
#include "DataStructure/Tree.h"
#include "Database/User.h"
#include "relevant.h"
unsigned int MaxGroupNumber = 1024;
unsigned int BufQueueSize = 1024 * 32;
unsigned int backboneThreadNumber = 4;
int k = 10;
short serverPORT = 9999;

//struct BackboneTran {
//    struct BufQueue *queue;
//    Queue Queue;
//};
//
//int serverFileDescriptor;
//
//pthread_mutex_t databaseMutex;
//
//struct Clients *AllClients;  //自动上锁
//Hash AllGroup; //手动上锁
//
//_Noreturn void *Clear(void *pointer) {
//    const int TimOut = 5;
//    const int Pace = 1;
//    while (true) {
//        sleep(Pace);
//        HashLock(AllGroup);
//        ArrayList array = HashToArrayList(AllGroup);
//        HashUnlock(AllGroup);
//        for (unsigned long long i = 0, size = ArrayListSize(array); i < size; i++) {
//            ArrayList arrayTemp = ((ArrayList) ArrayListGet(array, i));
//            ArrayListLock(arrayTemp);
//            for (unsigned long long j = 0, sizeT = ArrayListSize(arrayTemp); j < sizeT; j++) {
//                if (time(NULL) - ((struct Client *) ArrayListGet(arrayTemp, j))->time > TimOut) {
//                    ArrayListErase(arrayTemp, j); // delete a user from a group
//                }
//            }
//            if (ArrayListSize(arrayTemp) == 0) {
//                HashLock(AllGroup);
//                HashErase(AllGroup, (void *) arrayTemp); //delete a group
//                HashUnlock(AllGroup);
//            }
//            ArrayListUnlock(arrayTemp);
//        }
//        ArrayListDestroy(array);
//        for (int i = 0; i < 65536; i++) {
//            struct Hash_Iterator it = NewHash_Iterator(AllClients->clients[i]);
//            HashLock(AllClients->clients[i]);
//            struct Client *p;
//            while ((p = NextHash_Iterator(&it)) != NULL) {
//                if (time(NULL) - p->time > TimOut) {
//                    HashErase(AllClients->clients[i], (void *) (unsigned long long) p->address.sin_addr.s_addr);
//                    free(p);//delete user and free the client
//                }
//            }
//            HashUnlock(AllClients->clients[i]);
//        }
//    }
//}
//
//int iii = 0;
//
//_Noreturn void *Handle(struct BackboneTran *tran) {
//    Queue Queue = tran->Queue;
//    unsigned int HandleBufSize = 1024;
//    unsigned sleepPace = 1024;
//    while (true) {
//        if (QueueTryLock(Queue) == 0) {
//            struct Message *messages[HandleBufSize];
//            int length = 0;
//            for (int i = 0; i < HandleBufSize && !QueueIsEmpty(Queue); i++) {
//                messages[length++] = (struct Message *) QueueFront(Queue);
//                QueuePop(Queue);
//            }
//            QueueUnlock(Queue);
//            for (int i = 0; i < length; i++) {
//                printf("%d\n", iii++);
//                printf("%ld\n", time(NULL));
//                struct Message message = *messages[i];
//                free(messages[i]);
//                switch (message.data.code) {
//                    case TOUCH: {
//                        struct Client *client = ClientGet(AllClients, message.address);
//                        if (client != NULL) {
//                            client->time = time(NULL);
//                            strcpy(message.data.data, "Server : YES");
//                        } else {
//                            strcpy(message.data.data, "Server : NO");
//                        }
//                        break;
//                    }
//                    case LOGIN: {
//                        struct User userBuf;
//                        memcpy(&userBuf, message.data.data + 64, sizeof(struct User));
//                        long place = GetUserPlaceByUsername(userBuf.username);
//                        if (place == -1) {
//                            strcpy(message.data.data, "Server : None username");
//                        } else {
//                            struct User temp;
//                            GetUserByPlace(&temp, place);
//                            if (strcmp(temp.password, userBuf.password) == 0) {
//                                struct Client *client = (struct Client *) malloc(sizeof(struct Client));
//                                if (ClientsInsert(AllClients, message.address, client)) {
//                                    client->user = userBuf;
//                                    client->address = message.address;
//                                    client->length = message.length;
//                                    client->time = time(NULL);
//                                    strcpy(message.data.data, "Server : Login successfully");
//                                } else {
//                                    free(client);
//                                    strcpy(message.data.data, "Server : You're already logged in");
//                                }
//                            } else {
//                                strcpy(message.data.data, "Server : Wrong password");
//                            }
//                        }
//                        break;
//                    }
//                    case LOGOUT: {
//                        struct Client *client = ClientGet(AllClients, message.address);
//                        if (client != NULL) {
//                            ClientErase(AllClients, message.address);
//                            free(client);
//                            strcpy(message.data.data, "Server : Logout successfully");
//                        } else {
//                            strcpy(message.data.data, "Server : You haven't logged in yet");
//                        }
//                        break;
//                    }
//                    case JOIN: {
//                        ArrayList array = HashGet(AllGroup, (*(void **) (message.data.data + 64)));
//                        struct Client *client = ClientGet(AllClients, message.address);
//                        if (array != NULL) {
//                            if (client != NULL) {
//                                if (ArrayListContain(array, (void *) client)) {
//                                    strcpy(message.data.data, "Server : You have already Joined");
//                                } else {
//                                    ArrayListLock(array);
//                                    ArrayListPushBack(array, (void *) client);
//                                    ArrayListUnlock(array);
//                                    strcpy(message.data.data, "Server : Join successfully");
//                                }
//                            } else {
//                                strcpy(message.data.data, "Server : You haven't logged in yet");
//                            }
//                        } else {
//                            strcpy(message.data.data, "Server : Wrong group id");
//                        }
//                        break;
//                    }
//                    case DETACH: {
//                        ArrayList array = HashGet(AllGroup, (*(void **) (message.data.data + 64)));
//                        if (array != NULL) {
//                            struct Client *client = ClientGet(AllClients, message.address);
//                            if (client != NULL) {
//                                for (unsigned long long ii = 0, size = ArrayListSize(array); ii < size; ii++) {
//                                    if (((struct Client *) ArrayListGet(array, ii)) == client) {
//                                        ArrayListLock(array);
//                                        ArrayListErase(array, ii);
//                                        ArrayListUnlock(array);
//                                        strcpy(message.data.data, "Server : Detach successfully");
//                                        goto SEND;
//                                    }
//                                }
//                                strcpy(message.data.data, "Server : You are not in this group");
//                            } else {
//                                strcpy(message.data.data, "Server : You haven't logged in yet");
//                            }
//                        } else {
//                            strcpy(message.data.data, "Server : Wrong group id");
//                        }
//                        break;
//                    }
//                    case REGISTER: {
//                        struct User userBuf;
//                        memcpy(&userBuf, message.data.data + 64, sizeof(struct User));
//                        long place = GetUserReadyPlaceByUsername(userBuf.username);
//                        if (place != -1) {
//                            pthread_mutex_lock(&databaseMutex);
//                            userBuf.id = place;
//                            if (InsertUserByPlace(&userBuf, place) != -1) {
//                                strcpy(message.data.data, "Server : Register successfully");
//                            } else {
//                                strcpy(message.data.data, "Server : Register by error");
//                            }
//                            pthread_mutex_unlock(&databaseMutex);
//                        } else {
//                            strcpy(message.data.data, "Server : Duplicate username");
//                        }
//                        break;
//                    }
//                    case UNREGISTER: {
//                        struct User userBuf;
//                        memcpy(&userBuf, message.data.data + 64, sizeof(struct User));
//                        pthread_mutex_lock(&databaseMutex);
//                        long place = GetUserPlaceByUsername(userBuf.username);
//                        if (place == -1) {
//                            strcpy(message.data.data, "Server : None username");
//                        } else {
//                            struct User userTemp;
//                            GetUserByPlace(&userTemp, place);
//                            if (strcmp(userBuf.password, userTemp.password) == 0) {
//                                if (RemoveUserByPlace(place) == place) {
//                                    strcpy(message.data.data, "Server : Unregister successfully");
//                                } else {
//                                    strcpy(message.data.data, "Server : Unregister by error : unknown");
//                                }
//                            } else {
//                                strcpy(message.data.data, "Server : Wrong password");
//                            }
//                        }
//                        pthread_mutex_unlock(&databaseMutex);
//                        break;
//                    }
//                    case NEWGROUP: {
//                        ArrayList group = ArrayListNew();
//                        if (group != NULL) {
//                            struct Client *client = ClientGet(AllClients, message.address);
//                            if (client != NULL) {
//                                *((unsigned long long *) (message.data.data + 64)) = (unsigned long long) group;
//                                HashLock(AllGroup);
//                                if (HashInsert(AllGroup, (void *) group, (void *) group)
//                                    && ArrayListPushBack(group, client)) {
//                                    strcpy(message.data.data, "Server : Create group successfully");
//                                } else {
//                                    ArrayListDestroy(group);
//                                    HashErase(AllGroup, (void *) group);
//                                    strcpy(message.data.data, "Server : Create group by error : hash insert error");
//                                }
//                                HashUnlock(AllGroup);
//                            } else {
//                                strcpy(message.data.data, "Server : You haven't logged in yet");
//                            }
//                        } else {
//                            strcpy(message.data.data, "Server : Create group by error");
//                        }
//                        break;
//                    }
//                    case CHAT: { // 注意这里的群组 要求用户主动在客户端退出,然后就无法再向这个群组发送信息了,如果主动检测,性能损耗过高
//                        puts(message.data.data + 128);
//                        ArrayList array = (ArrayList) HashGet(AllGroup, (*(void **) (message.data.data + 64)));
//                        if (array != NULL) {
//                            if (ArrayListContain(array, ClientGet(AllClients, message.address))) {
//                                strcpy(message.data.data, "Server : Chatting");
//                                for (unsigned long ii = 0, size = ArrayListSize(array); ii < size; ii++) {
//                                    struct Client *client = ArrayListGet(array, ii);
//                                    sendto(serverFileDescriptor, &message.data, sizeof(struct CommunicationData), 0,
//                                           (struct sockaddr *) &client->address, client->length);
//                                }
//                                continue;
//                            } else {
//                                strcpy(message.data.data, "Server : You are not in this group");
//                            }
//                        } else {
//                            strcpy(message.data.data, "Server : None groupId");
//                        }
//                        break;
//                    }
//                    default: {
//                        strcpy(message.data.data, "Server : Unknown");
//                        break;
//                    }
//                }
//                SEND:
//                sendto(serverFileDescriptor, &message.data, sizeof(struct CommunicationData), 0,
//                       (struct sockaddr *) &message.address, message.length);
//            }
//        } else {
//            usleep(sleepPace);
//        }
//    }
//}
//
//_Noreturn void *Convert(struct BackboneTran *tran) {
//    unsigned int ConvertMaximum = 1024;
//    unsigned int sleepPace = 1024;
//    struct BufQueue *queue = tran->queue;
//    Queue Queue = tran->Queue;
//    while (true) {
//        QueueLock(Queue);
//        if (!BufQueueIsEmpty(queue)) {
//            if (BufQueueTryLock(queue) == 0) {
//                for (unsigned int i = 0; i < ConvertMaximum && !BufQueueIsEmpty(queue); i++) {
//                    struct Message *temp = (struct Message *) malloc(sizeof(struct Message));
//                    *temp = BufQueueFront(queue)->message;
//                    if (!QueuePush(Queue, (void *) temp)) {
//                        free(temp);
//                        break;
//                    }
//                    BufQueuePop(queue);
//                }
//                BufQueueUnlock(queue);
//            } else {
//                usleep(sleepPace);
//            }
//        } else {
//            usleep(sleepPace);
//        }
//        QueueUnlock(Queue);
//    }
//}
//
//_Noreturn void *GetMessage(struct BackboneTran *tran) {
//    unsigned int sleepPace = 1024;
//    struct BufQueue *queue = tran->queue;
//    while (true) {
//        struct DataBuf *temp = BufQueueBack(queue);
//        long long count = recvfrom(serverFileDescriptor, &temp->message.data, sizeof(struct DataBuf),
//                                   0, (struct sockaddr *) &temp->message.address, &temp->message.length);
//        switch (count) {
//            case -1: {
//                perror("Receive data fail");
//                break;
//            }
//            case sizeof(struct CommunicationData): {
//                BufQueueLock(queue);
//                while (BufQueueIsFull(queue)) {
//                    BufQueueUnlock(queue);
//                    usleep(sleepPace);
//                    BufQueueLock(queue);
//                }
//                BufQueuePush(queue);
//                BufQueueUnlock(queue);
//                break;
//            }
//            default: {
//                puts("Invalid data package");
//                break;
//            }
//        }
//    }
//}

int main() {
    return 0;
//    for (int ii = 0; ii < 1000; ii++, usleep(100)) {
//        printf("%d\n", ii);
//        serverFileDescriptor = socket(AF_INET, SOCK_DGRAM, 0);
//        if (serverFileDescriptor < 0) {
//            perror("Create socket fail!");
//            exit(-1);
//        }
//        struct sockaddr_in serverAddress;
//        memset(&serverAddress, 0, sizeof(struct sockaddr_in));
//        serverAddress.sin_family = AF_INET;
//        serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
//        serverAddress.sin_port = htons(serverPORT);
//        if (bind(serverFileDescriptor, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
//            perror("Socket bind fail");
//            close(serverFileDescriptor);
//            exit(-1);
//        }
//        if (pthread_mutex_init(&databaseMutex, NULL)) {
//            perror("mutex init failed");
//            exit(-1);
//        }
//        AllClients = ClientsNew();
//        AllGroup = HashNew(MaxGroupNumber);
//        struct BackboneTran *backboneTrans = (struct BackboneTran *)
//                malloc(sizeof(struct BackboneTran) * backboneThreadNumber);
//        pthread_t *GetThreads = (pthread_t *) malloc(sizeof(pthread_t) * backboneThreadNumber);
//        pthread_t *ConcertThreads = (pthread_t *) malloc(sizeof(pthread_t) * backboneThreadNumber);
//        pthread_t *HandleThreads = (pthread_t *) malloc(sizeof(pthread_t) * backboneThreadNumber * k);
//        for (int i = 0; i < backboneThreadNumber; i++) {
//            if ((backboneTrans[i].queue = BufQueueNew(BufQueueSize)) == NULL) {
//                perror("create queue failed");
//                exit(-1);
//            }
//            if ((backboneTrans[i].Queue = QueueNew()) == NULL) {
//                perror("create linkQueue failed");
//                exit(-1);
//            }
//            pthread_attr_t attr;
//            pthread_attr_init(&attr);
//            struct sched_param sched = {99};
//            pthread_attr_setschedpolicy(&attr, SCHED_RR);
//            pthread_attr_setschedparam(&attr, &sched);
//            if (pthread_create(&GetThreads[i], &attr, (void *(*)(void *)) GetMessage, &backboneTrans[i]) != 0) {
//                perror("create GetThread failed");
//                exit(-1);
//            }
//            sched.sched_priority = 98;
//            pthread_attr_setschedpolicy(&attr, SCHED_RR);
//            pthread_attr_setschedparam(&attr, &sched);
//            if (pthread_create(&ConcertThreads[i], &attr, (void *(*)(void *)) Convert, &backboneTrans[i]) != 0) {
//                perror("create ConvertThread failed");
//                exit(-1);
//            }
//            for (int j = 0; j < k; j++) {
//                if (pthread_create(&HandleThreads[i * k + j], NULL,
//                                   (void *(*)(void *)) Handle, &backboneTrans[i]) != 0) {
//                    perror("create HandleThread failed");
//                    exit(-1);
//                }
//            }
//        }
//        pthread_t clearThread;
//        if (pthread_create(&clearThread, NULL, (void *(*)(void *)) Clear, NULL) != 0) {
//            perror("create ClearThread failed");
//            exit(-1);
//        }
//
//        getchar();
//        getchar();
//        getchar();
//
//        pthread_cancel(clearThread);
//        if (pthread_join(clearThread, NULL) != 0) {
//            perror("join ClearThread failed");
//            exit(-1);
//        }
//
//        for (int i = 0; i < backboneThreadNumber; i++) {
//            pthread_cancel(ConcertThreads[i]);
//            if (pthread_join(ConcertThreads[i], NULL) != 0) {
//                perror("detach thread failed");
//                exit(-1);
//            }
//            pthread_cancel(GetThreads[i]);
//            if (pthread_join(GetThreads[i], NULL) != 0) {
//                perror("detach thread failed");
//                exit(-1);
//            }
//            for (int j = 0; j < k; j++) {
//                pthread_cancel(HandleThreads[i * k + j]);
//                if (pthread_join(HandleThreads[i * k + j], NULL) != 0) {
//                    perror("detach thread failed");
//                    exit(-1);
//                }
//            }
//            BufQueueDestroy(backboneTrans[i].queue);
//            QueueDestroy(backboneTrans[i].Queue);
//        }
//        if (pthread_mutex_destroy(&databaseMutex)) {
//            perror("mutex delete failed");
//            exit(-1);
//        }
//        free(GetThreads);
//        free(backboneTrans);
//        HashLock(AllGroup);
//        ArrayList array = HashToArrayList(AllGroup);
//        HashUnlock(AllGroup);
//        for (unsigned long long i = 0; i < ArrayListSize(array); i++) {
//            ArrayListDestroy(((ArrayList) ArrayListGet(array, i)));
//        }
//        ClientsDestroy(AllClients);
//        close(serverFileDescriptor);
//    }
    return 0;
}