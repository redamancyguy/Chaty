//
// Created by sunwenli on 2021/10/24.
//
#include <pthread.h>
#include <malloc.h>
#include <stdlib.h>
#include "Queue.h"

struct Node_ {
    struct Node_ *next;
    void *data;
};

struct Queue_ {
    pthread_rwlock_t rwlock;
    struct Node_ *head;
    struct Node_ *tail;
    long long size;
};
long long QueueSize(Queue queue){
    return queue->size;
}
Queue QueueNew() {
    Queue queue = (Queue) malloc(sizeof(struct Queue_));
    if (queue == NULL) {
        return NULL;
    }
    void *temp = malloc(sizeof(struct Node_));
    if (temp == NULL) {
        free(queue);
        return NULL;
    }
    if (pthread_rwlock_init(&queue->rwlock, NULL) != 0) {
        free(queue->head);
        free(queue);
        return NULL;
    }
    ((struct Node_ *) temp)->next = NULL;
    queue->head = queue->tail = (struct Node_ *) temp;
    queue->size == 0;
    return queue;
}

int QueueWLock(Queue queue) {
    return pthread_rwlock_wrlock(&queue->rwlock);
}

int QueueRLock(Queue queue) {
    return pthread_rwlock_rdlock(&queue->rwlock);
}

int QueueUnLock(Queue queue) {
    return pthread_rwlock_unlock(&queue->rwlock);
}

void QueueDestroy(Queue queue) {
    pthread_rwlock_destroy(&queue->rwlock);
    while (queue->head != queue->tail) {
        void *temp = queue->head;
        queue->head = queue->head->next;
        free(temp);
    }
    free(queue->head);
    free(queue);
}

bool QueueIsEmpty(Queue queue) {
    bool result = queue->head == queue->tail;
    return result;
}

void *QueueFront(Queue queue) {
    void *result = queue->head->next->data;
    return result;
}

bool QueueDelete(Queue queue, void *data) {
    struct Node_ *i = queue->head, *tail = queue->tail;
    for (; i->next != tail; i = i->next) {
        if (i->next->data == data) {
            void *temp = i->next;
            i->next = i->next->next;
            free(temp);
            --queue->size;
            return true;
        }
    }
    if (i->next->data == data) {
        free(i->next);
        queue->tail = i;
        --queue->size;
        return true;
    }
    return false;
}

Array QueueToArray(Queue queue) {
    Array array;
    array.data = (void **) malloc(sizeof(void *) * queue->size);
    if (array.data == NULL) {
        array.size = 0;
    } else {
        array.size = queue->size;
        unsigned long long place = 0;
        struct Node_ *i, *tail;
        for (i = queue->head->next, tail = queue->tail; i != tail; i = i->next) {
            array.data[place++] = i->data;
        }
        array.data[place] = i->data;
    }
    return array;
}

bool QueuePush(Queue queue, void *data) {
    void *temp = malloc(sizeof(struct Node_));
    if (temp == NULL) {
        return false;
    }
    queue->tail->next = (struct Node_ *) temp;
    ((struct Node_ *) temp)->data = data;
    queue->tail = (struct Node_ *) temp;
    ++queue->size;
    return true;
}

void QueuePop(Queue queue) {
    void *temp = queue->head;
    queue->head = queue->head->next;
    free(temp);
    --queue->size;
}