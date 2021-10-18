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

long GetUserPlaceByUsername(char *username) {
    FILE *userFile;
    if ((userFile = fopen(fileName, "rb")) == 0) {
        if ((userFile = fopen(fileName, "w")) == 0) {
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
    FILE *userFile = NULL;
    if ((userFile = fopen(fileName, "rb")) == 0) {
        if ((userFile = fopen(fileName, "w")) == 0) {
            return -1;
        }
    }
    long place = 0;
    struct User userBuf;
    while (fread(&userBuf, sizeof(struct User), 1, userFile) > 0) {
        if (strcmp(username, userBuf.username) < 0) {
            break;
        } else {
            place++;
        }
    }
    fclose(userFile);
    return place;
}

long GetUserByPlace(struct User *user, unsigned long place) {
    FILE *userFile;
    if ((userFile = fopen(fileName, "rb")) == 0) {
        return -1;
    }
    fseek(userFile, (long) (sizeof(struct User) * place), SEEK_SET);
    fread(user, sizeof(struct User), 1, userFile);
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

long GetUserByUserName(struct User *user, char *username) { //old
    FILE *userFile;
    struct User temp;
    if ((userFile = fopen(fileName, "rb")) == 0) {
        return -1;
    }
    long i = 0;
    while (fread(&temp, sizeof(struct User), 1, userFile) > 0) {
        if (strcmp(username, temp.username) == 0) {
            *user = temp;
            fclose(userFile);
            return i;
        }
        i++;
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
        fclose(userFile);
        return -1;
    }
    long i;
    for (i = 0; i < place; i++) {
        if (fread(&temp, sizeof(struct User), 1, userFile) < 0) {
            goto ERROR;
        }
        fwrite(&temp, sizeof(struct User), 1, tempFile);
    }
    if (fread(&temp, sizeof(struct User), 1, userFile) < 0) {
        goto ERROR;
    }
    for (; i < count - 1; i++) {
        if (fread(&temp, sizeof(struct User), 1, userFile) < 0) {
            goto ERROR;
        }
        fwrite(&temp, sizeof(struct User), 1, tempFile);
    }
    fclose(userFile);
    fclose(tempFile);
    remove(fileName);
    rename("temp", fileName);
    return (long) place;
    ERROR:
    fclose(tempFile);
    fclose(userFile);
    return -1;
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
        if ((userFile = fopen(fileName, "w")) == 0) {
            return -1;
        }
    }
    if ((tempFile = fopen("temp", "w")) == 0) {
        fclose(userFile);
        return -1;
    }
    long i;
    for (i = 0; i < place; i++) {
        if (fread(&temp, sizeof(struct User), 1, userFile) < 0) {
            goto ERROR;
        }
        fwrite(&temp, sizeof(struct User), 1, tempFile);
    }
    fwrite(user, sizeof(struct User), 1, tempFile);
    for (; i < count; i++) {
        if (fread(&temp, sizeof(struct User), 1, userFile) < 0) {
            goto ERROR;
        }
        fwrite(&temp, sizeof(struct User), 1, tempFile);
    }
    fclose(userFile);
    fclose(tempFile);
    remove(fileName);
    rename("temp", fileName);
    return (long) place;
    ERROR:
    fclose(userFile);
    fclose(tempFile);
    return -1;
}

int CLearUsers() {
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
    return (long) place;
}
