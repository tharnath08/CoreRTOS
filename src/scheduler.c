#include "CoreRTOS.h"
#include "scheduler.h"


TaskFunction taskFunction; // Pointer to the task function
uint32_t delay_ticks;         // Delay time in ticks
uint32_t wake_up_tick;       // Tick when the task should wake up
uint32_t stackPointer;      // Stack pointer for task
uint8_t priority;           // Task priority
task_state_t state;


void initRTOS(uint32_t freq){
    initSystick(freq);
}


void printTaskDetails(){
    for(int i=0; i<MAX_TASKS; i++){
        printf("Task-%d\n\tState: %d\n\tPriority: %d\n\tDelay Tick: %d\n\n",i,RTOS_tasks[i].state,RTOS_tasks[i].priority,RTOS_tasks[i].delay_ticks);
    }
}
BaseType_t createTask(TaskFunction pxTaskCode,uint32_t stack_size, uint8_t priority){

    if(taskCount >= MAX_TASKS){
        return -1;
    }
    for(int tskIndx = 0; tskIndx < MAX_TASKS; tskIndx++){
        TaskControlBlock *task = &RTOS_tasks[tskIndx];
        if(task->state == TASK_DELETED){
            task->taskFunction = pxTaskCode;
            task->priority = priority;
            task->state = TASK_READY;
            task->wake_up_tick = 0;
            task->delay_ticks = 0;
            task->stack = (uint32_t *)malloc(stack_size * sizeof(uint32_t));
            uint32_t *stack = task->stack + stack_size - 16;  // Reserve space for the stack frame
            task->stackPointer = stack;
            // Stack frame setup (R0-R3, R12, LR, PC, xPSR)
            stack[0] = 0x01000000;  // xPSR (set Thumb bit)
            stack[1] = (uint32_t)pxTaskCode;  // PC (Program Counter)
            stack[2] = 0xFFFFFFF9;  // LR (Link Register, invalid address to force return)
            stack[3] = 0x0;  // R12
            stack[4] = 0x0;  // R3
            stack[5] = 0x0;  // R2
            stack[6] = 0x0;  // R1
            stack[7] = 0x0;  // R0
            stack[8] = 0x0;  // R7
            stack[9] = 0x0;  // R6
            stack[10] = 0x0; // R5
            stack[11] = 0x0; // R4
            taskCount++;
            break;
        }
    }
    
    return 0;
}

TaskControlBlock* get_next_ready_task(void) {
    TaskControlBlock *highestPriorityTask = NULL; // Initialize to NULL
    int8_t highestPriority = -1; // Assume the worst priority
    uint8_t tempTskIndx = 0;
    for (int tskIndx = 0; tskIndx < MAX_TASKS; tskIndx++) {
        TaskControlBlock *task = &RTOS_tasks[tskIndx];
        if (task->state == TASK_READY && task->priority > highestPriority) {
            highestPriorityTask = task;
            highestPriority = task->priority;
            tempTskIndx = tskIndx;
        }
    }

    if (highestPriorityTask != NULL) {
        currentTaskIndex = tempTskIndx;
    }
    return highestPriorityTask; // Return the task or NULL if no ready tasks
}


BaseType_t startScheduler(void){
    while(1){
        // printTaskDetails();
        TaskControlBlock *nextTask = get_next_ready_task();
        if(nextTask != NULL)
            nextTask->taskFunction(NULL);
    }
}

BaseType_t yeild_to_scheduler(){
    run_next_ready_task();
}

BaseType_t run_next_ready_task(){
    TaskControlBlock *nextTask = get_next_ready_task();
    switch_to_task(nextTask);
}


TaskControlBlock* get_current_task(){
    return &RTOS_tasks[currentTaskIndex];
}

BaseType_t taskDelay(uint32_t delayTicks){
    TaskControlBlock *currentTask = get_current_task();

    currentTask->delay_ticks = delayTicks;
    currentTask->wake_up_tick = currentTicks + delayTicks;
    currentTask->state = TASK_DELAYED;

    // yeild_to_scheduler();

}

void tick_scheduler_callback(){
    currentTicks++;

    for(int tskIndx = 0; tskIndx < MAX_TASKS; tskIndx++){
        TaskControlBlock *task = &RTOS_tasks[tskIndx];
        if(task->state == TASK_DELAYED && currentTicks >= task->wake_up_tick){
            task->state = TASK_READY;
        }
    }

    // yeild_to_scheduler();
}