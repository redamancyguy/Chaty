#include <stdio.h>
#include <ucontext.h>
#include "unistd.h"
#include "malloc.h"

void func(ucontext_t *c1,ucontext_t *c2)
{
    for(int i=0;i<100;i++){
        printf("==========%d\n",i);
        usleep(10000);
//        setcontext(c2);
        swapcontext(c1, c2);
    }
    puts("OK");
}
int size = 1024*8;

int main (void)
{
    ucontext_t ctx[3];


    getcontext(&ctx[1]);
    ctx[1].uc_stack.ss_sp   = malloc(size);
    ctx[1].uc_stack.ss_size = size;
    ctx[1].uc_link = &ctx[2];
    makecontext(&ctx[1], func, 2,&ctx[1],&ctx[0]);


    getcontext(&ctx[2]);
    ctx[2].uc_stack.ss_sp   = malloc(size);
    ctx[2].uc_stack.ss_size = size;
    ctx[2].uc_link = &ctx[0];
    makecontext(&ctx[2], func, 2,&ctx[2],&ctx[0]);



    for(int i=0;i<1024;i++){
        printf("main %d\n",i);
        swapcontext(&ctx[0], &ctx[1]);
        swapcontext(&ctx[0], &ctx[2]);
    }
    puts("return");
    return 0;
}