//
// Created by 35148 on 2024/5/17.
//
#include "pthread.h"
#include <iostream>

void* ptintf_hello_world(void* tid);

int main(void){
    pthread_t thread;
    int status,i=10;
    printf("Main here. Creating thread %d\n",i);
    status=pthread_create(&thread,NULL,ptintf_hello_world,(void*)&i);

    pthread_join(thread,NULL);  //pthread_join函数以阻塞的方式等待指定的线程结束；如果线程已经结束，函数会立即返回，并且指定的线程必须是joinable的

    if(status!=0){
        printf("pthread_create returned error code %d\n",status);
        exit(-1);
    }
    exit(0);
}
void* ptintf_hello_world(void* tid){
    printf("Hello world %d.\n",*(int *)tid);
    exit(0);
}