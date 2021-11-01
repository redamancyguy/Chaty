#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <pthread.h>
#include "DataStructure/Array.h"
#include "DataStructure/ArrayList.h"
#include "DataStructure/Hash.h"
#include "DataStructure/Queue.h"
#include "DataStructure/Stack.h"
#include "DataStructure/Tree.h"
#include "Database/User.h"
#include "relevant.h"
#include "BufferQueue/BufferQueue.h"
#include "DataStructure/HashList.h"

enum errorCode {
    NewSocket = 1,
    BindSocket = 2,
    DatabaseMutex = 3,
    NewThread = 4,
    StartThread = 5,
    JoinThread = 6,
    CancelThread = 7,
    NewHash = 10,
    NewQueue = 11,
};

//server start information
const short serverPort = 9999;
const int threadNumber = 10;

//server running information
int serverFileDescriptor;

Hash AllClients[65536];
HashList AllGroups;//HashList->Queue
_Noreturn void *Clear(void *pointer) {
    const int Pace = 1;
    while (true) {
        sleep(Pace);
        Array groups = HashListToArray(AllGroups);
        int TimOut = 5;
        for (long long i = 0, size = groups.size; i < size; i++) {
            Queue group = (Queue) groups.data[i];
            for (long long j = 0, groupSize = QueueSize(group); j < groupSize; j++) {
                if (time(NULL) - ((struct Client *) QueueFront(group))->time > TimOut) {
                    TreeDelete(((struct Client *) QueueFront(group))->groups, (void *) group);
                    QueuePop(group);
                } else {
                    QueuePush(group, QueueFront(group));
                    QueuePop(group);
                }
            }
            if (QueueSize(group) == 0) {
                HashListErase(AllGroups, (void *) group); //delete a group
            }
        }
        free(groups.data);
        TimOut = 7;
        for (int i = 0; i < 65536; i++) {
            Hash clientsHash = AllClients[i];
            Array clients = HashToArray(AllClients[i]);
            for (long long j = 0, size = clients.size; j < size; j++) {
                struct Client *client = clients.data[j];
                if (time(NULL) - client->time > TimOut) {
                    HashErase(clientsHash, (void *) (unsigned long long) client->address.sin_addr.s_addr);
                    TreeDestroy(client->groups);
                    free(client);
                }
            }
            free(clients.data);
        }
    }
}
int iii=0;
pthread_mutex_t mutex;
_Noreturn void *Handle(void *pointer) {
    BufferQueue queue = *((BufferQueue *) pointer);
    unsigned int HandleBufSize = 64;
    unsigned sleepPace = 1024;
    while (true) {
        if (!BufferQueueIsEmpty(queue)) {
            struct Message messages[HandleBufSize];
            int length = 0;
            for (int i = 0; i < HandleBufSize && !BufferQueueIsEmpty(queue); i++) {
                pthread_mutex_lock(&mutex);
                printf("%lu %d\n",pthread_self(),iii++);
                pthread_mutex_unlock(&mutex);
                messages[length++] = *BufferQueueFront(queue);
                BufferQueuePop(queue);
            }
            for (int i = 0; i < length; i++) {
                struct Message message = messages[i];
                switch (message.data.code) {
                    case TOUCH: {
                        struct Client *client = HashGet(AllClients[message.address.sin_port],
                                                        (void *) (unsigned long long) message.address.sin_addr.s_addr);
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
                        memcpy(&userBuf, message.data.data + 64, sizeof(struct User));
                        long place = GetUserPlaceByUsername(userBuf.username);
                        if (place == -1) {
                            strcpy(message.data.data, "Server : None username");
                        } else {
                            struct User temp;
                            GetUserByPlace(&temp, place);
                            if (strcmp(temp.password, userBuf.password) == 0) {
                                struct Client *client = (struct Client *) malloc(sizeof(struct Client));
                                if (client == NULL) {
                                    message.data.code = ERROR;
                                    strcpy(message.data.data, "Server : Login by error");
                                } else {
                                    client->groups = TreeNew();
                                    if (client->groups == NULL) {
                                        message.data.code = ERROR;
                                        free(client);
                                        strcpy(message.data.data, "Server : Login by error");
                                    } else {
                                        Hash clients = AllClients[message.address.sin_port];
                                        if (HashInsert(clients,
                                                       (void *) (unsigned long long) message.address.sin_addr.s_addr,
                                                       client)) {
                                            client->user = userBuf;
                                            client->address = message.address;
                                            client->length = message.length;
                                            client->time = time(NULL);
                                            strcpy(message.data.data, "Server : Login successfully");
                                        } else {
                                            TreeDestroy(client->groups);
                                            free(client);
                                            message.data.code = ERROR;
                                            strcpy(message.data.data, "Server : You're already logged in");
                                        }
                                    }
                                }
                            } else {
                                message.data.code = ERROR;
                                strcpy(message.data.data, "Server : Wrong password");
                            }
                        }
                        break;
                    }
                    case LOGOUT: {
                        Hash clients = AllClients[message.address.sin_port];
                        struct Client *client = HashGet(clients,
                                                        (void *) (unsigned long long) message.address.sin_addr.s_addr);
                        if (client != NULL) {
                            HashErase(clients, (void *) (unsigned long long) message.address.sin_addr.s_addr);
                            Array groups = TreeKeyToArray(client->groups);
                            for (long long k = 0; k < groups.size; k++) {
                                QueueDelete((Queue) groups.data[k], (void *) client);
                            }
                            free(groups.data);
                            TreeDestroy(client->groups);
                            free(client);
                            strcpy(message.data.data, "Server : Logout successfully");
                        } else {
                            message.data.code = ERROR;
                            strcpy(message.data.data, "Server : You haven't logged in yet");
                        }
                        break;
                    }
                    case REGISTER: {
                        struct User userBuf;
                        memcpy(&userBuf, message.data.data + 64, sizeof(struct User));
                        long place = GetUserReadyPlaceByUsername(userBuf.username);
                        if (place != -1) {
                            userBuf.id = place;
                            if (InsertUserByPlace(&userBuf, place) != -1) {
                                strcpy(message.data.data, "Server : Register successfully");
                            } else {
                                message.data.code = ERROR;
                                strcpy(message.data.data, "Server : Register by error");
                            }
                        } else {
                            message.data.code = ERROR;
                            strcpy(message.data.data, "Server : Duplicate username");
                        }
                        break;
                    }
                    case UNREGISTER: {
                        struct User userBuf;
                        memcpy(&userBuf, message.data.data + 64, sizeof(struct User));
                        long place = GetUserPlaceByUsername(userBuf.username);
                        if (place == -1) {
                            message.data.code = ERROR;
                            strcpy(message.data.data, "Server : None username");
                        } else {
                            struct User userTemp;
                            GetUserByPlace(&userTemp, place);
                            if (strcmp(userBuf.password, userTemp.password) == 0) {
                                if (RemoveUserByPlace(place) == place) {
                                    strcpy(message.data.data, "Server : Unregister successfully");
                                    struct Client *client = HashGet(AllClients[message.address.sin_port],
                                                                    (void *) (unsigned long long) message.address.sin_addr.s_addr);
                                    Array groups = TreeToArray(client->groups);
                                    for (int k = 0; k < groups.size; k++) {
                                        QueueDelete(((Queue) groups.data[k]), (void *) client);
                                    }
                                    HashErase(AllClients[message.address.sin_port],
                                              (void *) (unsigned long long) message.address.sin_addr.s_addr);
                                    free(groups.data);
                                } else {
                                    message.data.code = ERROR;
                                    strcpy(message.data.data, "Server : Unregister by error : unknown");
                                }
                            } else {
                                message.data.code = ERROR;
                                strcpy(message.data.data, "Server : Wrong password");
                            }
                        }
                        break;
                    }
                    case NEWGROUP: {
                        struct Client *client = HashGet(AllClients[message.address.sin_port],
                                                        (void *) (unsigned long long) message.address.sin_addr.s_addr);
                        if (client != NULL) {
                            Queue group = QueueNew();
                            if (group != NULL) {
                                *((unsigned long long *) (message.data.data + 64)) = (unsigned long long) group;
                                if (HashListInsert(AllGroups, (void *) group, (void *) group)
                                    && QueuePush(group, (void *) client) &&
                                    TreeInsert(client->groups, (void *) group, (void *) group)) {
                                    strcpy(message.data.data, "Server : Create group successfully");
                                } else {
                                    QueueDestroy(group);
                                    HashListErase(AllGroups, (void *) group);
                                    TreeDelete(client->groups, (void *) group);
                                    message.data.code = ERROR;
                                    strcpy(message.data.data, "Server : Create group by error : hash insert error");
                                }
                            } else {
                                message.data.code = ERROR;
                                strcpy(message.data.data, "Server : Create group by error");
                            }
                        } else {
                            message.data.code = ERROR;
                            strcpy(message.data.data, "Server : You haven't logged in yet");
                        }
                        break;
                    }
                    case JOIN: {
                        Queue group = HashListGet(AllGroups, (*(void **) (message.data.data + 64)));
                        struct Client *client = HashGet(AllClients[message.address.sin_port],
                                                        (void *) (unsigned long long) message.address.sin_addr.s_addr);
                        if (group != NULL) {
                            if (client != NULL) {
                                if (TreeGet(client->groups, (void *) group) != NULL) {
                                    message.data.code = ERROR;
                                    strcpy(message.data.data, "Server : You have already Joined");
                                } else {
                                    TreeInsert(client->groups, (void *) group, (void *) group);
                                    QueuePush(group, (void *) client);
                                    strcpy(message.data.data, "Server : Join successfully");
                                }
                            } else {
                                message.data.code = ERROR;
                                strcpy(message.data.data, "Server : You haven't logged in yet");
                            }
                        } else {
                            message.data.code = ERROR;
                            strcpy(message.data.data, "Server : Wrong group id");
                        }
                        break;
                    }
                    case DETACH: {
                        struct Client *client = HashGet(AllClients[message.address.sin_port],
                                                        (void *) (unsigned long long) message.address.sin_addr.s_addr);
                        if (client != NULL) {
                            Queue group = TreeGet(client->groups, (*(void **) (message.data.data + 64)));
                            if (group != NULL) {
                                TreeDelete(client->groups, (void *) group);
                                QueueDelete(group, (void *) client);
                                strcpy(message.data.data, "Server : Detach successfully");
                            } else {
                                message.data.code = ERROR;
                                strcpy(message.data.data, "Server : You are not in this group");
                            }
                        } else {
                            message.data.code = ERROR;
                            strcpy(message.data.data, "Server : You haven't logged in yet");
                        }
                        break;
                    }
                    case DELETEGROUP: {
                        struct Client *client = HashGet(AllClients[message.address.sin_port],
                                                        (void *) (unsigned long long) message.address.sin_addr.s_addr);
                        if (client != NULL) {
                            Queue group = TreeGet(client->groups, (*(void **) (message.data.data + 64)));
                            if (group != NULL) {
                                TreeDelete(client->groups, (void *) group);
                                Array temp = QueueToArray(group);
                                for (int k = 0; k < temp.size; k++) {
                                    TreeDelete(((struct Client *) temp.data[k])->groups, (void *) group);
                                }
                                QueueDestroy(group);
                                HashListErase(AllGroups, group);
                                strcpy(message.data.data, "Server : Detach successfully");
                            } else {
                                message.data.code = ERROR;
                                strcpy(message.data.data, "Server : You are not in this group");
                            }
                        } else {
                            message.data.code = ERROR;
                            strcpy(message.data.data, "Server : You haven't logged in yet");
                        }
                        break;
                    }
                    case CHAT: {
                        struct Client *client = HashGet(AllClients[message.address.sin_port],
                                                        (void *) (unsigned long long) message.address.sin_addr.s_addr);
                        if (client != NULL) {
                            Queue group = TreeGet(client->groups, (*(void **) (message.data.data + 64)));
                            if (group != NULL) {
                                Array clients = QueueToArray(group);
                                for (int k = 0; k < clients.size; k++) {
                                    struct Client *clientT = clients.data[k];
                                    sendto(serverFileDescriptor, &message.data, sizeof(struct CommunicationData), 0,
                                           (struct sockaddr *) &clientT->address, clientT->length);
                                }
                                free(clients.data);
                                continue;
                            } else {
                                message.data.code = ERROR;
                                strcpy(message.data.data, "Server : You are not in this group");
                            }
                        } else {
                            message.data.code = ERROR;
                            strcpy(message.data.data, "Server : Your have not logged in");
                        }
                        break;
                    }
                    default: {
                        message.data.code = UNKNOWN;
                        strcpy(message.data.data, "Server : Unknown");
                        break;
                    }
                }
                sendto(serverFileDescriptor, &message.data, sizeof(struct CommunicationData), 0,
                       (struct sockaddr *) &message.address, message.length);
            }
        } else {
            usleep(sleepPace);
        }
    }
}

