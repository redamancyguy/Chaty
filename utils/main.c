#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "DataStructure/HashList.h"
#include "DataStructure/Queue.h"
long long unsigned* randUnsignedLong(long long unsigned int length) { // remember to free the array
    long long unsigned* a = (long long unsigned*)malloc(sizeof(long long unsigned) * length);
    if (a == NULL) {
        return NULL;
    }
    for (long long unsigned i = 0; i < length; i++) {
        a[i] = i;
    }
    srand((unsigned int)time(NULL));
    long long unsigned temp;
    while (--length) {
        temp = rand() % (length + 1);
        long long unsigned t = a[length];
        a[length] = a[temp];
        a[temp] = t;
    }
    return a;
}
int main() {
    for(int ii=0;ii<10000;ii++){
        int num = 163840;
        Queue q = QueueNew();
        long unsigned *a = randUnsignedLong(num);
        for(int i=0;i<num;i++){
            QueuePush(q,a[i]);
        }
        for(int i=0;i<num;i++){
            if(QueueFront(q) != a[i]){
                puts("???");
            }
            QueuePop(q);
        }
        free(a);
        QueueDestroy(q);
        if(ii%10==0){
            printf("ii %d\n",ii);
        }
    }
    printf("Hello, World!\n");
    sleep(10);
    return 0;
}