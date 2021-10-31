//
// Created by sunwenli on 2021/10/30.
//
#include <malloc.h>
#include <stdbool.h>
#include <stdlib.h>
#include "BufferQueue.h"

struct BufferQueue_ {
    pthread_rwlock_t rwlock;
    unsigned short head, tail;
    struct Buffer data[65536];
    bool status;
};

BufferQueue BufferQueueNew() {
    BufferQueue queue = (BufferQueue) malloc(sizeof(struct BufferQueue_));
    if (queue == NULL) {
        return NULL;
    }
    if (pthread_rwlock_init(&queue->rwlock, NULL) != 0) {
        free(queue);
        return NULL;
    }
    queue->head = queue->tail = 0;
    queue->status = false;
    for (int i = 0; i < 65536; i++) {
        queue->data[i].message.length = sizeof(struct sockaddr_in);
    }
    return queue;
}

void BufferQueueDestroy(BufferQueue queue) {
    pthread_rwlock_destroy(&queue->rwlock);
    free(queue);
}

void BufferQueuePush(BufferQueue queue) {
    if(pthread_rwlock_wrlock(&queue->rwlock) !=0 ){
        exit(-4);
    }
    queue->status = true;
    ++queue->tail;
    pthread_rwlock_unlock(&queue->rwlock);
}

void BufferQueuePop(BufferQueue queue) {
    if(pthread_rwlock_wrlock(&queue->rwlock) !=0 ){
        exit(-4);
    }
    queue->status = false;
    ++queue->head;
    pthread_rwlock_unlock(&queue->rwlock);
}

bool BufferQueueIsFull(BufferQueue queue) {
    if(pthread_rwlock_rdlock(&queue->rwlock) != 0){
        exit(-4);
    }
    bool result = queue->head == queue->tail && queue->status;
    pthread_rwlock_unlock(&queue->rwlock);
    return result;
}

bool BufferQueueIsEmpty(BufferQueue queue) {
    if(pthread_rwlock_rdlock(&queue->rwlock) != 0){
        exit(-4);
    }
    bool result = queue->head == queue->tail && !queue->status;
    pthread_rwlock_unlock(&queue->rwlock);
    return result;
}

struct Message *BufferQueueFront(BufferQueue queue) {
    if(pthread_rwlock_rdlock(&queue->rwlock) != 0){
        exit(-4);
    }
    struct Message *result = &queue->data[queue->head].message;
    pthread_rwlock_unlock(&queue->rwlock);
    return result;
}

struct Message *BufferQueueEnd(BufferQueue queue) {
    if(pthread_rwlock_rdlock(&queue->rwlock) != 0){
        exit(-4);
    }
    struct Message *result = &queue->data[queue->tail].message;
    pthread_rwlock_unlock(&queue->rwlock);
    return result;
}