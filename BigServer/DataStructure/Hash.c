//
// Created by sunwenli on 2021/10/24.
//
#include <string.h>
#include <malloc.h>
#include <pthread.h>
#include "Hash.h"

struct Node_ {
    bool status;
    void *key;
    void *value;
};
struct Hash_ {
    pthread_mutex_t mutex;
    struct Node_ *table;
    long long capacity;
    long long size;
};

struct Hash_Iterator NewHash_Iterator(Hash hash){
    struct Hash_Iterator it;
    it.hash = hash;
    it.place = 0;
    return it;
}

void *NextHash_Iterator(struct Hash_Iterator *it){
    while(it->place < it->hash->capacity){
        if(it->hash->table[it->place].status){
            return it->hash->table[it->place++].value;
        }
        it->place++;
    }
    return NULL;
}


int HashLock(Hash hash){
    return pthread_mutex_lock(&hash->mutex);
}
int HashUnlock(Hash hash){
    return pthread_mutex_unlock(&hash->mutex);
}
int HashTryLock(Hash hash){
    return pthread_mutex_trylock(&hash->mutex);
}


long long HashSize(Hash hash){
    return hash->size;
}
long long HashCapacity(Hash hash){
    return hash->capacity;
}
Hash HashNew(const long long capacity) {
    Hash hash = (Hash) malloc(sizeof(struct Hash_));
    if (hash == NULL) {
        return NULL;
    }
    hash->table = (struct Node_ *) malloc(sizeof(struct Node_) * capacity);
    if (hash->table == NULL) {
        free(hash);
        return NULL;
    }
    hash->size = 0;
    hash->capacity = capacity;
    memset(hash->table, 0, sizeof(struct Node_) * capacity);
    if(pthread_mutex_init(&hash->mutex,NULL) != 0){
        free(hash->table);
        free(hash);
        return NULL;
    }
    return hash;
}

void HashDestroy(Hash hash) {
    pthread_mutex_destroy(&hash->mutex);
    free(hash->table);
    free(hash);
}


void HashClear(Hash hash) {
    for (long long i = 0, capacity = hash->capacity; i < capacity; i++) {
        if (hash->table[i].status) {
            hash->table[i].status = false;
            hash->table[i].key = NULL;
            hash->table[i].value = NULL;
        }
    }
    hash->size = 0;
}

bool HashInsert(Hash hash,void *const key,void *const value) {
    long long flag = (long long) key % hash->capacity;
    long long doubleLength = hash->capacity - flag < flag ? hash->capacity - flag : flag;
    long long i = 0;
    while (i < doubleLength) {
        long long temp = flag + i;
        if (!hash->table[temp].status) {
            hash->table[temp].key = key;
            hash->table[temp].value = value;
            hash->table[temp].status = true;
            ++hash->size;
            return true;
        } else if (hash->table[temp].key == key) {
            return false;
        }
        temp = flag - i;
        if (!hash->table[temp].status) {
            hash->table[temp].key = key;
            hash->table[temp].value = value;
            hash->table[temp].status = true;
            ++hash->size;
            return true;
        } else if (hash->table[temp].key == key) {
            return false;
        }
        i++;
    }
    if (flag == i) {
        i = flag + i;
        while (i < hash->capacity) {
            if (!hash->table[i].status) {
                hash->table[i].key = key;
                hash->table[i].value = value;
                hash->table[i].status = true;
                ++hash->size;
                return true;
            } else if (hash->table[i].key == key) {
                return false;
            }
            i++;
        }
    } else {
        i = flag - i - 1;
        while (i >= 0) {
            if (!hash->table[i].status) {
                hash->table[i].key = key;
                hash->table[i].value = value;
                hash->table[i].status = true;
                ++hash->size;
                return true;
            } else if (hash->table[i].key == key) {
                return false;
            }
            i--;
        }
    }
    return false;
}


void *HashGet(Hash hash,void *const key) {
    long long flag = (long long)key % hash->capacity;
    long long doubleLength = hash->capacity - flag < flag ? hash->capacity - flag : flag;
    long long i = 0;
    while (i < doubleLength) {
        long long temp = flag + i;
        if (hash->table[temp].status && hash->table[temp].key == key ){
            return hash->table[temp].value;
        }
        temp = flag - i;
        if (hash->table[temp].status && hash->table[temp].key == key ){
            return hash->table[temp].value;
        }
        i++;
    }
    if (flag == i) {
        i =  flag + i;
        while (i < hash->capacity) {
            if (hash->table[i].status && hash->table[i].key == key ){
                return hash->table[i].value;
            }
            i++;
        }
    } else {
        i = flag - i - 1;
        while (i >= 0) {
            if (hash->table[i].status && hash->table[i].key == key ){
                return hash->table[i].value;
            }
            i--;
        }
    }
    return NULL;
}

bool HashSet(Hash hash,void *const key,void *const value) {
    long long flag = (long long) key % hash->capacity;
    long long doubleLength = hash->capacity - flag < flag ? hash->capacity - flag : flag;
    long long i = 0;
    while (i < doubleLength) {
        long long temp = flag + i;
        if (!hash->table[temp].status) {
            hash->table[temp].key = key;
            hash->table[temp].value = value;
            hash->table[temp].status = true;
            ++hash->size;
            return true;
        } else if (hash->table[temp].key == key) {
            hash->table[temp].value = value;
            return true;
        }
        temp = flag - i;
        if (!hash->table[temp].status) {
            hash->table[temp].key = key;
            hash->table[temp].value = value;
            hash->table[temp].status = true;
            ++hash->size;
            return true;
        } else if (hash->table[temp].key == key) {
            hash->table[temp].value = value;
            return true;
        }
        i++;
    }
    if (flag == i) {
        i = flag + i;
        while (i < hash->capacity) {
            if (!hash->table[i].status) {
                hash->table[i].key = key;
                hash->table[i].value = value;
                hash->table[i].status = true;
                ++hash->size;
                return true;
            } else if (hash->table[i].key == key) {
                hash->table[i].value = value;
                return true;
            }
            i++;
        }
    } else {
        i = flag - i - 1;
        while (i >= 0) {
            if (!hash->table[i].status) {
                hash->table[i].key = key;
                hash->table[i].value = value;
                hash->table[i].status = true;
                ++hash->size;
                return true;
            } else if (hash->table[i].key == key) {
                hash->table[i].value = value;
                return true;
            }
            i--;
        }
    }
    return false;
}

bool HashErase(Hash hash,void *const key) {
    long long flag = (long long)key % hash->capacity;
    long long doubleLength = hash->capacity - flag < flag ? hash->capacity - flag : flag;
    long long i = 0;
    while (i < doubleLength) {
        long long temp = flag + i;
        if (hash->table[temp].status && hash->table[temp].key == key ){
            hash->table[temp].status = false;
            hash->size--;
            return true;
        }
        temp = flag - i;
        if (hash->table[temp].status && hash->table[temp].key == key ){
            hash->table[temp].status = false;
            hash->size--;
            return true;
        }
        i++;
    }
    if (flag == i) {
        i =  flag + i;
        while (i < hash->capacity) {
            if (hash->table[i].status && hash->table[i].key == key ){
                hash->table[i].status = false;
                hash->size--;
                return true;
            }
            i++;
        }
    } else {
        i = flag - i - 1;
        while (i >= 0) {
            if (hash->table[i].status && hash->table[i].key == key ){
                hash->table[i].status = false;
                hash->size--;
                return true;
            }
            i--;
        }
    }
    return false;
}