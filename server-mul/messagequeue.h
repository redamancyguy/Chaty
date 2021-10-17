//
// Created by sunwenli on 2021/10/12.
//

#include "commondata.h"
#include <stdbool.h>
#ifndef MESSAGEQUEUE_H
#define MESSAGEQUEUE_H



typedef struct{
    struct QueueNode *head;
    struct QueueNode *tail;
}MessageQueue_,*MessageQueue;

MessageQueue New_Queue();

bool Push_Queue(MessageQueue queue,struct Message *message);

struct Message *Front_Queue(MessageQueue queue);

bool Empty_Queue(MessageQueue queue);
void Pop_Queue(MessageQueue queue);

void Destroy_Queue(MessageQueue queue);

#endif //MESSAGEQUEUE_H
