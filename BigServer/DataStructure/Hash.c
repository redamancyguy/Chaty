//
// Created by sunwenli on 2021/10/24.
//
#include <string.h>
#include <malloc.h>
#include <pthread.h>
#include <stdlib.h>
#include "Hash.h"

struct Node_ {
    bool status;
    void *key;
    void *value;
};
struct Hash_ {
    pthread_rwlock_t rwlock;
    struct Node_ *table;
    long long capacity;
    long long size;
};

long long HashSize(Hash hash) {
    if (pthread_rwlock_rdlock(&hash->rwlock) == 0) {
        long long result = hash->size;
        pthread_rwlock_unlock(&hash->rwlock);
        return result;
    }
    exit(-4);
}


long long HashCapacity(Hash hash) {
    if (pthread_rwlock_rdlock(&hash->rwlock) == 0) {
        long long result = hash->capacity;
        pthread_rwlock_unlock(&hash->rwlock);
        return result;
    }
    exit(-4);
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
    hash->capacity = capacity;
    hash->size = 0;
    memset(hash->table, 0, sizeof(struct Node_) * capacity);
    if (pthread_rwlock_init(&hash->rwlock, NULL) != 0) {
        free(hash->table);
        free(hash);
        return NULL;
    }
    return hash;
}

void HashDestroy(Hash hash) {
    pthread_rwlock_destroy(&hash->rwlock);
    free(hash->table);
    free(hash);
}


void HashClear(Hash hash) {
    if (pthread_rwlock_rdlock(&hash->rwlock) != 0) {
        exit(-4);
    }
    for (long long i = 0, capacity = hash->capacity; i < capacity; i++) {
        if (hash->table[i].status) {
            hash->table[i].status = false;
            hash->table[i].key = NULL;
            hash->table[i].value = NULL;
        }
    }
    pthread_rwlock_unlock(&hash->rwlock);
}

bool HashInsert(Hash hash, void *const key, void *const value) {
    if (pthread_rwlock_wrlock(&hash->rwlock) != 0) {
        exit(-4);
    }
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
            pthread_rwlock_unlock(&hash->rwlock);
            return true;
        } else if (hash->table[temp].key == key) {
            pthread_rwlock_unlock(&hash->rwlock);
            return false;
        }
        temp = flag - i;
        if (!hash->table[temp].status) {
            hash->table[temp].key = key;
            hash->table[temp].value = value;
            hash->table[temp].status = true;
            ++hash->size;
            pthread_rwlock_unlock(&hash->rwlock);
            return true;
        } else if (hash->table[temp].key == key) {
            pthread_rwlock_unlock(&hash->rwlock);
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
                pthread_rwlock_unlock(&hash->rwlock);
                return true;
            } else if (hash->table[i].key == key) {
                pthread_rwlock_unlock(&hash->rwlock);
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
                pthread_rwlock_unlock(&hash->rwlock);
                return true;
            } else if (hash->table[i].key == key) {
                pthread_rwlock_unlock(&hash->rwlock);
                return false;
            }
            i--;
        }
    }
    pthread_rwlock_unlock(&hash->rwlock);
    return false;
}


void *HashGet(Hash hash, void *const key) {
    if (pthread_rwlock_rdlock(&hash->rwlock) != 0) {
        exit(-4);
    }
    long long flag = (long long) key % hash->capacity;
    long long doubleLength = hash->capacity - flag < flag ? hash->capacity - flag : flag;
    long long i = 0;
    while (i < doubleLength) {
        long long temp = flag + i;
        if (hash->table[temp].status && hash->table[temp].key == key) {
            pthread_rwlock_unlock(&hash->rwlock);
            return hash->table[temp].value;
        }
        temp = flag - i;
        if (hash->table[temp].status && hash->table[temp].key == key) {
            pthread_rwlock_unlock(&hash->rwlock);
            return hash->table[temp].value;
        }
        i++;
    }
    if (flag == i) {
        i = flag + i;
        while (i < hash->capacity) {
            if (hash->table[i].status && hash->table[i].key == key) {
                pthread_rwlock_unlock(&hash->rwlock);
                return hash->table[i].value;
            }
            i++;
        }
    } else {
        i = flag - i - 1;
        while (i >= 0) {
            if (hash->table[i].status && hash->table[i].key == key) {
                pthread_rwlock_unlock(&hash->rwlock);
                return hash->table[i].value;
            }
            i--;
        }
    }
    pthread_rwlock_unlock(&hash->rwlock);
    return NULL;
}

