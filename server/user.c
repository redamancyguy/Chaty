//
// Created by sunwenli on 2021/10/8.
//
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "user.h"
const char *fileName = "users";
long GetUserCount() {
    FILE *userFile;
    if ((userFile = fopen(fileName, "rb")) == 0) {
        return -1;
    }
    fseek(userFile,0,SEEK_END);
    long temp = ftell(userFile);
    fclose(userFile);
    return temp/(long)sizeof(struct User);
}
struct User *GetUserById(unsigned int id) {
    FILE *userFile;
    if ((userFile = fopen(fileName, "rb")) == 0) {
        return NULL;
    }
    if(id>=GetUserCount()){
        return NULL;
    }
    struct User *user = (struct User*)malloc(sizeof(struct User));
    fseek(userFile, (long)sizeof(struct User) * id, SEEK_SET);
    fread(user, sizeof(struct User), 1, userFile);
    fseek(userFile, 0, SEEK_END);
    fclose(userFile);
    return user;
}

struct User *GetUserByNickName(char *nickname) {
    FILE *userFile;
    struct User *user=NULL;
    struct User temp;
    if ((userFile = fopen(fileName, "rb")) == 0) {
        return NULL;
    }
    for(unsigned int i=0,count = GetUserCount();i<count;i++){
        fread(&temp, sizeof(struct User), 1, userFile);
        if(strcmp(nickname,temp.nickname) == 0){
            user = (struct User*)malloc(sizeof(struct User));
            memcpy(user,&temp,sizeof(struct User));
            break;
        }
    }
    fclose(userFile);
    return user;
}

bool SetUserById(struct User *user) {
    FILE *userFile;
    if ((userFile = fopen(fileName, "rb+")) == 0) {
        if ((userFile = fopen(fileName, "wb")) == 0) {
            return false;
        }
    }
    fseek(userFile, (long)sizeof(struct User) * user->id, SEEK_SET);
    fwrite(user, sizeof(struct User), 1, userFile);
    fclose(userFile);
    return true;
}
