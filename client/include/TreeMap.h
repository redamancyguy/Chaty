//
// Created by 15066 on 2021/9/11.
//


#ifndef TREEMAP_H
#define TREEMAP_H
#include<stdbool.h>
typedef struct TreeMap_* TreeMap;
TreeMap Tree_New();
bool Tree_ContainKey(TreeMap tree, void *key);
bool Tree_Insert(TreeMap tree, void *key, void *value);
bool Tree_SetOrInsert(TreeMap tree, void *key, void *value);
bool Tree_Set(TreeMap tree, void *key, void *value);
void *Tree_Get(TreeMap tree, void *key);
bool Tree_Delete(TreeMap tree, void *key);
void Tree_Destroy(TreeMap tree);
void Tree_Display(TreeMap tree);

#endif //TREEMAP_H
