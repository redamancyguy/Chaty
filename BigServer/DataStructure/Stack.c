//
// Created by sunwenli on 2021/10/28.
//
#include <malloc.h>
#include "Stack.h"

struct StackNode_ {
    struct StackNode_ *below;
    void *data;
};

struct Stack_ {
    struct StackNode_ *top, *bottom;
};


Stack StackNew() {
    Stack stack = (struct Stack_ *) malloc(sizeof(struct Stack_));
    if (stack == NULL) {
        return NULL;
    }
    stack->top = stack->bottom = (struct StackNode_ *) malloc(sizeof(struct StackNode_));
    if (stack->bottom == NULL) {
        free(stack);
        return NULL;
    }
    return stack;
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
    free(stack->top);
    free(stack);
}
