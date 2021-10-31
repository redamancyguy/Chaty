//
// Created by sunwenli on 2021/10/8.
//
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "User.h"

pthread_rwlock_t userDatabaseRwlock;
char *userDatabaseFileName = "users";
FILE *userDatabaseFile;
bool UserDataBaseOpen(){
    if(pthread_rwlock_init(&userDatabaseRwlock,NULL)!=0){
        exit(-4);
    }
    if ((userDatabaseFile = fopen(userDatabaseFileName, "rb+")) == 0) {
        if ((userDatabaseFile = fopen(userDatabaseFileName, "wb")) == 0) {
            return false;
        }
    }
    return true;
}
void UserDataBaseClose(){
    pthread_rwlock_destroy(&userDatabaseRwlock);
    fclose(userDatabaseFile);
}

long GetUserCount() {
    fseek(userDatabaseFile, 0, SEEK_END);
    long temp = ftell(userDatabaseFile);
    return temp / (long) sizeof(struct User);
}

long GetUserPlaceByUsername(char *username) {
    if(pthread_rwlock_rdlock(&userDatabaseRwlock) !=0){
        exit(-4);
    }
    long right = GetUserCount();
    long left = 0;
    long temp;
    while (left <= right) {
        temp = (left + right) / 2;
        fseek(userDatabaseFile, (long) (sizeof(struct User) * temp), SEEK_SET);
        struct User userBuf;
        if (fread(&userBuf, sizeof(struct User), 1, userDatabaseFile) < 0) {
            break;
        }
        int result = strcmp(username, userBuf.username);
        if (result < 0) {
            right = temp - 1;
        } else if (result > 0) {
            left = temp + 1;
        } else {
            goto END;
        }
    }
    temp = -1;
    END:
    pthread_rwlock_unlock(&userDatabaseRwlock);
    return temp;
}

long GetUserReadyPlaceByUsername(char *username) {
    if(pthread_rwlock_wrlock(&userDatabaseRwlock) !=0){
        exit(-4);
    }
    long count = GetUserCount();
    long right = count;
    long left = 0;
    long temp;
    while (left <= right) {
        temp = (left + right) / 2;
        fseek(userDatabaseFile, (long) (sizeof(struct User) * temp), SEEK_SET);
        struct User userBuf;
        if (fread(&userBuf, sizeof(struct User), 1,userDatabaseFile) < 0) {
            break;
        }
        int result = strcmp(username, userBuf.username);
        if (result < 0) {
            right = temp - 1;
        } else if (result > 0) {
            left = temp + 1;
        } else {
            pthread_rwlock_unlock(&userDatabaseRwlock);
            return -1;
        }
    }
    fseek(userDatabaseFile, (long) (sizeof(struct User) * temp), SEEK_SET);
    struct User userBuf;
    while (temp > 0) {
        fseek(userDatabaseFile, (long) (sizeof(struct User) * temp), SEEK_SET);
        if (fread(&userBuf, sizeof(struct User), 1, userDatabaseFile) < 0) {
            goto END;
        }
        if (strcmp(username, userBuf.username) > 0) {
            break;
        }
        temp--;
    }
    while (temp < count) {
        fseek(userDatabaseFile, (long) (sizeof(struct User) * temp), SEEK_SET);
        if (fread(&userBuf, sizeof(struct User), 1, userDatabaseFile) < 0) {
            goto END;
        }
        if (strcmp(username, userBuf.username) < 0) {
            break;
        }
        temp++;
    }
    END:
    pthread_rwlock_unlock(&userDatabaseRwlock);
    return temp;
}

long GetUserByPlace(struct User *user, unsigned long place) {
    if(pthread_rwlock_rdlock(&userDatabaseRwlock) != 0){
        exit(-4);
    }
    fseek(userDatabaseFile, (long) (sizeof(struct User) * place), SEEK_SET);
    if (fread(user, sizeof(struct User), 1, userDatabaseFile) < 0) {
        pthread_rwlock_unlock(&userDatabaseRwlock);
        return -1;
    }
    pthread_rwlock_unlock(&userDatabaseRwlock);
    return (long) place;
}

