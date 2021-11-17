//
// Created by sunwenli on 2021/11/15.
//
#include <stdio.h>
#include <malloc.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include "MemoryToFile.h"

struct FileInfo{
    pthread_rwlock_t rwlock;
    long long blockSize;
    FILE* file;
};
File FileNew(char *fileName,long long blockSize){
    File file = (File)malloc(sizeof(struct FileInfo));
    if(file == NULL){
        return NULL;
    }
    if(pthread_rwlock_init(&file->rwlock,NULL) !=0){
        free(file);
        return NULL;
    }
    if((file->file = fopen(fileName,"rbx+")) == NULL){
        if((file->file = fopen(fileName,"wb")) == NULL){
            free(file);
            return NULL;
        }
        fclose(file->file);
        if((file->file = fopen(fileName,"rbx+")) == NULL){
            free(file);
            return NULL;
        }
    }
    file->blockSize = blockSize;
    return file;
}
int FileWLock(File file){
    return pthread_rwlock_wrlock(&file->rwlock);
}
int FileRLock(File file){
    return pthread_rwlock_rdlock(&file->rwlock);
}
int FileUnLock(File file){
    return pthread_rwlock_unlock(&file->rwlock);
}
long long FileCount(File file){
    fseek(file->file,0,SEEK_END);
    return ftell(file->file)/file->blockSize;
}
bool FileWrite(File file,char* buffer,long long index){
    if(fseek(file->file,file->blockSize*index,SEEK_SET) !=0 ){
        return false;
    }
    return fwrite(buffer,file->blockSize,1,file->file) == 1;
}
bool FileRead(File file,char* buffer,long long index){
    if(fseek(file->file,file->blockSize*index,SEEK_SET) !=0 ){
        return false;
    }
    return fread(buffer,file->blockSize,1,file->file) == 1;
}
void FileDestroy(File file){
    fclose(file->file);
    free(file);
}