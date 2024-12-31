#include <stdio.h>
#include "CoreRTOS.h"
#include "scheduler.h"

void hello_world(){
        printf("hello_world\n");
}

void task2(){
        printf("this is task 2 \n");
}

int main(){
    createTask(hello_world, 1);
    createTask(task2, 2);
    startScheduler();
}
