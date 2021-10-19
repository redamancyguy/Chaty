//
// Created by sunwenli on 2021/10/8.
//
#include <stdio.h>
#include <string.h>
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

long GetUserPlaceByUsername(char *username) {
    FILE *userFile;
    if ((userFile = fopen(fileName, "rb")) == 0) {
        if ((userFile = fopen(fileName, "wb")) == 0) {
            return -1;
        }
    }
    long right = GetUserCount();
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
    return temp;
}

long GetUserReadyPlaceByUsername(char *username) {
    FILE *userFile;
    if ((userFile = fopen(fileName, "rb")) == 0) {
        if ((userFile = fopen(fileName, "wb")) == 0) {
            return -1;
        }
    }
    long count = GetUserCount();
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
            goto END;
        }
    }
    struct User userBuf;
    while (strcmp(username, userBuf.username) < 0 && temp > 0 &&
           fread(&userBuf, sizeof(struct User), 1, userFile) > 0) {
        temp--;
        fseek(userFile, (long) (sizeof(struct User) * temp), SEEK_SET);
    }
    while (strcmp(username, userBuf.username) > 0 && temp < count &&
           fread(&userBuf, sizeof(struct User), 1, userFile) > 0) {
        temp++;
    }
    END:
    fclose(userFile);
    return temp;
}

long GetUserByPlace(struct User *user, unsigned long place) {
    FILE *userFile;
    if ((userFile = fopen(fileName, "rb")) == 0) {
        return -1;
    }
    fseek(userFile, (long) (sizeof(struct User) * place), SEEK_SET);
    if (fread(user, sizeof(struct User), 1, userFile) < 0) {
        return -1;
    }
    return (long) place;
}

void Show() {
    FILE *userFile;
    struct User temp;
    if ((userFile = fopen(fileName, "rb")) == 0) {
        return;
    }
    while (fread(&temp, sizeof(struct User), 1, userFile) > 0) {
        printf("Id : %20ld\tEmail : %20s\tUsername: %20s\tPassword : %20s\n", temp.id, temp.email, temp.username,
               temp.password);
    }
    fclose(userFile);
}

long RemoveUserByPlace(unsigned long place) {
    FILE *writeFile;
    FILE *readFile;
    long count = GetUserCount();
    if (place >= count) {
        return -1;
    }
    if ((readFile = fopen(fileName, "rb")) == 0) {
        return -1;
    }
    if ((writeFile = fopen(fileName, "rb+")) == 0) {
        return -1;
    }
    struct User temp;
    fseek(readFile, (long)(sizeof(struct User) * place), SEEK_SET);
    fseek(writeFile, (long)(sizeof(struct User) * place), SEEK_SET);
    if (fread(&temp, sizeof(struct User), 1, readFile) < 0) {
        goto ERROR;
    }
    while(fread(&temp, sizeof(struct User), 1, readFile) > 0){
        fwrite(&temp, sizeof(struct User), 1, writeFile);
    }
    fclose(readFile);
    fclose(writeFile);
    if(truncate(fileName, (long)(sizeof(struct User) * (count - 1))) == -1){
        return -1;
    }
    return (long) place;
    ERROR:
    fclose(readFile);
    fclose(writeFile);
    return -1;
}

long InsertUserByPlace(struct User *user, unsigned long place) {
    FILE *writeFile;
    FILE *readFile;
    long count = GetUserCount();
    if (place > count) {
        return -1;
    }
    if ((readFile = fopen(fileName, "rb")) == 0) {
        return -1;
    }
    if ((writeFile = fopen(fileName, "rb+")) == 0) {
        return -1;
    }
    struct User temp;
    fseek(readFile, (long)(sizeof(struct User) * place), SEEK_SET);
    fseek(writeFile, (long)(sizeof(struct User) * place), SEEK_SET);
    if (fwrite(user, sizeof(struct User), 1, writeFile) < 0) {
        goto ERROR;
    }
    while(fread(&temp, sizeof(struct User), 1, readFile) > 0){
        fwrite(&temp, sizeof(struct User), 1, writeFile);
    }
    fclose(readFile);
    fclose(writeFile);
    return (long) place;
    ERROR:
    fclose(readFile);
    fclose(writeFile);
    return -1;
}

int CLearUsers() {
    FILE *tempFile;
    if ((tempFile = fopen(fileName, "wb")) == 0) {
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
    return (long) place;
}