_Noreturn void *GetMessage(void *pointer) {
    BufferQueue queue = *((BufferQueue *) pointer);
    while (true) {
        struct Message *buffer = BufferQueueEnd(queue);
        long long count = recvfrom(serverFileDescriptor, &buffer->data, sizeof(struct Buffer),
                                   0, (struct sockaddr *) &buffer->address, &buffer->length);
        switch (count) {
            case -1: {
                perror("Receive data fail");
                break;
            }
            case sizeof(struct CommunicationData): {
                while (true) {
                    if (BufferQueueIsFull(queue)) {
                        usleep(1024);
                    } else {
                        BufferQueuePush(queue);
                        break;
                    }
                }
                break;
            }
            default: {
                puts("Invalid data package");
                break;
            }
        }
    }
}
#include "FileServer/FileServer.h"
int main() {
    for (int ii = 0; ii < 1; ii++) {
        printf("%d\n", ii);
        serverFileDescriptor = socket(AF_INET, SOCK_DGRAM, 0);
        if (serverFileDescriptor < 0) {
            exit(NewSocket);
        }
        struct sockaddr_in serverAddress;
        memset(&serverAddress, 0, sizeof(struct sockaddr_in));
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
        serverAddress.sin_port = htons(serverPort);
        if (bind(serverFileDescriptor, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
            close(serverFileDescriptor);
            exit(BindSocket);
        }
        if (!UserDataBaseOpen()) {
            exit(-6);
        }
        AllGroups = HashListNew(1024);
        for (int i = 0; i < 65536; i++) {
            if ((AllClients[i] = HashNew(1024)) == NULL) {
                exit(NewHash);
            }
        }
        pthread_t clearThread;
        if (pthread_create(&clearThread, NULL, (void *(*)(void *)) Clear, NULL) != 0) {
            exit(StartThread);
        }
        Queue queues[threadNumber];
        for (int i = 0; i < threadNumber; i++) {
            if ((queues[i] = QueueNew()) == NULL) {
                exit(NewQueue);
            }
        }
        BufferQueue buffers[threadNumber];
        for (int i = 0; i < threadNumber; i++) {
            if ((buffers[i] = BufferQueueNew()) == NULL) {
                exit(NewQueue);
            }
        }

        pthread_t GetThreads[threadNumber];
        for (int i = 0; i < threadNumber; i++) {
            pthread_attr_t attr;
            pthread_attr_init(&attr);
            struct sched_param sched = {99};
            pthread_attr_setschedpolicy(&attr, SCHED_RR);
            pthread_attr_setschedparam(&attr, &sched);
            if (pthread_create(&GetThreads[i], &attr, (void *(*)(void *)) GetMessage, &buffers[i]) != 0) {
                exit(StartThread);
            }
        }
        pthread_t HandleThreads[threadNumber];
        for (int i = 0; i < threadNumber; i++) {
            if (pthread_create(&HandleThreads[i], NULL, (void *(*)(void *)) Handle, &buffers[i]) != 0) {
                exit(StartThread);
            }
        }

        fileServer();
        getchar();
        getchar();

        for (int i = 0; i < threadNumber; i++) {
            if (pthread_cancel(GetThreads[i]) != 0) {
                exit(CancelThread);
            }
            if (pthread_join(GetThreads[i], NULL) != 0) {
                exit(JoinThread);
            }
        }
        getchar();
        getchar();
        for (int i = 0; i < threadNumber; i++) {
            if (pthread_cancel(HandleThreads[i]) != 0) {
                exit(CancelThread);
            }
            if (pthread_join(HandleThreads[i], NULL) != 0) {
                exit(JoinThread);
            }
        }
        pthread_cancel(clearThread);
        if (pthread_join(clearThread, NULL) != 0) {
            exit(JoinThread);
        }
        for (int i = 0; i < threadNumber; i++) {
            BufferQueueDestroy(buffers[i]);
        }
        for (int i = 0; i < threadNumber; i++) {
            QueueDestroy(queues[i]);
        }
        for (int i = 0; i < 65536; i++) {
            HashDestroy(AllClients[i]);
        }
        HashListDestroy(AllGroups);
        UserDataBaseClose();
        close(serverFileDescriptor);
    }
    return 0;
}