//
// Created by 15066 on 2021/9/11.
//


#include<malloc.h>

#include "Stack.h"

#include "TreeMap.h"

struct TreeNode_ {
    struct TreeNode_ *P, *L, *R;
    void *Key;
    void *Value;
};
struct TreeMap_{
    struct TreeNode_ *Root;
};
struct TreeNode_ *Tree_NewNode(void *key, void *value) {
    struct TreeNode_ *pointer = (struct TreeNode_ *) malloc(sizeof(struct TreeNode_));
    if(pointer == NULL){
        return NULL;
    }
    pointer->P = NULL;
    pointer->L = NULL;
    pointer->R = NULL;
    pointer->Key = key;
    pointer->Value = value;
    return pointer;
}

bool Tree_ContainKey(TreeMap tree, void *key){
    struct TreeNode_ *pointer = tree->Root;
    while (pointer != NULL) {
        if (pointer->Key == key) {
            return true;
        }
        if (pointer->Key > key) {
            pointer = pointer->L;
        } else {
            pointer = pointer->R;
        }
    }
    return false;
}

bool Tree_IsRoot(TreeMap tree, struct TreeNode_ *p) {
    return tree->Root == p;
}
TreeMap Tree_New() {
    TreeMap tree = (struct TreeMap_*)malloc(sizeof(struct TreeMap_));
    if(tree == NULL){
        return NULL;
    }
    tree->Root = NULL;
    return tree;
}
bool Tree_Insert(TreeMap tree, void *key, void *value){
    if (tree->Root == NULL) {
        tree->Root = Tree_NewNode(key, value);
        if (tree->Root == NULL) {
            return false;
        }
        tree->Root->P = NULL;
        return true;
    }
    struct TreeNode_ *temp = tree->Root;
    while (1) {
        if (temp->Key == key) {
            return false;
        }
        if (temp->Key < key && temp->R != NULL) {
            temp = temp->R;
        } else if (temp->Key > key && temp->L != NULL) {
            temp = temp->L;
        } else {
            break;
        }
    }
    struct TreeNode_ *pointer = Tree_NewNode(key, value);
    if (pointer == NULL) {
        return false;
    }
    temp->Key < key ? (temp->R = pointer) : (temp->L = pointer);
    pointer->P = temp;
    return true;
}
bool Tree_SetOrInsert(TreeMap tree, void *key, void *value){
    if (tree->Root == NULL) {
        tree->Root = Tree_NewNode(key, value);
        if (tree->Root == NULL) {
            return false;
        }
        tree->Root->P = NULL;
        return true;
    }
    struct TreeNode_ *temp = tree->Root;
    while (1) {
        if (temp->Key == key) {
            temp->Key = key;
            temp->Value = value;
            return true;
        }
        if (temp->Key < key && temp->R != NULL) {
            temp = temp->R;
        } else if (temp->Key > key && temp->L != NULL) {
            temp = temp->L;
        } else {
            break;
        }
    }
    struct TreeNode_ *pointer = Tree_NewNode(key, value);
    if (pointer == NULL) {
        return false;
    }
    temp->Key < key ? (temp->R = pointer) : (temp->L = pointer);
    pointer->P = temp;
    return true;
}
bool Tree_Set(TreeMap tree, void *key, void *value){
    if (tree->Root == NULL) {
        return false;
    }
    struct TreeNode_ *pointer = tree->Root;
    while (pointer != NULL) {
        if (pointer->Key == key) {
            pointer->Value = value;
            return true;
        }
        if (pointer->Key > key) {
            pointer = pointer->L;
        } else {
            pointer = pointer->R;
        }
    }
    return false;
}
void *Tree_Get(TreeMap tree, void *key) {
    struct TreeNode_ *pointer = tree->Root;
    while (pointer != NULL) {
        if (pointer->Key == key) {
            return pointer->Value;
        }
        if (pointer->Key > key) {
            pointer = pointer->L;
        } else {
            pointer = pointer->R;
        }
    }
    return NULL;
}
bool Tree_Delete(TreeMap tree, void *key){
    struct TreeNode_ *pointer = tree->Root;
    while (pointer != NULL) {
        if (pointer->Key == key) {
            break;
        }
        if (pointer->Key > key) {
            pointer = pointer->L;
        } else {
            pointer = pointer->R;
        }
    }
    if (pointer == NULL) {
        return false;
    }
    if (pointer->L == NULL && pointer->R == NULL) {// 修改顶点问题
        if (Tree_IsRoot(tree, pointer)) {
            tree->Root = NULL;
            free(pointer);
            return true;
        }
        if (pointer->P->L == pointer) {
            pointer->P->L = NULL;
        } else {
            pointer->P->R = NULL;
        }
    } else if (pointer->R == NULL) {
        if (Tree_IsRoot(tree, pointer)) {
            tree->Root = pointer->L;
            pointer->L->P = NULL;
            free(pointer);
            return true;
        }
        if (pointer->P->L == pointer) { //pointer 是左子树
            pointer->P->L = pointer->L;
        } else {
            pointer->P->R = pointer->L;
        }
        pointer->L->P = pointer->P;
    } else if (pointer->L == NULL) {
        if (Tree_IsRoot(tree, pointer)) {
            tree->Root = pointer->R;
            pointer->R->P = NULL;
            free(pointer);
            return true;
        }
        if (pointer->P->L == pointer) {
            pointer->P->L = pointer->R;
        } else {
            pointer->P->R = pointer->R;
        }
        pointer->R->P = pointer->P;
    } else {
        struct TreeNode_ *ii;
        for (ii = pointer->R; ii->L != NULL; ii = ii->L);//右子树最小的一个
        pointer->Key = ii->Key;
        pointer->Value = ii->Value;
        if (ii->P->L == ii) {
            ii->P->L = ii->R;
        } else {
            ii->P->R = ii->R;
        }
        if(ii->R != NULL){
            ii->R->P = ii->P;
        }
        pointer = ii;
    }
    free(pointer);
    return true;
}
void Tree_Destroy(TreeMap tree){
    struct TreeNode_ *pointer = tree->Root;
    Stack stack = Stack_New();
    Stack stackToDestroy = Stack_New();
    while (!Stack_IsEmpty(stack) || pointer) {
        while (pointer != NULL) {
            Stack_Push(stack, pointer);
            pointer = pointer->L;
        }
        if (!Stack_IsEmpty(stack)) {
            pointer = Stack_Pop(stack);
            Stack_Push(stackToDestroy, pointer);
            pointer = pointer->R;
        }
    }
    Stack_Destroy(stack);
    while (!Stack_IsEmpty(stackToDestroy)) {
        free(Stack_Pop(stackToDestroy));
    }
    Stack_Destroy(stackToDestroy);
    free(tree);
}
void Tree_Display_(struct TreeNode_ *tree,int layer) {
    if(!tree){
        return;
    }
    Tree_Display_(tree->L,layer+1);
    for(int i=0;i<layer*3;i++){
        putchar(' ');
    }
    printf("%p:%p\n",tree->Key,tree->Value);
    Tree_Display_(tree->R,layer+1);
}
void Tree_Display(TreeMap tree){
    if (tree->Root == NULL) {
        return;
    } else {
        Tree_Display_(tree->Root,0);
    }
}