bool HashSet(Hash hash, void *const key, void *const value) {
    if (pthread_rwlock_wrlock(&hash->rwlock) != 0) {
        exit(-4);
    }
    long long flag = (long long) key % hash->capacity;
    long long doubleLength = hash->capacity - flag < flag ? hash->capacity - flag : flag;
    long long i = 0;
    while (i < doubleLength) {
        long long temp = flag + i;
        if (!hash->table[temp].status) {
            hash->table[temp].key = key;
            hash->table[temp].value = value;
            hash->table[temp].status = true;
            pthread_rwlock_unlock(&hash->rwlock);
            return true;
        } else if (hash->table[temp].key == key) {
            hash->table[temp].value = value;
            pthread_rwlock_unlock(&hash->rwlock);
            return true;
        }
        temp = flag - i;
        if (!hash->table[temp].status) {
            hash->table[temp].key = key;
            hash->table[temp].value = value;
            hash->table[temp].status = true;
            pthread_rwlock_unlock(&hash->rwlock);
            return true;
        } else if (hash->table[temp].key == key) {
            hash->table[temp].value = value;
            pthread_rwlock_unlock(&hash->rwlock);
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
                pthread_rwlock_unlock(&hash->rwlock);
                return true;
            } else if (hash->table[i].key == key) {
                hash->table[i].value = value;
                pthread_rwlock_unlock(&hash->rwlock);
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
                pthread_rwlock_unlock(&hash->rwlock);
                return true;
            } else if (hash->table[i].key == key) {
                hash->table[i].value = value;
                pthread_rwlock_unlock(&hash->rwlock);
                return true;
            }
            i--;
        }
    }
    pthread_rwlock_unlock(&hash->rwlock);
    return false;
}

bool HashErase(Hash hash, void *const key) {
    if (pthread_rwlock_wrlock(&hash->rwlock)) {
        exit(-4);
    }
    long long flag = (long long) key % hash->capacity;
    long long doubleLength = hash->capacity - flag < flag ? hash->capacity - flag : flag;
    long long i = 0;
    while (i < doubleLength) {
        long long temp = flag + i;
        if (hash->table[temp].status && hash->table[temp].key == key) {
            hash->table[temp].status = false;
            --hash->size;
            pthread_rwlock_unlock(&hash->rwlock);
            return true;
        }
        temp = flag - i;
        if (hash->table[temp].status && hash->table[temp].key == key) {
            hash->table[temp].status = false;
            --hash->size;
            pthread_rwlock_unlock(&hash->rwlock);
            return true;
        }
        i++;
    }
    if (flag == i) {
        i = flag + i;
        while (i < hash->capacity) {
            if (hash->table[i].status && hash->table[i].key == key) {
                hash->table[i].status = false;
                --hash->size;
                pthread_rwlock_unlock(&hash->rwlock);
                return true;
            }
            i++;
        }
    } else {
        i = flag - i - 1;
        while (i >= 0) {
            if (hash->table[i].status && hash->table[i].key == key) {
                hash->table[i].status = false;
                --hash->size;
                pthread_rwlock_unlock(&hash->rwlock);
                return true;
            }
            i--;
        }
    }
    pthread_rwlock_unlock(&hash->rwlock);
    return false;
}

Array HashToArray(Hash hash) {
    if (pthread_rwlock_rdlock(&hash->rwlock) != 0) {
        exit(-4);
    }
    Array array;
    array.data = (void **) malloc(sizeof(void *) * hash->size);
    if (array.data == NULL) {
        array.size = 0;
    } else {
        array.size = 0;
        for (unsigned long long i = 0, size = hash->capacity; i < size; i++) {
            if (hash->table[i].status) {
                array.data[array.size++] = hash->table[i].value;
            }
        }
    }
    pthread_rwlock_unlock(&hash->rwlock);
    return array;
}