
#include "Stack.h"
#include<malloc.h>
struct StackNode_ {
    struct StackNode_ *Below;
    void *Object;
};

struct Stack_ {
    struct StackNode_ *Top;
};
struct StackNode_ *Stack_NewNode(void *object) {
    struct StackNode_ *pointer = (struct StackNode_*)malloc(sizeof(struct StackNode_));
    if (pointer == NULL) {
        return NULL;
    }
    pointer->Object = object;
    pointer->Below = NULL;
    return pointer;
}

Stack Stack_New() {
    Stack stack = (struct Stack_*)malloc(sizeof(struct Stack_));
    if(stack == NULL){
        return NULL;
    }
    stack->Top = NULL;
    return stack;
}
void Stack_Push(Stack stack, void *object) {
    struct StackNode_ *pointer = Stack_NewNode(object);
    if (pointer == NULL) {
        return;
    }
    pointer->Below = stack->Top;
    stack->Top = pointer;
}
void *Stack_Top(Stack stack) {
    return Stack_IsEmpty(stack) ? NULL : stack->Top->Object;
}
void *Stack_Pop(Stack stack) {
    if (Stack_IsEmpty(stack)) {
        return NULL;
    }
    void *object = stack->Top->Object;
    void *oldTop = stack->Top;
    stack->Top = stack->Top->Below;
    free(oldTop);
    return object;
}
bool Stack_IsEmpty(Stack stack) {
    return stack->Top == NULL;
}
void Stack_Destroy(Stack stack) {
    while (!Stack_IsEmpty(stack)) {
        Stack_Pop(stack);
    }
    free(stack);
}