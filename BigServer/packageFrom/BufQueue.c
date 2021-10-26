//
// Created by sunwenli on 2021/10/25.
//
#include <malloc.h>
#include "BufQueue.h"


struct BufQueue *BufQueueNew(unsigned long long capacity) {
    struct BufQueue *queue = (struct BufQueue *) malloc(sizeof(struct BufQueue));
    if (queue == NULL) {
        return NULL;
    }
    queue->dataBuff = malloc(sizeof(struct DataBuf) * capacity);
    if (queue->dataBuff == NULL) {
        free(queue);
        return NULL;
    }
    for(long long i=0;i<capacity;i++){
        queue->dataBuff[i].message.length = sizeof(struct sockaddr_in);
    }
    queue->capacity = capacity;
    queue->head = queue->tail = 0;
    queue->status = false;
    return queue;
}

void BufQueuePush(struct BufQueue *queue) {
    if (++queue->tail == queue->capacity) {
        queue->tail = 0;
    }
    queue->status = true;
}

void BufQueuePop(struct BufQueue *queue) {
    if (++queue->head == queue->capacity) {
        queue->head = 0;
    }
    queue->status = false;
}

struct DataBuf *BufQueueFront(struct BufQueue *queue) {
    return &queue->dataBuff[queue->head];
}

struct DataBuf *BufQueueBack(struct BufQueue *queue) {
    return &queue->dataBuff[queue->tail];
}
bool BufQueueIsEmpty(struct BufQueue *queue) {
    return queue->head == queue->tail && !queue->status;
}

bool BufQueueIsFull(struct BufQueue *queue) {
    return queue->tail == queue->head && queue->status;
}

void BufQueueDestroy(struct BufQueue *queue) {
    free(queue->dataBuff);
    free(queue);
}