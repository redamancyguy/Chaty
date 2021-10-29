//
// Created by sunwenli on 2021/10/28.
//
#include <malloc.h>
#include <pthread.h>
#include "Tree.h"

struct TreeNode_ {
    struct TreeNode_ *left, *right;
    void *key;
    void *value;
};
struct Tree_ {
    pthread_mutex_t mutex;
    unsigned long long size;
    struct TreeNode_ *root;
};

int TreeLock(Tree tree) {
    return pthread_mutex_lock(&tree->mutex);
}
int TreeUnlock(Tree tree) {
    return pthread_mutex_unlock(&tree->mutex);
}
int TreeTryLock(Tree tree) {
    return pthread_mutex_trylock(&tree->mutex);
}
Tree TreeNew() {
    Tree tree = (Tree) malloc(sizeof(struct Tree_));
    if (tree == NULL) {
        return NULL;
    }
    tree->root = NULL;
    tree->size = 0;
    return tree;
}

static long long Height(struct TreeNode_ *node) {   //length between node and leaf
    if (node == NULL) {
        return 0;
    }
    long long left = Height(node->left);
    long long right = Height(node->right);
    return 1 + (left > right ? left : right);
}

static struct TreeNode_ *NewNode(void *key, void *value) {
    struct TreeNode_ *node = (struct TreeNode_ *) malloc(sizeof(struct TreeNode_));
    if (node == NULL) {
        return NULL;
    }
    node->left = node->right = NULL;
    node->key = key;
    node->value = value;
    return node;
}

static void Destroy(struct TreeNode_ *node) {
    if (node == NULL) {
        return;
    }
    Destroy(node->left);
    Destroy(node->right);
    free(node);
}

static struct TreeNode_ *LLRotate(struct TreeNode_ *node) {
    struct TreeNode_ *temp = node->left;
    node->left = temp->right;
    temp->right = node;
    return temp;
}

static struct TreeNode_ *LRRotate(struct TreeNode_ *node) {
    struct TreeNode_ *temp = node->left;
    struct TreeNode_ *temp2 = temp->right;
    node->left = temp2->right;
    temp2->right = node;
    temp->right = temp2->left;
    temp2->left = temp;
    return temp2;
}

static struct TreeNode_ *RLRotate(struct TreeNode_ *node) {
    struct TreeNode_ *temp = node->right;
    struct TreeNode_ *temp2 = temp->left;
    node->right = temp2->left;
    temp2->left = node;
    temp->left = temp2->right;
    temp2->right = temp;
    return temp2;
}

static struct TreeNode_ *RRRotate(struct TreeNode_ *node) {
    struct TreeNode_ *temp = node->right;
    node->right = temp->left;
    temp->left = node;
    return temp;
}

unsigned long long TreeSize(Tree tree) {
    return tree->size;
}

static struct TreeNode_ *Insert(Tree tree, struct TreeNode_ *node, void *key, void *value) {
    if (node == NULL) {
        struct TreeNode_ *temp = NewNode(key, value);
        if (temp == NULL) {
            exit(-4);
        }
        ++tree->size;
        return temp;
    }
    if (key < node->key) {
        node->left = Insert(tree, node->left, key, value);
    } else if (key > node->key) {
        node->right = Insert(tree, node->right, key, value);
    } else {
        return node;
    }
    long long difference = Height(node->left) - Height(node->right);
    if (difference > 1) {
        if (key < node->left->key) {
            return LLRotate(node);
        }
        if (key > node->left->key) {
            return LRRotate(node);
        }
    }
    if (difference < -1) {
        if (key < node->right->key) {
            return RLRotate(node);
        }
        if (key > node->right->key) {
            return RRRotate(node);
        }
    }
    return node;
}

struct TreeNode_ *Delete(Tree tree, struct TreeNode_ *node, void *key) {
    if (node == NULL) {
        return NULL;
    }
    if (key < node->key) {
        node->left = Delete(tree, node->left, key);
    } else if (key > node->key) {
        node->right = Delete(tree, node->right, key);
    } else {
        if(node->left == NULL){
            if(node->right == NULL){
                free(node);
                --tree->size;
                return NULL;
            }else{
                void *temp = node;
                node = node->right;
                free(temp);
                --tree->size;
            }
        }else if(node->right == NULL){
            void *temp = node;
            node = node->left;
            free(temp);
            --tree->size;
        }
        else {
            struct TreeNode_ *temp;
            for (temp = node->left; temp->right != NULL; temp = temp->right);
            node->key = temp->key;
            node->value = temp->value;
            node->left = Delete(tree, node->left, temp->key);
        }
    }
    long long difference = Height(node->left) - Height(node->right);
    if (difference > 1) {
        if (Height(node->left->left)- Height(node->left->right) >=0 ) {
            return LLRotate(node);
        }
        if (Height(node->left->left)- Height(node->left->right) < 0) {
            return LRRotate(node);
        }
    }
    if (difference < -1) {
        if (Height(node->right->left)- Height(node->right->left) > 0) {
            return RLRotate(node);
        }
        if (Height(node->right->left)- Height(node->right->left) <= 0) {
            return RRRotate(node);
        }
    }
    return node;
}

bool TreeDelete(Tree tree, void *key) {
    unsigned long long temp = tree->size;
    tree->root = Delete(tree,tree->root, key);
    return temp == tree->size ? false : true;
}
void *Get(struct TreeNode_ *node,void *key){
    if(node == NULL){
        return NULL;
    }
    if(key < node->key){
        return Get(node->left,key);
    }else if(key > node->key){
        return Get(node->right,key);
    }else{
        return node->value;
    }
}

void *TreeGet(Tree tree,void *key){
    return Get(tree->root,key);
}

static void Show(struct TreeNode_ *node, int num) {
    if (node == NULL) {
        return;
    }
    Show(node->left, num + 5);
    for (int i = 0; i < num; i++, putchar(' '));
    printf("%lld\n", node->key);
    Show(node->right, num + 5);
}

void ShowTree(Tree tree) {
    Show(tree->root, 0);
}

bool TreeInsert(Tree tree, void *key, void *value) {
    unsigned long long temp = tree->size;
    tree->root = Insert(tree, tree->root, key, value);
    return temp == tree->size ? false : true;
}

void TreeDestroy(Tree tree) {
    Destroy(tree->root);
}
