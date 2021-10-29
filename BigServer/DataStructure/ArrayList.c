//
// Created by 15066 on 2021/9/14.
//
#include<malloc.h>
#include<pthread.h>
#include "ArrayList.h"
struct ArrayList_{
    pthread_mutex_t mutex;
    void **Array;
    long long unsigned Size;
    long long unsigned Capacity;
};
typedef struct ArrayList_* ArrayList;

int ArrayListLock(ArrayList array){
    return pthread_mutex_lock(&array->mutex);
}
int ArrayListUnlock(ArrayList array){
    return pthread_mutex_unlock(&array->mutex);
}
int ArrayListTryLock(ArrayList array){
    return pthread_mutex_trylock(&array->mutex);
}


void *ArrayListGet(ArrayList array,unsigned long long index){
    return array->Array[index];
}

unsigned long long ArrayListSize(ArrayList array){
    return array->Size;
}

ArrayList ArrayListNew() {
    ArrayList array = (struct ArrayList_ *) malloc(sizeof(struct ArrayList_));
    if (array == NULL) {
        return NULL;
    }
    if(pthread_mutex_init(&array->mutex,NULL)!=0 ){
        free(array);
        return NULL;
    }
    array->Array = (void *) malloc(sizeof(void *) * 1024);
    if (array->Array == NULL) {
        free(array);
        return NULL;
    }
    array->Size = 0;
    array->Capacity = 1;
    return array;
}

void ArrayListDestroy(ArrayList array) {
    pthread_mutex_destroy(&array->mutex);
    free(array->Array);
    free(array);
}

bool ArrayListSetCapacity(ArrayList array, unsigned long long capacity) {
    void *temp = realloc(array->Array, sizeof(void *) * capacity);
    if (temp == NULL) {
        return false;
    }
    if (temp != array->Array) {
        array->Array = temp;
    }
    array->Capacity = capacity;
    return true;
}

bool ArrayListPushBack(ArrayList array, void *object) {
    if (array->Size < array->Capacity) {
        array->Array[array->Size++] = object;
        return true;
    } else {
        if (ArrayListSetCapacity(array, array->Capacity * 2)) {
            array->Array[array->Size++] = object;
            return true;
        }
        return false;
    }
}

bool ArrayListInsert(ArrayList array, unsigned long long index, void *object) {
    if(array->Size > array->Capacity){
        if(!ArrayListSetCapacity(array,array->Capacity*2)){
            return false;
        }
    }
    for (unsigned long long i = array->Size; i > index; i--) {
        array->Array[i] = array->Array[i - 1];
    }
    array->Array[index] = object;
    array->Size++;
    return true;
}

bool ArrayListErase(ArrayList array, unsigned long long index) {
    for (unsigned long long i = index; i < array->Size; i++) {
        array->Array[i] = array->Array[i + 1];
    }
    if(array->Size < array->Capacity/2 && array->Capacity > 1024){
        ArrayListSetCapacity(array,array->Capacity/2);
    }
    array->Size--;
    return true;
}


bool ArrayListContain(ArrayList array, void *object) {
    for (unsigned long long i = 0; i < array->Size; i++) {
        if(array->Array[i] == object){
            return true;
        }
    }
    return false;
}
