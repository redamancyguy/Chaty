//
// Created by sunwenli on 2021/10/8.
//
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "User.h"

struct UserDataBase_{
    pthread_rwlock_t rwlock;
    char fileName[32];
    FILE *userFile;
};
UserDataBase UserDataBaseOpen(char *fileName){
    UserDataBase dataBase = (UserDataBase)malloc(sizeof(struct UserDataBase_));
    if(dataBase == NULL){
        return NULL;
    }
    if(pthread_rwlock_init(&dataBase->rwlock,NULL)!=0){
        free(dataBase);
        return NULL;
    }
    strcpy(dataBase->fileName,fileName);
    FILE *userFile;
    if ((userFile = fopen(dataBase->fileName, "rb")) == 0) {
        if ((userFile = fopen(dataBase->fileName, "wb")) == 0) {
            return NULL;
        }
    }
    return dataBase;
}
void UserDataBaseClose(UserDataBase dataBase){
    pthread_rwlock_destroy(&dataBase->rwlock);
    free(dataBase);
}

long GetUserCount(UserDataBase dataBase) {
    FILE *userFile;
    if ((userFile = fopen(dataBase->fileName, "rb")) == 0) {
        return 0;
    }
    fseek(userFile, 0, SEEK_END);
    long temp = ftell(userFile);
    fclose(userFile);
    return temp / (long) sizeof(struct User);
}

