//
// Created by sunwenli on 2021/10/8.
//
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>
#include "user.h"

const char *fileName = "users";

long GetUserCount() {
    FILE *userFile;
    if ((userFile = fopen(fileName, "rb")) == 0) {
        return -1;
    }
    fseek(userFile, 0, SEEK_END);
    long temp = ftell(userFile);
    fclose(userFile);
    return temp / (long) sizeof(struct User);
}

struct User *GetUserByPlace(unsigned int place) {
    FILE *userFile;
    if ((userFile = fopen(fileName, "rb")) == 0) {
        return NULL;
    }
    if (place >= GetUserCount()) {
        return NULL;
    }
    struct User *user = (struct User *) malloc(sizeof(struct User));
    fseek(userFile, (long) sizeof(struct User) * place, SEEK_SET);
    fread(user, sizeof(struct User), 1, userFile);
    fclose(userFile);
    return user;
}

struct User *GetUserByUserName(char *username) {
    FILE *userFile;
    struct User *user = NULL;
    struct User temp;
    if ((userFile = fopen(fileName, "rb")) == 0) {
        return NULL;
    }
    for (unsigned int i = 0, count = GetUserCount(); i < count; i++) {
        fread(&temp, sizeof(struct User), 1, userFile);
        if (strcmp(username, temp.username) == 0) {
            user = (struct User *) malloc(sizeof(struct User));
            memcpy(user, &temp, sizeof(struct User));
            break;
        }
    }
    fclose(userFile);
    return user;
}


bool RemoveUserByPlace(unsigned int place) {
    FILE *userFile;
    FILE *tempFile;
    struct User temp;
    long count = GetUserCount();
    if(place >= count){
        return false;
    }
    if ((userFile = fopen(fileName, "rb")) == 0) {
        return false;
    }
    if ((tempFile = fopen("temp", "w")) == 0) {
        return false;
    }
    int i;
    for (i = 0; i < place; i++) {
        fread(&temp, sizeof(struct User), 1, userFile);
        fwrite(&temp, sizeof(struct User), 1, tempFile);
    }
    fread(&temp, sizeof(struct User), 1, userFile);
    for (; i < count-1; i++) {
        fread(&temp, sizeof(struct User), 1, userFile);
        fwrite(&temp, sizeof(struct User), 1, tempFile);
    }
    fclose(userFile);
    fclose(tempFile);
    remove(fileName);
    rename("temp",fileName);
    return true;
}

bool InsertUserByPlace(struct User *user,unsigned int place){
    FILE *userFile;
    FILE *tempFile;
    struct User temp;
    long count = GetUserCount();
    if(place >= count){
        return false;
    }
    if ((userFile = fopen(fileName, "rb")) == 0) {
        return false;
    }
    if ((tempFile = fopen("temp", "w")) == 0) {
        return false;
    }
    int i;
    for (i = 0; i < place; i++) {
        fread(&temp, sizeof(struct User), 1, userFile);
        fwrite(&temp, sizeof(struct User), 1, tempFile);
    }
    fwrite(user, sizeof(struct User), 1, tempFile);
    for (; i < count; i++) {
        fread(&temp, sizeof(struct User), 1, userFile);
        fwrite(&temp, sizeof(struct User), 1, tempFile);
    }
    fclose(userFile);
    fclose(tempFile);
    remove(fileName);
    rename("temp",fileName);
    return true;
}
bool SetUserByPlace(struct User *user,unsigned int place) {
    FILE *userFile;
    if ((userFile = fopen(fileName, "rb+")) == 0) {
        if ((userFile = fopen(fileName, "wb")) == 0) {
            return false;
        }
    }
    fseek(userFile, (long) sizeof(struct User) * place, SEEK_SET);
    fwrite(user, sizeof(struct User), 1, userFile);
    fclose(userFile);
    return true;
}
