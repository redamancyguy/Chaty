//
// Created by sunwenli on 2021/10/28.
//
#include <malloc.h>
#include <pthread.h>
#include "Stack.h"

struct StackNode_ {
    struct StackNode_ *below;
    void *data;
};

struct Stack_ {
    pthread_mutex_t mutex;
    struct StackNode_ *top, *bottom;
};


Stack StackNew() {
    Stack stack = (struct Stack_ *) malloc(sizeof(struct Stack_));
    if (stack == NULL) {
        return NULL;
    }
    if(pthread_mutex_init(&stack->mutex,NULL)!=0){
        free(stack);
        return NULL;
    }
    stack->top = stack->bottom = (struct StackNode_ *) malloc(sizeof(struct StackNode_));
    if (stack->bottom == NULL) {
        free(stack);
        return NULL;
    }
    return stack;
}
int StackLock(Stack stack){
    return pthread_mutex_lock(&stack->mutex);
}
int StackUnlock(Stack stack){
    return pthread_mutex_unlock(&stack->mutex);
}
int StackTryLock(Stack stack){
    return pthread_mutex_trylock(&stack->mutex);
}
bool StackPush(Stack stack, void *data) {
    struct StackNode_ *temp = (struct StackNode_ *) malloc(sizeof(struct StackNode_));
    if (temp == NULL) {
        return false;
    }
    temp->below = stack->top;
    temp->data = data;
    stack->top = temp;
    return true;
}

void StackPop(Stack stack) {
    struct StackNode_ *temp = stack->top;
    stack->top = stack->top->below;
    free(temp);
}

bool StackIsEmpty(Stack stack) {
    return stack->top == stack->bottom;
}

void *StackTop(Stack stack) {
    return stack->top->data;
}

void StackDestroy(Stack stack) {
    while (stack->top != stack->bottom) {
        void *temp = stack->top;
        stack->top = stack->top->below;
        free(temp);
    }
    pthread_mutex_destroy(&stack->mutex);
    free(stack->top);
    free(stack);
}