long GetUserPlaceByUsername(UserDataBase dataBase,char *username) {
    if(pthread_rwlock_rdlock(&dataBase->rwlock) !=0){
        exit(-4);
    }
    FILE *userFile;
    if ((userFile = fopen(dataBase->fileName, "rb")) == 0) {
        if ((userFile = fopen(dataBase->fileName, "wb")) == 0) {
            pthread_rwlock_unlock(&dataBase->rwlock);
            return -1;
        }
    }
    long right = GetUserCount(dataBase);
    long left = 0;
    long temp;
    while (left <= right) {
        temp = (left + right) / 2;
        fseek(userFile, (long) (sizeof(struct User) * temp), SEEK_SET);
        struct User userBuf;
        if (fread(&userBuf, sizeof(struct User), 1, userFile) < 0) {
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
    fclose(userFile);
    pthread_rwlock_unlock(&dataBase->rwlock);
    return temp;
}

long GetUserReadyPlaceByUsername(UserDataBase dataBase,char *username) {
    if(pthread_rwlock_wrlock(&dataBase->rwlock) !=0){
        exit(-4);
    }
    FILE *userFile;
    if ((userFile = fopen(dataBase->fileName, "rb")) == 0) {
        if ((userFile = fopen(dataBase->fileName, "wb")) == 0) {
            pthread_rwlock_unlock(&dataBase->rwlock);
            return -1;
        }
    }
    long count = GetUserCount(dataBase);
    long right = count;
    long left = 0;
    long temp;
    while (left <= right) {
        temp = (left + right) / 2;
        fseek(userFile, (long) (sizeof(struct User) * temp), SEEK_SET);
        struct User userBuf;
        if (fread(&userBuf, sizeof(struct User), 1, userFile) < 0) {
            break;
        }
        int result = strcmp(username, userBuf.username);
        if (result < 0) {
            right = temp - 1;
        } else if (result > 0) {
            left = temp + 1;
        } else {
            fclose(userFile);
            pthread_rwlock_unlock(&dataBase->rwlock);
            return -1;
        }
    }
    fseek(userFile, (long) (sizeof(struct User) * temp), SEEK_SET);
    struct User userBuf;
    while (temp > 0) {
        fseek(userFile, (long) (sizeof(struct User) * temp), SEEK_SET);
        if (fread(&userBuf, sizeof(struct User), 1, userFile) < 0) {
            goto END;
        }
        if (strcmp(username, userBuf.username) > 0) {
            break;
        }
        temp--;
    }
    while (temp < count) {
        fseek(userFile, (long) (sizeof(struct User) * temp), SEEK_SET);
        if (fread(&userBuf, sizeof(struct User), 1, userFile) < 0) {
            goto END;
        }
        if (strcmp(username, userBuf.username) < 0) {
            break;
        }
        temp++;
    }
    END:
    fclose(userFile);
    pthread_rwlock_unlock(&dataBase->rwlock);
    return temp;
}

long GetUserByPlace(UserDataBase dataBase,struct User *user, unsigned long place) {
    if(pthread_rwlock_rdlock(&dataBase->rwlock) != 0){
        exit(-4);
    }
    FILE *userFile;
    if ((userFile = fopen(dataBase->fileName, "rb")) == 0) {
        pthread_rwlock_unlock(&dataBase->rwlock);
        return -1;
    }
    fseek(userFile, (long) (sizeof(struct User) * place), SEEK_SET);
    if (fread(user, sizeof(struct User), 1, userFile) < 0) {
        pthread_rwlock_unlock(&dataBase->rwlock);
        return -1;
    }
    pthread_rwlock_unlock(&dataBase->rwlock);
    return (long) place;
}

void Show(UserDataBase dataBase) {
    FILE *userFile;
    struct User temp;
    if ((userFile = fopen(dataBase->fileName, "rb")) == 0) {
        return;
    }
    while (fread(&temp, sizeof(struct User), 1, userFile) > 0) {
        printf("Id : %20ld\tEmail : %20s\tUsername: %20s\tPassword : %20s\n", temp.id, temp.email, temp.username,
               temp.password);
    }
    fclose(userFile);
}

long RemoveUserByPlace(UserDataBase dataBase,unsigned long place) {
    if(pthread_rwlock_wrlock(&dataBase->rwlock) !=0){
        exit(-4);
    }
    FILE *writeFile;
    FILE *readFile;
    long count = GetUserCount(dataBase);
    if (place >= count) {
        pthread_rwlock_unlock(&dataBase->rwlock);
        return -1;
    }
    if ((readFile = fopen(dataBase->fileName, "rb")) == 0) {
        pthread_rwlock_unlock(&dataBase->rwlock);
        return -1;
    }
    if ((writeFile = fopen(dataBase->fileName, "rb+")) == 0) {
        pthread_rwlock_unlock(&dataBase->rwlock);
        return -1;
    }
    struct User temp;
    fseek(readFile, (long) (sizeof(struct User) * place), SEEK_SET);
    fseek(writeFile, (long) (sizeof(struct User) * place), SEEK_SET);
    if (fread(&temp, sizeof(struct User), 1, readFile) < 0) {
        goto ERROR;
    }
    while (fread(&temp, sizeof(struct User), 1, readFile) > 0) {
        fwrite(&temp, sizeof(struct User), 1, writeFile);
    }
    fclose(readFile);
    fclose(writeFile);
    if (truncate(dataBase->fileName, (long) (sizeof(struct User) * (count - 1))) == -1) {
        pthread_rwlock_unlock(&dataBase->rwlock);
        return -1;
    }
    pthread_rwlock_unlock(&dataBase->rwlock);
    return (long) place;
    ERROR:
    fclose(readFile);
    fclose(writeFile);
    pthread_rwlock_unlock(&dataBase->rwlock);
    return -1;
}

long InsertUserByPlace(UserDataBase dataBase,struct User *user, unsigned long place) {
    if(pthread_rwlock_wrlock(&dataBase->rwlock) !=0){
        exit(-4);
    }
    FILE *writeFile;
    FILE *readFile;
    long count = GetUserCount(dataBase);
    if (place > count) {
        pthread_rwlock_unlock(&dataBase->rwlock);
        return -1;
    }
    if ((readFile = fopen(dataBase->fileName, "rb+")) == 0) {
        if ((readFile = fopen(dataBase->fileName, "w")) == 0) {
            pthread_rwlock_unlock(&dataBase->rwlock);
            return -1;
        }
        fclose(readFile);
        readFile = fopen(dataBase->fileName, "rb+");
    }
    if ((writeFile = fopen(dataBase->fileName, "rb+")) == 0) {
        pthread_rwlock_unlock(&dataBase->rwlock);
        return -1;
    }
    struct User temp;
    fseek(readFile, (long) (sizeof(struct User) * place), SEEK_SET);
    fseek(writeFile, (long) (sizeof(struct User) * place), SEEK_SET);
    if (fwrite(user, sizeof(struct User), 1, writeFile) < 0) {
        goto ERROR;
    }
    while (fread(&temp, sizeof(struct User), 1, readFile) > 0) {
        fwrite(&temp, sizeof(struct User), 1, writeFile);
    }
    fclose(readFile);
    fclose(writeFile);
    pthread_rwlock_unlock(&dataBase->rwlock);
    return (long) place;
    ERROR:
    fclose(readFile);
    fclose(writeFile);
    pthread_rwlock_unlock(&dataBase->rwlock);
    return -1;
}

int CLearUsers(UserDataBase dataBase) {
    if(pthread_rwlock_wrlock(&dataBase->rwlock) !=0){
        exit(-4);
    }
    FILE *tempFile;
    if ((tempFile = fopen(dataBase->fileName, "wb")) == 0) {
        pthread_rwlock_unlock(&dataBase->rwlock);
        return -1;
    }
    fclose(tempFile);
    pthread_rwlock_unlock(&dataBase->rwlock);
    return 1;
}

long SetUserByPlace(UserDataBase dataBase,struct User *user, unsigned long place) {
    if(pthread_rwlock_wrlock(&dataBase->rwlock) != 0){
        exit(-4);
    }
    FILE *userFile;
    if ((userFile = fopen(dataBase->fileName, "rb+")) == 0) {
        if ((userFile = fopen(dataBase->fileName, "wb")) == 0) {
            pthread_rwlock_unlock(&dataBase->rwlock);
            return -1;
        }
    }
    fseek(userFile, (long) (sizeof(struct User) * place), SEEK_SET);
    fwrite(user, sizeof(struct User), 1, userFile);
    fclose(userFile);
    pthread_rwlock_unlock(&dataBase->rwlock);
    return (long) place;
}
