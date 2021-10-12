//////////////////////////////////

#ifndef STACK_H
#define STACK_H
#include<stdbool.h>
typedef struct Stack_* Stack;
Stack Stack_New();
void Stack_Push(Stack stack, void *object);
void *Stack_Top(Stack stack);
void *Stack_Pop(Stack stack);
void Stack_Destroy(Stack stack);
bool Stack_IsEmpty(Stack stack);
#endif //STACK_H