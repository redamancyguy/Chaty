//
// Created by sunwenli on 2021/10/12.
//

#include "messagequeue.h"
#include <malloc.h>

struct QueueNode {
    struct Message message;
    struct QueueNode *next;
};

//next->tail
struct QueueNode *NewQueueNode() {
    struct QueueNode *node = (struct QueueNode *) malloc(sizeof(struct QueueNode));
    if (node == NULL) {
        return NULL;
    }
    return node;
}

MessageQueue New_Queue() {
    MessageQueue queue = (MessageQueue_ *) malloc(sizeof(MessageQueue_));
    if (queue == NULL) {
        return NULL;
    }
    queue->head = queue->tail = NewQueueNode();
    return queue;
}

bool Push_Queue(MessageQueue queue, struct Message message) {
    queue->tail->next = NewQueueNode();
    queue->tail->next->message = message;
    queue->tail = queue->tail->next;
    return true;
}

struct Message Front_Queue(MessageQueue queue) {
    return queue->head->next->message;
}

bool Empty_Queue(MessageQueue queue) {
    return queue->head == queue->tail;
}

void Pop_Queue(MessageQueue queue) {
    void *temp = queue->head;
    queue->head = queue->head->next;
    free(temp);
}

void Destroy_Queue(MessageQueue queue) {
    while (queue->head != queue->tail) {
        void *temp = queue->head;
        queue->head = queue->head->next;
        free(temp);
    }
    free(queue->head);
    free(queue);
}