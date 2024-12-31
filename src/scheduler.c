#include "CoreRTOS.h"
#include "scheduler.h"


BaseType_t CreateTask(TaskFunction pxTaskCode, uint8_t priority){

    if(currentTask > MAX_TASKS){
        return -1;
    }

    TaskControlBlock task;
    task.taskFunction = pxTaskCode;
    task.priority = priority;
    task.status = TASK_READY;
 
    RTOS_tasks[currentTask] = task;
    currentTask++;

    return 0;
}

TaskControlBlock getNextTask(void){
    TaskControlBlock highestPriorityTask;
    int8_t highestPriority = MAX_TASKS;

    for(int i = 0;i<MAX_TASKS;i++){
        if(RTOS_tasks[i].status == TASK_READY && RTOS_tasks[i].priority<highestPriority){
            highestPriorityTask = RTOS_tasks[i];
            highestPriority = RTOS_tasks[i].priority;

        }
    }

    return highestPriorityTask;
}

BaseType_t startScheduler(void){
    while(1){
        TaskControlBlock nextTask = getNextTask();
        nextTask.taskFunction(NULL);
    }
}