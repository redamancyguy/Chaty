//
// Created by sunwenli on 2021/10/30.
//
#include <malloc.h>
#include <stdbool.h>
#include "BufferQueue.h"

struct BufferQueue_ {
    pthread_mutex_t mutex;
    unsigned short head, tail;
    struct Buffer data[65536];
    bool status;
};


BufferQueue BufferQueueNew() {
    BufferQueue queue = (BufferQueue) malloc(sizeof(struct BufferQueue_));
    if (queue == NULL) {
        return NULL;
    }
    if (pthread_mutex_init(&queue->mutex, NULL) != 0) {
        free(queue);
        return NULL;
    }
    queue->head = queue->tail = 0;
    queue->status = false;
    for(int i=0;i<65536;i++){
        queue->data[i].message.length = sizeof(struct sockaddr_in);
    }
    return queue;
}

int BufferQueueLock(BufferQueue queue) {
    return pthread_mutex_lock(&queue->mutex);
}
int BufferQueueUnlock(BufferQueue queue) {
    return pthread_mutex_unlock(&queue->mutex);
}
int BufferQueueTryLock(BufferQueue queue) {
    return pthread_mutex_trylock(&queue->mutex);
}
void BufferQueueDestroy(BufferQueue queue) {
    free(queue);
}

void BufferQueuePush(BufferQueue queue) {
    queue->status = true;
    ++queue->tail;
}

void BufferQueuePop(BufferQueue queue) {
    queue->status = false;
    ++queue->head;
}

bool BufferQueueIsFull(BufferQueue queue) {
    return queue->head == queue->tail && queue->status;
}

bool BufferQueueIsEmpty(BufferQueue queue) {
    return queue->head == queue->tail && !queue->status;
}
struct Message *BufferQueueFront(BufferQueue queue){
    return &queue->data[queue->head].message;
}
struct Message *BufferQueueEnd(BufferQueue queue){
    return &queue->data[queue->tail].message;
}