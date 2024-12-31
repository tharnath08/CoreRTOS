#include "CoreRTOS.h"
#include "scheduler.h"


BaseType_t createTask(TaskFunction pxTaskCode, uint8_t priority){

    if(taskCount >= MAX_TASKS){
        return -1;
    }

    for(int tskIndx = 0; tskIndx < MAX_TASKS; tskIndx++){
        TaskControlBlock *task = &RTOS_tasks[tskIndx];
        if(task->state == NULL || task->state == TASK_DELETED){
            task->taskFunction = pxTaskCode;
            task->priority = priority;
            task->state = TASK_READY;
            task->wake_up_tick = 0;
            task->delay_ticks = 0;
            taskCount++;
            break;
        }
    }

    return 0;
}

TaskControlBlock* get_next_ready_task(void){
    TaskControlBlock *highestPriorityTask;
    int8_t highestPriority = 1000;
    uint8_t tempTskIndx = 0;

    for(int tskIndx = 0;tskIndx<MAX_TASKS;tskIndx++){
        TaskControlBlock *task = &RTOS_tasks[tskIndx];
        if(task->state == TASK_READY && task->priority<highestPriority){
            highestPriorityTask = task;
            highestPriority = task->priority;
            tempTskIndx = tskIndx;
        }
    }

    if (highestPriorityTask == NULL) {
        return NULL; // No ready tasks
    }

    currentTaskIndex = tempTskIndx;
    return highestPriorityTask;
}

BaseType_t startScheduler(void){
    while(1){
        TaskControlBlock *nextTask = get_next_ready_task();
        nextTask->taskFunction(NULL);
    }
}

BaseType_t yeild_to_scheduler(){
    run_next_ready_task();
}

BaseType_t run_next_ready_task(){
    TaskControlBlock *nextTask = get_next_ready_task();
    switch_to_task(&nextTask);
}

BaseType_t switch_to_task(TaskControlBlock *task) {

}


TaskControlBlock* get_current_task(){
    return &RTOS_tasks[currentTaskIndex];
}

BaseType_t taskDelay(uint32_t delayTicks){
    TaskControlBlock *currentTask = get_current_task();

    currentTask->delay_ticks = delayTicks;
    currentTask->wake_up_tick = currentTicks + delayTicks;
    currentTask->state = TASK_DELAYED;

    yeild_to_scheduler();

}

void tick_scheduler_callback(){
    currentTicks++;

    for(int tskIndx = 0; tskIndx < MAX_TASKS; tskIndx++){
        TaskControlBlock *task = &RTOS_tasks[tskIndx];
        if(task->state == TASK_DELAYED && currentTicks >= task->wake_up_tick){
            task->state = TASK_READY;
        }
    }

    yeild_to_scheduler();
}