#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "DataStructure/HashList.h"
#include "DataStructure/Queue.h"
#include "FileData/MemoryToFile.h"
#define size 100
int main() {
    char i[size] = "sudsalfhaldsfhdsajkn";
    char j[size] = "SunWenli";
    File file = FileNew("sunwenli",size);
    FileWrite(file,(&j),1000);
    printf("%d\n",FileRead(file,(&i),1000));
    printf("%s\n",i);
    FileDestroy(file);
    return 0;
}