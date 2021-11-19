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
#include "DataStructure/Queue.h"
#include "DataStructure/Stack.h"
#include "DataStructure/Tree.h"
#include "DataStructure/HashList.h"

#include "BufferQueue.h"
#include "User.h"
#include "Communication.h"

enum errorCode {
    NewSocket,
    BindSocket,
    DatabaseMutex,
    NewThread,
    StartThread,
    JoinThread,
    CancelThread,
    NewHash,
    NewQueue,
};
const short serverPort = 9999;
const int threadNumber = 10;
int serverFileDescriptor;

_Noreturn void *Handle(void *pointer) {
    BufferQueue queue = *((BufferQueue *) pointer);
    unsigned int HandleBufSize = 64;
    unsigned sleepPace = 1024;
    while (true) {
        if (!BufferQueueIsEmpty(queue)) {
            struct Message messages[HandleBufSize];
            int length = 0;
            for (int i = 0; i < HandleBufSize && !BufferQueueIsEmpty(queue); i++) {
                messages[length++] = *BufferQueueFront(queue);
                BufferQueuePop(queue);
            }
            for (int i = 0; i < length; i++) {
                struct Message message = messages[i];
                switch (message.data.code) {
                    case TOUCH: {
                        struct Client *client = ClientsGet(AllClient,
                                                           ((struct TouchPackage *) (message.data.data))->userId);
                        if (client != NULL) {
                            client->time = time(NULL);
                            strcpy(message.data.data, "Server : YES");
                        } else {
                            strcpy(message.data.data, "Server : NO");
                        }
                        break;
                    }
                    case LOGIN: {
                        int result = ClientsInsert(AllClient, ((struct LogPackage *) (message.data.data))->user.id);
                        if (result == 0) {
                            strcpy(message.data.data, "Server : Login successfully");
                        } else if (result == 1) {
                            strcpy(message.data.data, "Server : You have already logged");
                        } else {
                            strcpy(message.data.data, "Server : Login by error");
                        }
                        break;
                    }
                    case LOGOUT: {
                        struct Client *client = ClientsGet(AllClient,
                                                           ((struct LogPackage *) (message.data.data))->user.id);
                        if (client != NULL) {
                            Array personalGroups = TreeToArray(client->groups);
                            for (long long k = 0; k < personalGroups.size; k++) {
                                QueueDelete((Queue) personalGroups.data[k], (void *) client);
                            }
                            free(personalGroups.data);
                            if (ClientsDelete(AllClient, ((struct LogPackage *) (message.data.data))->user.id) == 0) {
                                strcpy(message.data.data, "Server : Logout successfully");
                            } else {
                                strcpy(message.data.data, "Server : Logout by error");
                            }
                        } else {
                            message.data.code = ERROR;
                            strcpy(message.data.data, "Server : You haven't logged in yet");
                        }
                        break;
                    }
                    case NEWGROUP: {
                        struct Client *client = ClientsGet(AllClient,
                                                           ((struct GroupPackage *) (message.data.data))->userId);
                        if (client != NULL) {
                            Queue group = GroupsInsert(groups, ((struct GroupPackage *) (message.data.data))->groupId);
                            if (group != NULL) {
                                QueuePush(group, client);
                                TreeInsert(client->groups,    (void *) (long long) ((struct GroupPackage *) (message.data.data))->groupId,   group);
                                strcpy(message.data.data, "Server : Create group successfully");
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
                        Queue group = GroupsGet(groups, ((struct GroupPackage *) (message.data.data))->groupId);
                        if (group != NULL) {
                            struct Client *client = ClientsGet(AllClient,
                                                               ((struct GroupPackage *) (message.data.data))->userId);
                            if (client != NULL) {
                                if (!TreeInsert(client->groups,(void *) (long long) ((struct GroupPackage *) (message.data.data))->groupId, (void *) group)) {
                                    message.data.code = ERROR;
                                    strcpy(message.data.data, "Server : You have already Joined");
                                } else {
                                    if (!QueuePush(group, (void *) client)) {
                                        strcpy(message.data.data, "Server : Join by error");
                                    } else {
                                        strcpy(message.data.data, "Server : Join successfully");
                                    }
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
                        struct Client *client = ClientsGet(AllClient,
                                                           ((struct GroupPackage *) (message.data.data))->userId);
                        if (client != NULL) {
                            Queue group = TreeGet(client->groups,
                                                  (void *) (long long) ((struct GroupPackage *) (message.data.data))->groupId);
                            if (group != NULL) {
                                TreeDelete(client->groups,(void *) (long long) ((struct GroupPackage *) (message.data.data))->groupId);
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
                        struct Client *client = ClientsGet(AllClient,
                                                           ((struct GroupPackage *) (message.data.data))->userId);
                        if (client != NULL) {
                            Queue group = TreeGet(client->groups,
                                                  (void *) (long long) ((struct GroupPackage *) (message.data.data))->groupId);
                            if (group != NULL) {
                                Array ar = QueueToArray(group);
                                for (int k = 0; k < ar.size; k++) {
                                    //把 所有 客户端 中 的 group 全 删除掉
                                    TreeDelete(((struct Client *) ar.data[k])->groups,
                                               (void *) (long long) ((struct GroupPackage *) (message.data.data))->groupId);
                                }
                                QueueDestroy(group);
                                free(ar.data);
                                strcpy(message.data.data, "Server : Delete successfully");
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
//                    case CHAT: {
//                        struct Clients *client = HashListGet(AllClients[message.address.sin_port],
//                                                             (void *) (unsigned long long) message.address.sin_addr.s_addr);
//                        if (client != NULL) {
//                            Queue group = TreeGet(client->groups, (*(void **) (message.data.data + 64)));
//                            if (group != NULL) {
//                                Array clients = QueueToArray(group);
//                                for (int k = 0; k < clients.size; k++) {
//                                    struct Clients *clientT = clients.data[k];
//                                    sendto(serverFileDescriptor, &message.data, sizeof(struct Package), 0,
//                                           (struct sockaddr *) &clientT->address, clientT->length);
//                                }
//                                free(clients.data);
//                                continue;
//                            } else {
//                                message.data.code = ERROR;
//                                strcpy(message.data.data, "Server : You are not in this group");
//                            }
//                        } else {
//                            message.data.code = ERROR;
//                            strcpy(message.data.data, "Server : Your have not logged in");
//                        }
//                        break;
//                    }
                    default: {
                        message.data.code = UNKNOWN;
                        strcpy(message.data.data, "Server : Unknown");
                        break;
                    }
                }
                sendto(serverFileDescriptor, &message.data, sizeof(struct Package), 0,
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
        long long count = recvfrom(serverFileDescriptor, &buffer->data, sizeof(struct Package),
                                   0, (struct sockaddr *) &buffer->address, &buffer->length);
        switch (count) {
            case -1: {
                perror("package size < 0");
                break;
            }
            case sizeof(struct Package): {
                while (BufferQueueIsFull(queue)) {
                    usleep(1024);
                }
                BufferQueuePush(queue);
                break;
            }
            default: {
                perror("package size error");
            }
        }

    }
}

void Clear() {
    int timeOut = 5;
    while(1){
        sleep(timeOut);
        for(int i=0;i<65536*1024;i++){
            struct Client *client = ClientsGet(AllClient,i);
            if(client != NULL && client->time < time(NULL)-timeOut){
                Array gs = TreeToArray(client->groups);
                for(int j=0;j<gs.size;j++){
                    QueueDelete(((Queue) gs.data[j]),client);
                }
                ClientsDelete(AllClient,i);
                free(gs.data);
            }
        }
    }
}

int main() {
    for (int ii = 0; ii < 1000; ii++) {
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

        if ((groups = GroupsNew()) == NULL) {
            exit(-8);
        }

        if ((AllClient = ClientsNew()) == NULL) {
            exit(-8);
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
        GroupsDestroy(groups);
        ClientsDestroy(AllClient);
        close(serverFileDescriptor);
    }
    return 0;
}
