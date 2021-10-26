//
// Created by sunwenli on 2021/10/24.
//
#include <pthread.h>
#include <malloc.h>
#include "Queue.h"

struct Queue_ {
    pthread_mutex_t mutex;
    struct Node_ *head;
    struct Node_ *tail;
};

struct Node_ {
    struct Node_ *next;
    void *data;
};

//struct Node_ *node = (struct Node_ *)malloc(sizeof(struct Node_));

int LockQueue(Queue queue){
   return pthread_mutex_lock(&queue->mutex);
}
int TryLockQueue(Queue queue){
   return pthread_mutex_trylock(&queue->mutex);
}
int UnlockQueue(Queue queue){
   return pthread_mutex_unlock(&queue->mutex);
}
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
    if(pthread_mutex_init(&queue->mutex,NULL) != 0){
        free(queue->head);
        free(queue);
        return NULL;
    }
    return queue;
}

void DestroyQueue(Queue queue) {
    pthread_mutex_destroy(&queue->mutex);
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
    queue->tail = (struct Node_ *) temp;
    return true;
}

void PopQueue(Queue queue) {
    void *temp = queue->head;
    queue->head = queue->head->next;
    free(temp);
}