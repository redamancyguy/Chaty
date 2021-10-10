//
// Created by sunwenli on 2021/10/8.
//
#include <stdio.h>
#include <string.h>
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

long GetUserByPlace(struct User *user, unsigned long place) {
    FILE *userFile;
    if ((userFile = fopen(fileName, "rb")) == 0) {
        return -1;
    }
    if (place >= GetUserCount()) {
        return -1;
    }
    fseek(userFile, (long) (sizeof(struct User) * place), SEEK_SET);
    fread(user, sizeof(struct User), 1, userFile);
    fclose(userFile);
    return (long) place;
}
void Show() {
    FILE *userFile;
    struct User temp;
    if ((userFile = fopen(fileName, "rb")) == 0) {
        return;
    }
    for (long i = 0, count = GetUserCount(); i < count; i++) {
        fread(&temp, sizeof(struct User), 1, userFile);
        printf("%ld %s %s\n",temp.id,temp.username,temp.password);
    }
    fclose(userFile);
}

long GetUserByUserName(struct User *user, char *username) {
    FILE *userFile;
    struct User temp;
    if ((userFile = fopen(fileName, "rb")) == 0) {
        return -1;
    }
    for (long i = 0, count = GetUserCount(); i < count; i++) {
        fread(&temp, sizeof(struct User), 1, userFile);
        if (strcmp(username, temp.username) == 0) {
            memcpy(user, &temp, sizeof(struct User));
            fclose(userFile);
            return i;
        }
    }
    fclose(userFile);
    return -1;
}


long RemoveUserByPlace(unsigned long place) {
    FILE *userFile;
    FILE *tempFile;
    struct User temp;
    long count = GetUserCount();
    if (place >= count) {
        return -1;
    }
    if ((userFile = fopen(fileName, "rb")) == 0) {
        return -1;
    }
    if ((tempFile = fopen("temp", "w")) == 0) {
        return -1;
    }
    long i;
    for (i = 0; i < place; i++) {
        fread(&temp, sizeof(struct User), 1, userFile);
        fwrite(&temp, sizeof(struct User), 1, tempFile);
    }
    fread(&temp, sizeof(struct User), 1, userFile);
    for (; i < count - 1; i++) {
        fread(&temp, sizeof(struct User), 1, userFile);
        fwrite(&temp, sizeof(struct User), 1, tempFile);
    }
    fclose(userFile);
    fclose(tempFile);
    remove(fileName);
    rename("temp", fileName);
    return (long) place;
}

long InsertUserByPlace(struct User *user, unsigned long place) {
    FILE *userFile;
    FILE *tempFile;
    struct User temp;
    long count = GetUserCount();
    if (place > count) {
        return -1;
    }
    if ((userFile = fopen(fileName, "rb")) == 0) {
        return -1;
    }
    if ((tempFile = fopen("temp", "w")) == 0) {
        return -1;
    }
    long i;
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
    rename("temp", fileName);
    return (long) place;
}

int CLearUsers(){
    FILE *tempFile;
    if ((tempFile = fopen(fileName, "w")) == 0) {
        return -1;
    }
    fclose(tempFile);
    return 1;
}

long SetUserByPlace(struct User *user, unsigned long place) {
    FILE *userFile;
    if ((userFile = fopen(fileName, "rb+")) == 0) {
        if ((userFile = fopen(fileName, "wb")) == 0) {
            return -1;
        }
    }
    fseek(userFile, (long) (sizeof(struct User) * place), SEEK_SET);
    fwrite(user, sizeof(struct User), 1, userFile);
    fclose(userFile);
    return (long)place;
}
