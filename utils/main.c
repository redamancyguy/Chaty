#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include "DataStructure/HashList.h"
#include "DataStructure/Queue.h"
#include "FileData/MemoryToFile.h"
#define size 100
int main() {
    time_t t = clock();
    pthread_rwlock_t rwlock;
    pthread_rwlock_init(&rwlock,NULL);
    for(int i=0;i<65536*5533;i++){
//        pthread_rwlock_wrlock(&rwlock);
        int *j = malloc(5);
        free(j);
//        pthread_rwlock_unlock(&rwlock);
    }
    printf("%lld\n",clock()-t);
    //633409
    //8574533
    //10890205
    //3347741
    return 0;
}