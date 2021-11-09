////
//// Created by sunwenli on 2021/10/8.
////
//#include <pthread.h>
//#include <stdio.h>
//#include <string.h>
//#include <unistd.h>
//#include <stdlib.h>
//#include "User.h"
//#include "../DataStructure/Tree.h"
//
//pthread_rwlock_t rwlock;
//char *fileName = "users";
//FILE *file;
//Tree users;
//unsigned long long idCount;
//
//bool UserDataBaseOpen() {
//    if ((file = fopen(fileName, "rb+")) == 0) {
//        if ((file = fopen(fileName, "w")) == 0) {
//            return false;
//        }
//        fclose(file);
//        if ((file = fopen(fileName, "rb+")) == 0) {
//            return false;
//        }
//    }
//    if (pthread_rwlock_init(&rwlock, NULL) != 0) {
//        fclose(file);
//        return false;
//    }
//    users = TreeNew();
//    if (users == NULL) {
//        pthread_rwlock_destroy(&rwlock);
//        fclose(file);
//        return false;
//    }
//    idCount = 0;
//    return true;
//}
//
//void UserDataBaseClose() {
//    pthread_rwlock_destroy(&rwlock);
//    fclose(file);
//    TreeDestroy(users);
//}
//
//bool IsContainUserId(unsigned long long id) {
//    if (pthread_rwlock_rdlock(&rwlock) != 0) {
//        return false;
//    }
//    bool result = TreeGet(users, (void *) id) != NULL;
//    pthread_rwlock_unlock(&rwlock);
//    return result;
//}
//
//bool GetUserById(struct User *user, unsigned long long id) {
//    if (pthread_rwlock_rdlock(&rwlock) != 0) {
//        return false;
//    }
//    long long index = (long long) TreeGet(users, (void *) id);
//    if (index == 0) {
//        pthread_rwlock_unlock(&rwlock);
//        return false;
//    }
//    fseek(file, (long) (index * sizeof(struct User)), SEEK_SET);
//    if (fread(user, sizeof(struct User), 1, file) < 0) {
//        pthread_rwlock_unlock(&rwlock);
//        return false;
//    }
//    pthread_rwlock_unlock(&rwlock);
//    return true;
//}
//
//bool SetUserById(struct User *user, unsigned long long id) {
//    if (pthread_rwlock_wrlock(&rwlock) != 0) {
//        return false;
//    }
//    long long index = (long long)TreeGet(users,(void*)id);
//    if (index == 0){
//        pthread_rwlock_unlock(&rwlock);
//        return false;
//    }
//    fseek(file,(long)sizeof(struct User)*index,SEEK_SET);
//    if(fwrite(user,sizeof(struct User),1,file) < 0){
//        pthread_rwlock_unlock(&rwlock);
//        return false;
//    }
//    pthread_rwlock_unlock(&rwlock);
//    return true;
//}
//
//bool RemoveUserById(unsigned long long id){
//    if (pthread_rwlock_wrlock(&rwlock) != 0) {
//        return false;
//    }
//    long long index = (long long)TreeGet(users,(void*)id);
//    if (index == 0){
//        pthread_rwlock_unlock(&rwlock);
//        return false;
//    }
//    return TreeDelete(users,(void*)id);
//}
//
//bool InsertUser(struct User *user){
//    TreeInsert(users,++idCount,++idCount);
//}
//
//void Show();