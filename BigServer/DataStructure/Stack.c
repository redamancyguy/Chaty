//
// Created by sunwenli on 2021/10/28.
//
#include <malloc.h>
#include <pthread.h>
#include <stdlib.h>
#include "Stack.h"

struct StackNode_ {
    struct StackNode_ *below;
    void *data;
};

struct Stack_ {
    pthread_rwlock_t rwlock;
    struct StackNode_ *top, *bottom;
};


Stack StackNew() {
    Stack stack = (struct Stack_ *) malloc(sizeof(struct Stack_));
    if (stack == NULL) {
        return NULL;
    }
    if (pthread_rwlock_init(&stack->rwlock, NULL) != 0) {
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

Array StackToArray(Stack stack) {
    if(pthread_rwlock_rdlock(&stack->rwlock) !=0){
        exit(-4);
    }
    long long length = 0;
    for (struct StackNode_ *i = stack->top, *bottom = stack->bottom; i != bottom; i = i->below) { length++; }
    Array array;
    array.data = (void **) malloc(sizeof(void *) * length);
    if (array.data == NULL) {
        array.size = 0;
    } else {
        array.size = 0;
        for (struct StackNode_ *i = stack->top, *bottom = stack->bottom; i != bottom; i = i->below) {
            array.data[array.size++] = i->data;
        }
    }
    pthread_rwlock_unlock(&stack->rwlock);
    return array;
}

bool StackPush(Stack stack, void *data) {
    if(pthread_rwlock_wrlock(&stack->rwlock) != 0){
        exit(-4);
    }
    struct StackNode_ *temp = (struct StackNode_ *) malloc(sizeof(struct StackNode_));
    if (temp == NULL) {
        pthread_rwlock_unlock(&stack->rwlock);
        return false;
    }
    temp->below = stack->top;
    temp->data = data;
    stack->top = temp;
    pthread_rwlock_unlock(&stack->rwlock);
    return true;
}

void StackPop(Stack stack) {
    if(pthread_rwlock_wrlock(&stack->rwlock) !=0){
        exit(-4);
    }
    struct StackNode_ *temp = stack->top;
    stack->top = stack->top->below;
    free(temp);
}

bool StackIsEmpty(Stack stack) {
    if(pthread_rwlock_rdlock(&stack->rwlock) !=0){
        exit(-4);
    }
    bool result = stack->top == stack->bottom;
    pthread_rwlock_unlock(&stack->rwlock);
    return result;
}

void *StackTop(Stack stack) {
    if(pthread_rwlock_rdlock(&stack->rwlock) !=0){
        exit(-4);
    }
    void *result = stack->top->data;
    pthread_rwlock_unlock(&stack->rwlock);
    return result;
}

void StackDestroy(Stack stack) {
    while (stack->top != stack->bottom) {
        void *temp = stack->top;
        stack->top = stack->top->below;
        free(temp);
    }
    pthread_rwlock_destroy(&stack->rwlock);
    free(stack->top);
    free(stack);
}
