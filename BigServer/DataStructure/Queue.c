//
// Created by sunwenli on 2021/10/24.
//
#include <malloc.h>
#include "Queue.h"

struct Queue_ {
    struct Node_ *head;
    struct Node_ *tail;
};

struct Node_ {
    struct Node_ *next;
    void *data;
};

//struct Node_ *node = (struct Node_ *)malloc(sizeof(struct Node_));


Queue NewQueue() {
    Queue queue = (Queue) malloc(sizeof(struct Queue_));
    if (queue == NULL) {
        return NULL;
    }
    void *temp = malloc(sizeof(struct Node_));
    if (temp == NULL) {
        free(queue);
        return NULL;
    }
    ((struct Node_ *) temp)->next = NULL;
    queue->head = queue->tail = (struct Node_ *) temp;
    return queue;
}

void DestroyQueue(Queue queue) {
    while (queue->head != queue->tail) {
        void *temp = queue->head;
        queue->head = queue->head->next;
        free(temp);
    }
    free(queue->head);
    free(queue);
}

bool IsEmptyQueue(Queue queue) {
    return queue->head == queue->tail;
}

void *FrontQueue(Queue queue) {
    return queue->head->next->data;
}

bool PushQueue(Queue queue, void *data) {
    void *temp = malloc(sizeof(struct Node_));
    if (temp == NULL) {
        return false;
    }
    queue->tail->next = (struct Node_ *) temp;
    ((struct Node_ *) temp)->data = data;
    queue->tail = queue->tail->next;
    return true;
}

void *PopQueue(Queue queue) {
    void *temp = queue->head;
    void *data = queue->head->next->data;
    queue->head = queue->head->next;
    free(temp);
    return data;
}