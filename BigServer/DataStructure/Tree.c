//
// Created by sunwenli on 2021/10/28.
//
#include <malloc.h>
#include <pthread.h>
#include <stdlib.h>
#include "Tree.h"

struct TreeNode_ {
    struct TreeNode_ *left, *right;
    void *key;
    void *value;
    long long height;
};
struct Tree_ {
    pthread_rwlock_t rwlock;
    struct TreeNode_ *root;
    long long size;
};

Tree TreeNew() {
    Tree tree = (Tree) malloc(sizeof(struct Tree_));
    if (tree == NULL) {
        return NULL;
    }
    tree->root = NULL;
    tree->size = 0;
    if (pthread_rwlock_init(&tree->rwlock, NULL) != 0) {
        free(tree);
        return NULL;
    }
    return tree;
}

static struct TreeNode_ *NewNode(void *key, void *value) {
    struct TreeNode_ *node = (struct TreeNode_ *) malloc(sizeof(struct TreeNode_));
    if (node == NULL) {
        return NULL;
    }
    node->left = node->right = NULL;
    node->key = key;
    node->value = value;
    node->height = 1;
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

long long Height(struct TreeNode_ *node) {
    return node == NULL ? 0 : node->height;
}

long long GetBalance(struct TreeNode_ *node) {
    return node == NULL ? 0 : Height(node->left) - Height(node->right);
}

static struct TreeNode_ *LLRotate(struct TreeNode_ *node) {
    struct TreeNode_ *temp = node->left;
    node->left = temp->right;
    temp->right = node;
    node->height = 1 + (Height(node->left) > Height(node->right) ? Height(node->left) : Height(node->right));
    temp->height = 1 + (Height(temp->left) > Height(temp->right) ? Height(temp->left) : Height(temp->right));
    return temp;
}

static struct TreeNode_ *LRRotate(struct TreeNode_ *node) {
    struct TreeNode_ *temp = node->left;
    struct TreeNode_ *temp2 = temp->right;
    node->left = temp2->right;
    temp2->right = node;
    temp->right = temp2->left;
    temp2->left = temp;
    node->height = 1 + (Height(node->left) > Height(node->right) ? Height(node->left) : Height(node->right));
    temp->height = 1 + (Height(temp->left) > Height(temp->right) ? Height(temp->left) : Height(temp->right));
    temp2->height = 1 + (Height(temp2->left) > Height(temp2->right) ? Height(temp2->left) : Height(temp2->right));
    return temp2;
}

static struct TreeNode_ *RLRotate(struct TreeNode_ *node) {
    struct TreeNode_ *temp = node->right;
    struct TreeNode_ *temp2 = temp->left;
    node->right = temp2->left;
    temp2->left = node;
    temp->left = temp2->right;
    temp2->right = temp;
    node->height = 1 + (Height(node->left) > Height(node->right) ? Height(node->left) : Height(node->right));
    temp->height = 1 + (Height(temp->left) > Height(temp->right) ? Height(temp->left) : Height(temp->right));
    temp2->height = 1 + (Height(temp2->left) > Height(temp2->right) ? Height(temp2->left) : Height(temp2->right));
    return temp2;
}

static struct TreeNode_ *RRRotate(struct TreeNode_ *node) {
    struct TreeNode_ *temp = node->right;
    node->right = temp->left;
    temp->left = node;
    node->height = 1 + (Height(node->left) > Height(node->right) ? Height(node->left) : Height(node->right));
    temp->height = 1 + (Height(temp->left) > Height(temp->right) ? Height(temp->left) : Height(temp->right));
    return temp;
}

unsigned long long TreeSize(Tree tree) {
    if (pthread_rwlock_rdlock(&tree->rwlock) != 0) {
        exit(-4);
    }
    unsigned long long result = tree->size;
    pthread_rwlock_unlock(&tree->rwlock);
    return result;
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
    node->height = 1 + (Height(node->left) > Height(node->right) ? Height(node->left) : Height(node->right));
    long long difference = GetBalance(node);
    if (difference > 1) {
        if (key < node->left->key) {
            return LLRotate(node);
        } else {
            return LRRotate(node);
        }
    }
    if (difference < -1) {
        if (key < node->right->key) {
            return RLRotate(node);
        } else {
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
        if (node->left == NULL) {
            if (node->right == NULL) {
                free(node);
                --tree->size;
                return NULL;
            } else {
                void *temp = node;
                node = node->right;
                free(temp);
                --tree->size;
            }
        } else if (node->right == NULL) {
            void *temp = node;
            node = node->left;
            free(temp);
            --tree->size;
        } else {
            struct TreeNode_ *temp;
            for (temp = node->left; temp->right != NULL; temp = temp->right);
            node->key = temp->key;
            node->value = temp->value;
            node->left = Delete(tree, node->left, temp->key);
        }
    }
    node->height = 1 + (Height(node->left) > Height(node->right) ? Height(node->left) : Height(node->right));
    long long difference = GetBalance(node);
    if (difference > 1) {
        if (Height(node->left->left) - Height(node->left->right) >= 0) {
            return LLRotate(node);
        } else {
            return LRRotate(node);
        }
    }
    if (difference < -1) {
        if (Height(node->right->left) - Height(node->right->left) > 0) {
            return RLRotate(node);
        } else {
            return RRRotate(node);
        }
    }
    return node;
}

bool TreeDelete(Tree tree, void *key) {
    if(pthread_rwlock_wrlock(&tree->rwlock) != 0){
        exit(-4);
    }
    unsigned long long temp = tree->size;
    tree->root = Delete(tree, tree->root, key);
    bool result = temp == tree->size ? false : true;
    pthread_rwlock_unlock(&tree->rwlock);
    return result;
}

void *Get(struct TreeNode_ *node, void *key) {
    if (node == NULL) {
        return NULL;
    }
    if (key < node->key) {
        return Get(node->left, key);
    } else if (key > node->key) {
        return Get(node->right, key);
    } else {
        return node->value;
    }
}

void *TreeMinimumKey(Tree tree) {
    if(pthread_rwlock_rdlock(&tree->rwlock) != 0){
        exit(-4);
    }
    struct TreeNode_ *node = tree->root;
    while(node->left!=NULL){
        node = node->left;
    }
    void *result = node->key;
    pthread_rwlock_unlock(&tree->rwlock);
    return result;
}

void *TreeGet(Tree tree, void *key) {
    if(pthread_rwlock_rdlock(&tree->rwlock) != 0){
        exit(-4);
    }
    void *result = Get(tree->root, key);
    pthread_rwlock_unlock(&tree->rwlock);
    return result;
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
    if(pthread_rwlock_rdlock(&tree->rwlock) != 0){
        exit(-4);
    }
    Show(tree->root, 0);
    pthread_rwlock_unlock(&tree->rwlock);
}

bool TreeInsert(Tree tree, void *key, void *value) {
    if(pthread_rwlock_wrlock(&tree->rwlock) !=0){
        exit(-4);
    }
    unsigned long long temp = tree->size;
    tree->root = Insert(tree, tree->root, key, value);
    bool result = temp != tree->size;
    pthread_rwlock_unlock(&tree->rwlock);
    return result;
}

void TreeDestroy(Tree tree) {
    pthread_rwlock_destroy(&tree->rwlock);
    Destroy(tree->root);
    free(tree);
}

static void ToArray(void **data, unsigned long long *place, struct TreeNode_ *node) {
    if (node == NULL) {
        return;
    }
    ToArray(data, place, node->left);
    data[(*place)++] = node->value;
    ToArray(data, place, node->right);
}

Array TreeToArray(Tree tree) {
    if(pthread_rwlock_rdlock(&tree->rwlock) !=0){
        exit(-4);
    }
    Array array;
    array.data = (void **) malloc(sizeof(void *) * tree->size);
    if (array.data == NULL) {
        array.size = 0;
    } else {
        unsigned long long place = 0;
        ToArray(array.data, &place, tree->root);
        array.size = tree->size;
    }
    pthread_rwlock_unlock(&tree->rwlock);
    return array;
}

static void KeyToArray(void **data, unsigned long long *place, struct TreeNode_ *node) {
    if (node == NULL) {
        return;
    }
    ToArray(data, place, node->left);
    data[(*place)++] = node->key;
    ToArray(data, place, node->right);
}

Array TreeKeyToArray(Tree tree) {
    if(pthread_rwlock_rdlock(&tree->rwlock) !=0){
        exit(-4);
    }
    Array array;
    array.data = (void **) malloc(sizeof(void *) * tree->size);
    if (array.data == NULL) {
        array.size = 0;
    } else {
        unsigned long long place = 0;
        KeyToArray(array.data, &place, tree->root);
        array.size = tree->size;
    }
    pthread_rwlock_unlock(&tree->rwlock);
    return array;
}