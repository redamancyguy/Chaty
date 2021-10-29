//
// Created by sunwenli on 2021/10/24.
//
#include <pthread.h>
#include <malloc.h>
#include "Queue.h"

struct Node_ {
    struct Node_ *next;
    void *data;
};

struct Queue_ {
    pthread_mutex_t mutex;
    struct Node_ *head;
    struct Node_ *tail;
};

//struct Node_ *node = (struct Node_ *)malloc(sizeof(struct Node_));

int QueueLock(Queue queue){
   return pthread_mutex_lock(&queue->mutex);
}
int QueueTryLock(Queue queue){
   return pthread_mutex_trylock(&queue->mutex);
}
int QueueUnlock(Queue queue){
   return pthread_mutex_unlock(&queue->mutex);
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
    ((struct Node_ *) temp)->next = NULL;
    queue->head = queue->tail = (struct Node_ *) temp;
    if(pthread_mutex_init(&queue->mutex,NULL) != 0){
        free(queue->head);
        free(queue);
        return NULL;
    }
    return queue;
}

void QueueDestroy(Queue queue) {
    pthread_mutex_destroy(&queue->mutex);
    while (queue->head != queue->tail) {
        void *temp = queue->head;
        queue->head = queue->head->next;
        free(temp);
    }
    free(queue->head);
    free(queue);
}

bool QueueIsEmpty(Queue queue) {
    return queue->head == queue->tail;
}

void *QueueFront(Queue queue) {
    return queue->head->next->data;
}
bool QueueDelete(Queue queue, void *data){
    struct Node_*i=queue->head,*tail=queue->tail;
    for(;i->next!=tail;i=i->next){
        if(i->next->data == data){
            void *temp = i->next;
            i->next = i->next->next;
            free(temp);
            return true;
        }
    }
    if(i->next->data == data){
        free(i->next);
        queue->tail = i;
        return true;
    }
    return false;
}
ArrayList QueueToArrayList(Queue queue){
    ArrayList array = ArrayListNew();
    if(array == NULL){
        return NULL;
    }
    for(struct Node_*i=queue->head,*tail=queue->tail;i!=tail;i=i->next){
        ArrayListPushBack(array,i->next->data);
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
    return true;
}

void QueuePop(Queue queue) {
    void *temp = queue->head;
    queue->head = queue->head->next;
    free(temp);
}