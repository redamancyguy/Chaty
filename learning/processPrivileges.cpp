#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sched.h>
void run(){
    long i=0;
    while(i++ < 1000000000000000){
        char buf[1024];
        memset(buf,0,1024);
    }
}
int main1() {
    long time = clock();
    pthread_attr_t attr;
    struct sched_param param;
    int ret;
    pthread_attr_init(&attr);
    // policy = SCHED_FIFO;

    ret = pthread_attr_setschedpolicy(&attr, SCHED_RR);
    if (ret != 0)    printf(" pthread_attr_setschedpolicy/n%s/n", strerror(ret));

    param.sched_priority = 1;
    ret = pthread_attr_setschedparam(&attr, &param);
    if (ret != 0)    printf(" pthread_attr_setschedparam/n%s/n", strerror(ret));
    pthread_t thread_la;
    pthread_attr_setinheritsched(&attr,PTHREAD_EXPLICIT_SCHED);   // 设置线程调度策略时，必须设置此参数，否则会继承父进程的属性。
    ret = pthread_create(&thread_la, &attr, run, NULL);

    pthread_join(thread_la,NULL);

    cpu_set_t set;
    CPU_ZERO(&set);
//    if (pthread_getaffinity_np(pthread_self(), sizeof(set), &set) < 0)
//    {
//        fprintf(stderr, "get thread affinity failed\n");
//    }
    CPU_SET(0 ,&set);
    if (pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &set) < 0)
    {
        fprintf(stderr, "set thread[%x] affinity failed\n", (unsigned int)pthread_self());
        return 1;
    }
    for(int i=0;i<12;i++){
        printf("%d\n",CPU_ISSET(i, &set));
    }
    for(int i=0;i<4;i++){
        CPU_SET(12-1-i, &set);
    }
    puts("==========================");
    for(int i=0;i<12;i++){
        printf("%d\n",CPU_ISSET(i, &set));
    }
    printf("%d\n",sysconf(_SC_NPROCESSORS_CONF));
    printf("%d\n",sysconf(_SC_NPROCESSORS_ONLN));
    long i=0;
    while(i++ < 100000000){
        char buf[1024];
        memset(buf,0,1024);
    }
    printf("Hello, World!\n");
    printf("%ld\n",clock()-time);
    return 0;
}

//pthread_exit