void Show() {
    struct User temp;
    fseek(userDatabaseFile,0,SEEK_SET);
    while (fread(&temp, sizeof(struct User), 1, userDatabaseFile) > 0) {
        printf("Id : %20ld\tEmail : %20s\tUsername: %20s\tPassword : %20s\n", temp.id, temp.email, temp.username,
               temp.password);
    }
}

long RemoveUserByPlace(unsigned long place) {
    if(pthread_rwlock_wrlock(&userDatabaseRwlock) !=0){
        exit(-4);
    }
    FILE *readFile;
    long count = GetUserCount();
    if (place >= count) {
        pthread_rwlock_unlock(&userDatabaseRwlock);
        return -1;
    }
    if ((readFile = fopen(userDatabaseFileName, "rb")) == 0) {
        pthread_rwlock_unlock(&userDatabaseRwlock);
        return -1;
    }
    struct User temp;
    fseek(readFile, (long) (sizeof(struct User) * place), SEEK_SET);
    fseek(userDatabaseFile, (long) (sizeof(struct User) * place), SEEK_SET);
    if (fread(&temp, sizeof(struct User), 1, readFile) < 0) {
        goto ERROR;
    }
    while (fread(&temp, sizeof(struct User), 1, readFile) > 0) {
        fwrite(&temp, sizeof(struct User), 1, userDatabaseFile);
    }
    fclose(readFile);
    if (truncate(userDatabaseFileName, (long) (sizeof(struct User) * (count - 1))) == -1) {
        pthread_rwlock_unlock(&userDatabaseRwlock);
        return -1;
    }
    pthread_rwlock_unlock(&userDatabaseRwlock);
    return (long) place;
    ERROR:
    fclose(readFile);
    pthread_rwlock_unlock(&userDatabaseRwlock);
    return -1;
}

long InsertUserByPlace(struct User *user, unsigned long place) {
    if(pthread_rwlock_wrlock(&userDatabaseRwlock) !=0){
        exit(-4);
    }
    FILE *readFile;
    long count = GetUserCount();
    if (place > count) {
        pthread_rwlock_unlock(&userDatabaseRwlock);
        return -1;
    }
    if ((readFile = fopen(userDatabaseFileName, "rb+")) == 0) {
        if ((readFile = fopen(userDatabaseFileName, "w")) == 0) {
            pthread_rwlock_unlock(&userDatabaseRwlock);
            return -1;
        }
        fclose(readFile);
        readFile = fopen(userDatabaseFileName, "rb+");
    }
    struct User temp;
    fseek(readFile, (long) (sizeof(struct User) * place), SEEK_SET);
    fseek(userDatabaseFile, (long) (sizeof(struct User) * place), SEEK_SET);
    if (fwrite(user, sizeof(struct User), 1, userDatabaseFile) < 0) {
        goto ERROR;
    }
    while (fread(&temp, sizeof(struct User), 1, readFile) > 0) {
        fwrite(&temp, sizeof(struct User), 1, userDatabaseFile);
    }
    fclose(readFile);
    pthread_rwlock_unlock(&userDatabaseRwlock);
    return (long) place;
    ERROR:
    fclose(readFile);
    pthread_rwlock_unlock(&userDatabaseRwlock);
    return -1;
}

long SetUserByPlace(struct User *user, unsigned long place) {
    if(pthread_rwlock_wrlock(&userDatabaseRwlock) != 0){
        exit(-4);
    }
    FILE *userFile;
    if ((userFile = fopen(userDatabaseFileName, "rb+")) == 0) {
        if ((userFile = fopen(userDatabaseFileName, "wb")) == 0) {
            pthread_rwlock_unlock(&userDatabaseRwlock);
            return -1;
        }
    }
    fseek(userFile, (long) (sizeof(struct User) * place), SEEK_SET);
    fwrite(user, sizeof(struct User), 1, userFile);
    fclose(userFile);
    pthread_rwlock_unlock(&userDatabaseRwlock);
    return (long) place;
}
