#include "mythread.h"
#include <iostream>
MyThread::MyThread()
{

}
MyThread::~MyThread(){

}
void MyThread::run(){
    while(isrun == true) {
        std::cout<<"running"<<std::endl;
        int ms = 1000;
        struct timespec ts = { ms / 1000, (ms % 1000) * 1000 * 1000 };
        nanosleep(&ts, NULL);
    }
}
void MyThread::start() {
    std::cout<<"start"<<std::endl;
    this->isrun = true;
}

void MyThread::stop(){
    std::cout<<"stop"<<std::endl;
    this->isrun = false;
}
