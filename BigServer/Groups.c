//
// Created by sunwenli on 2021/11/16.
//
#include <malloc.h>
#include "DataStructure/HashList.h"
#include "DataStructure/Queue.h"
#include "Groups.h"


struct Groups_{
    HashList groups[256];
};

Groups GroupsNew(){
    Groups groups = malloc(sizeof(struct Groups_));
    if(groups == NULL){
        return NULL;
    }
    for(int i=0;i<256;i++){
        if((groups->groups[i] = HashListNew(1024)) == NULL){
            for(int j=0;j<i;j++){
                HashListDestroy(groups->groups[j]);
            }
            return NULL;
        }
    }
    return groups;
}

Queue GroupsInsert(Groups groups,unsigned long long id){
    unsigned char port = id;
    Queue queue = QueueNew();
    if(queue == NULL){
        return NULL;
    }
    if(!HashListInsert(groups->groups[port],(void*)(id>>8),queue)){
        QueueDestroy(queue);
        return NULL;
    }
    return queue;
}
Queue GroupsGet(Groups groups,unsigned long long id){
    unsigned char port = id;
    return HashListGet(groups->groups[port],(void*)(id>>8));
}


bool GroupsErase(Groups groups,unsigned long long id){
    unsigned char port = id;
    Queue queue = HashListGet(groups->groups[port],(void*)(id>>8));
    QueueDestroy(queue);
    return HashListErase(groups->groups[port],(void*)(id>>8));
}


void GroupsDestroy(Groups groups){
    for(int i=0;i<256;i++){
        Array ar = HashListToArray(groups->groups[i]);
        for(int j=0;j<ar.size;j++){
            QueueDestroy((Queue)ar.data[j]);
        }
        free(ar.data);
        HashListDestroy(groups->groups[i]);
    }
    free(groups);
}

