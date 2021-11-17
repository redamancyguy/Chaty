//
// Created by sunwenli on 2021/10/31.
//

#include "HashList.h"
#include <string.h>
#include <malloc.h>
#include <pthread.h>
#include <stdlib.h>

struct LinkNode_ {
    bool status;
    void *key;
    void *value;
    struct LinkNode_ *next;
};
struct HashList_ {
    pthread_rwlock_t rwlock;
    struct LinkNode_ *lists;//带头节点的链表 节点数组(链表头数组)
    long long capacity;
    long long size;
};

void HashListDestroy(HashList hash) {
    pthread_rwlock_destroy(&hash->rwlock);
    for (long long i = 0; i < hash->capacity; i++) {
        struct LinkNode_ *j = hash->lists[i].next;
        while (j != NULL) {
            void *temp = j;
            j = j->next;
            free(temp);
        }
    }
    free(hash->lists);
    free(hash);
}

HashList HashListNew(const long long capacity) {
    HashList hash = (HashList) malloc(sizeof(struct HashList_));
    if (hash == NULL) {
        return NULL;
    }
    hash->lists = (struct LinkNode_ *) calloc(sizeof(struct LinkNode_) , capacity);
    if (hash->lists == NULL) {
        free(hash);
        return NULL;
    }
    hash->capacity = capacity;
    hash->size = 0;
    if (pthread_rwlock_init(&hash->rwlock, NULL) != 0) {
        free(hash->lists);
        free(hash);
        return NULL;
    }
    return hash;
}

bool HashListInsert(HashList hash, void *const key, void *const value) {
    if (pthread_rwlock_wrlock(&hash->rwlock) != 0) {
        exit(-4);
    }
    long long index = (long long) key % hash->capacity;
    struct LinkNode_ *last = &hash->lists[index];
    struct LinkNode_ *head = last->next;
    while (head != NULL) {
        if (head->key == key) {
            pthread_rwlock_unlock(&hash->rwlock);
            return false;
        }
        last = head;
        head = head->next;
    }
    if ((last->next = (struct LinkNode_ *) malloc(sizeof(struct LinkNode_))) == NULL) {
        pthread_rwlock_unlock(&hash->rwlock);
        return false;
    }
    last->next->next = NULL;
    last->next->key = key;
    last->next->value = value;
    pthread_rwlock_unlock(&hash->rwlock);
    ++hash->size;
    return true;
}
void *HashListGet(HashList hash, void *const key) {
    if (pthread_rwlock_rdlock(&hash->rwlock) != 0) {
        exit(-4);
    }
    long long index = (long long) key % hash->capacity;
    struct LinkNode_ *head = hash->lists[index].next;
    while (head != NULL) {
        if (head->key == key) {
            pthread_rwlock_unlock(&hash->rwlock);
            return head->value;
        }
        head = head->next;
    }
    pthread_rwlock_unlock(&hash->rwlock);
    return NULL;
}

bool HashListSet(HashList hash, void *const key, void *const value){
    if (pthread_rwlock_wrlock(&hash->rwlock) != 0) {
        exit(-4);
    }
    long long index = (long long) key % hash->capacity;
    struct LinkNode_ *head = hash->lists[index].next;
    while (head != NULL) {
        if (head->key == key) {
            pthread_rwlock_unlock(&hash->rwlock);
            head->value = value;
            return true;
        }
        head = head->next;
    }
    pthread_rwlock_unlock(&hash->rwlock);
    return false;
}

bool HashListErase(HashList hash, void *const key) {
    if (pthread_rwlock_wrlock(&hash->rwlock) != 0) {
        exit(-4);
    }
    long long index = (long long) key % hash->capacity;
    struct LinkNode_ *head = &hash->lists[index];
    while (head->next != NULL) {
        if (head->next->key == key) {
            void *temp = head->next;
            head->next = head->next->next;
            free(temp);
            pthread_rwlock_unlock(&hash->rwlock);
            --hash->size;
            return true;
        }
        head = head->next;
    }
    pthread_rwlock_unlock(&hash->rwlock);
    return false;
}
Array HashListToArray(HashList hash) {
    if (pthread_rwlock_rdlock(&hash->rwlock) != 0) {
        exit(-4);
    }
    Array array;
    array.data = (void **) malloc(sizeof(void *) * hash->size);
    if (array.data == NULL) {
        array.size = 0;
    } else {
        array.size = 0;
        for (long long i = 0; i < hash->capacity; i++) {
            struct LinkNode_ *j = hash->lists[i].next;
            while (j != NULL) {
                array.data[array.size++] = j->value;
                j = j->next;
            }
        }
    }
    pthread_rwlock_unlock(&hash->rwlock);
    return array;
}