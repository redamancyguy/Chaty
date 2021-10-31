//
// Created by 15066 on 2021/9/14.
//
#include<pthread.h>
#include<string.h>
#include <stdlib.h>
#include "ArrayList.h"
struct ArrayList_{
    pthread_rwlock_t rwlock;
    void **Array;
    long long Size;
    long long unsigned Capacity;
};
typedef struct ArrayList_* ArrayList;

void *ArrayListGet(ArrayList array,unsigned long long index){
    if(pthread_rwlock_rdlock(&array->rwlock) != 0){
        exit(-4);
    }
    void *result = array->Array[index];
    pthread_rwlock_unlock(&array->rwlock);
    return result;
}

long long ArrayListSize(ArrayList array){
    if(pthread_rwlock_rdlock(&array->rwlock) != 0){
        exit(-4);
    }
    long long result = array->Size;
    pthread_rwlock_unlock(&array->rwlock);
    return result;
}

ArrayList ArrayListNew() {
    ArrayList array = (struct ArrayList_ *) malloc(sizeof(struct ArrayList_));
    if (array == NULL) {
        return NULL;
    }
    if(pthread_rwlock_init(&array->rwlock,NULL)!=0 ){
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
    pthread_rwlock_destroy(&array->rwlock);
    free(array->Array);
    free(array);
}

bool ArrayListSetCapacity(ArrayList array, unsigned long long capacity) {
    void *temp = realloc(array->Array, sizeof(void *) * capacity);
    if (temp == NULL) {
        pthread_rwlock_unlock(&array->rwlock);
        return false;
    }
    if (temp != array->Array) {
        array->Array = temp;
    }
    array->Capacity = capacity;
    return true;
}

bool ArrayListPushBack(ArrayList array, void *object) {
    if(pthread_rwlock_wrlock(&array->rwlock) != 0){
        exit(-4);
    }
    if (array->Size < array->Capacity) {
        array->Array[array->Size++] = object;
        pthread_rwlock_unlock(&array->rwlock);
        return true;
    } else {
        if (ArrayListSetCapacity(array, array->Capacity * 2)) {
            array->Array[array->Size++] = object;
            pthread_rwlock_unlock(&array->rwlock);
            return true;
        }
        pthread_rwlock_unlock(&array->rwlock);
        return false;
    }
}

bool ArrayListInsert(ArrayList array, unsigned long long index, void *object) {
    if(pthread_rwlock_wrlock(&array->rwlock) != 0){
        exit(-4);
    }
    if(array->Size > array->Capacity){
        if(!ArrayListSetCapacity(array,array->Capacity*2)){
            pthread_rwlock_unlock(&array->rwlock);
            return false;
        }
    }
    for (unsigned long long i = array->Size; i > index; i--) {
        array->Array[i] = array->Array[i - 1];
    }
    array->Array[index] = object;
    array->Size++;
    pthread_rwlock_unlock(&array->rwlock);
    return true;
}

bool ArrayListErase(ArrayList array, unsigned long long index) {
    if(pthread_rwlock_wrlock(&array->rwlock) != 0){
        exit(-4);
    }
    for (unsigned long long i = index; i < array->Size; i++) {
        array->Array[i] = array->Array[i + 1];
    }
    if(array->Size < array->Capacity/2 && array->Capacity > 1024){
        ArrayListSetCapacity(array,array->Capacity/2);
    }
    array->Size--;
    pthread_rwlock_unlock(&array->rwlock);
    return true;
}


bool ArrayListContain(ArrayList array, void *object) {
    if(pthread_rwlock_rdlock(&array->rwlock) != 0){
        exit(-4);
    }
    for (unsigned long long i = 0; i < array->Size; i++) {
        if(array->Array[i] == object){
            pthread_rwlock_unlock(&array->rwlock);
            return true;
        }
    }
    pthread_rwlock_unlock(&array->rwlock);
    return false;
}
Array ArrayListToArray(ArrayList arraylist){
    if(pthread_rwlock_wrlock(&arraylist->rwlock) != 0){
        exit(-4);
    }
    Array array;
    array.data = (void**) malloc(sizeof(void*)*arraylist->Size);
    if(array.data == NULL){
        array.size = 0;
    }else{
        array.size=arraylist->Size;
        memcpy(array.data,arraylist->Array,sizeof(void*)*array.size);
    }
    pthread_rwlock_unlock(&arraylist->rwlock);
    return array;
}