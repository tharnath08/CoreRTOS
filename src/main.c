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
    CreateTask(hello_world, 1);
    CreateTask(task2, 2);
    startScheduler();
}
