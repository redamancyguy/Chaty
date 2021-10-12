//
// Created by sunwenli on 2021/10/12.
//

#include "messagequeue.h"
#include <malloc.h>
struct QueueNode{
    struct Message message;
    struct QueueNode *next;
};
//next->tail
struct QueueNode *NewQueueNode(){
    struct QueueNode *node = (struct QueueNode *) malloc(sizeof(struct QueueNode));
    if(node == NULL){
        return NULL;
    }
    return node;
}

MessageQueue New_Queue(){
    MessageQueue queue = (MessageQueue_ *) malloc(sizeof(MessageQueue_));
    if(queue == NULL){
        return NULL;
    }
    queue->head = queue->tail = NULL;
    return queue;
}

bool Push_Queue(MessageQueue queue,struct Message message){
    if(queue->tail == NULL){
        queue->head = queue->tail = NewQueueNode();
        if(queue->head != NULL){
            queue->head->message = message;
            return true;
        }else{
            return false;
        }
    }
    queue->tail->next = NewQueueNode();
    if(queue->tail->next != NULL){
        queue->tail->next->message = message;
        queue->tail = queue->tail->next;
        return true;
    }else{
        return false;
    }
}

struct Message Front_Queue(MessageQueue queue){
    return queue->head->message;
}

bool Empty_Queue(MessageQueue queue){
    return queue->tail == NULL;
}

void Pop_Queue(MessageQueue queue){
    if(queue->head==queue->tail){
        free(queue->tail);
        queue->tail = queue->head = NULL;
        return;
    }
    void *temp = queue->head;
    queue->head = queue->head->next;
    free(temp);
}

void Destroy_Queue(MessageQueue queue){
    while(queue->head != queue->tail){
        void *temp = queue->head;
        queue->head = queue->head->next;
        free(temp);
    }
    free(queue->head);
    free(queue);
}