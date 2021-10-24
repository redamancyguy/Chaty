//
// Created by sunwenli on 2021/10/25.
//
#include <malloc.h>
#include "messageQueue.h"


struct MessageQueue *MessageQueueNew(unsigned long long capacity) {
    struct MessageQueue *queue = (struct MessageQueue *) malloc(sizeof(struct MessageQueue));
    if (queue == NULL) {
        return NULL;
    }
    queue->messages = malloc(sizeof(struct Message) * capacity);
    if (queue->messages == NULL) {
        free(queue);
        return NULL;
    }
    queue->capacity = capacity;
    queue->head = queue->tail = 0;
    queue->status = false;
    return queue;
}

void MessageQueuePush(struct MessageQueue *queue, struct Message *message) {
    queue->messages[queue->tail++] = *message;
    if (queue->tail == queue->capacity) {
        queue->tail = 0;
    }
    queue->status = true;
}

struct Message *MessageQueuePop(struct MessageQueue *queue) {
    struct Message *result = &queue->messages[queue->head++];
    if (queue->head == queue->capacity) {
        queue->head = 0;
    }
    queue->status = false;
    return result;
}

struct Message *MessageQueueFront(struct MessageQueue *queue) {
    return &queue->messages[queue->head];
}

bool MessageQueueIsEmpty(struct MessageQueue *queue) {
    return queue->head == queue->tail && !queue->status;
}

bool MessageQueueIsFull(struct MessageQueue *queue) {
    return queue->tail == queue->head && queue->status;
}

void MessageQueueDestroy(struct MessageQueue *queue) {
    free(queue->messages);
    free(queue);
}