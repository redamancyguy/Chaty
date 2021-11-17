//
// Created by sunwenli on 2021/11/16.
//
#include <malloc.h>
#include <string.h>
#include "User.h"
#include "FileData/MemoryToFile.h"

struct Users_ {
    unsigned long long count;
    File file;
    File deleted;
};

Users UsersOpen() {
    Users users = (Users) malloc(sizeof(struct Users_));
    if (users == NULL) {
        return NULL;
    }
    if ((users->file = FileOpen("users", sizeof(struct User))) == NULL) {
        free(users);
        return NULL;
    }
    if ((users->deleted = FileOpen("usersDeleted", sizeof(unsigned long long))) == NULL) {
        FileClose(users->file);
        free(users);
        return NULL;
    }
    users->count = FileCount(users->file);
    return users;
}

unsigned long long UsersInsert(Users users, struct User *user) {
    long long fileCount = FileCount(users->deleted);
    if (fileCount == 0) {
        if (!FileWrite(users->file, user, (long long) users->count)) {
            return -1;
        }
        return users->count++;
    } else {
        long long userId;
        if(!FileRead(users->deleted, &userId, fileCount - 1)){
            return -1;
        }
        if (!FileWrite(users->file, user,userId)) {
            return -1;
        }
        FileTruncate(users->deleted,fileCount-1);
        return userId;
    }
}
bool UsersGet(Users users,struct User *user, unsigned long long userID) {
    return FileRead(users->file, user, (long long) userID);
}
bool UsersDelete(Users users, unsigned long long userID) {
    struct User user;
    FileWrite(users->deleted,&userID,FileCount(users->deleted));
    memset(&user, 0, sizeof(struct User));
    return FileWrite(users->file, &user, (long long) userID);
}

void UsersClose(Users users) {
    FileClose(users->file);
    free(users);
}