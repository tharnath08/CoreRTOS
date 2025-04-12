#include "CoreRTOS.h"
#include "scheduler.h"

TaskFunction taskFunction; // Pointer to the task function
uint32_t delay_ticks;         // Delay time in ticks
uint32_t wake_up_tick;       // Tick when the task should wake up
uint32_t stackPointer;      // Stack pointer for task
uint8_t priority;           // Task priority
task_state_t state;
volatile uint32_t newStackPointer;


void initRTOS(uint32_t freq){
    initSystick(freq);
}

void printTaskDetails(){
    for(int i=0; i<MAX_TASKS; i++){
        printf("Task-%d\n\tState: %d\n\tPriority: %d\n\tDelay Tick: %d\n\n",i,RTOS_tasks[i]->state,RTOS_tasks[i]->priority,RTOS_tasks[i]->delay_ticks);
    }
}


BaseType_t createTask(TaskFunction taskCode,uint32_t stackSize, uint8_t priority){

    if(taskCount >= MAX_TASKS){
        return -1;
    }

    TaskControlBlock *newTask = (TaskControlBlock *)malloc(sizeof(TaskControlBlock));
    if (newTask == NULL) {
        return -1;
    }

    newTask->stack = (uint32_t *)malloc(stackSize * sizeof(uint32_t));
    if (newTask->stack == NULL) {
        free(newTask);
        return -1;
    }

    newTask->taskFunction = taskCode;
    newTask->priority = priority;
    newTask->state = TASK_READY;

    uint32_t *stack = newTask->stack + stackSize - 17;
    newTask->stackPointer = stack;


    /*
    Stack frame layout to mimic Cortex-M hardware behavior:
    +------+
    | xPSR | stack[16]  (0x01000000 -> Thumb mode enabled)
    |  PC  | stack[15]  (Address of task function)
    |  LR  | stack[14]  (0xFFFFFFFD -> Return to thread mode with PSP)
    |  R12 | stack[13]  
    |  R3  | stack[12]  
    |  R2  | stack[11]  
    |  R1  | stack[10]  
    |  R0  | stack[9]   
    +------+
    Registers saved by software in PendSV:
    +------+
    |  LR  | stack[8]  (Special EXC_RETURN value for PSP)
    |  R7  | stack[7]  
    |  R6  | stack[6]  
    |  R5  | stack[5]  
    |  R4  | stack[4]  
    |  R11 | stack[3]  
    |  R10 | stack[2]  
    |  R9  | stack[1]  
    |  R8  | stack[0]  
    +------+
    */

    // Hardware-saved registers
    stack[15] = 0x01000000;        // xPSR (Thumb bit set)
    stack[14] = (uint32_t)taskCode; // PC (entry point of task)
    stack[13] = 0xFFFFFFFD;        // LR (Return to Thread Mode with PSP)
    stack[12] = 0x1c;        // R12
    stack[11] = 0x13;        // R3
    stack[10] = 0x12;        // R2
    stack[9] = 0x11;        // R1
    stack[8]  = 0x10;        // R0

    // Software-saved registers
    stack[7]  = 0x17;        // R7
    stack[6]  = 0x16;        // R6
    stack[5]  = 0x15;        // R5
    stack[4]  = 0x14;        // R4
    stack[3]  = 0x1b;        // R11
    stack[2]  = 0x1a;        // R10
    stack[1]  = 0x19;        // R9
    stack[0]  = 0x18;        // R8


    for (int i = 0; i < MAX_TASKS; i++) {
        if (RTOS_tasks[i] == NULL) {
            RTOS_tasks[i] = newTask;
            taskCount++;
            return i;
        }
    }
    
    return -1;
}

TaskControlBlock* get_next_ready_task(void) {
    TaskControlBlock *highestPriorityTask = NULL; // Initialize to NULL
    int8_t highestPriority = -1; // Assume the worst priority
    uint8_t tempTskIndx = 0;
    for (int tskIndx = 0; tskIndx < MAX_TASKS; tskIndx++) {
        TaskControlBlock *task = RTOS_tasks[tskIndx];
        if (task->state == TASK_READY && task->priority > highestPriority) {
            highestPriorityTask = task;
            highestPriority = task->priority;
            tempTskIndx = tskIndx;
        }
    }

    if (highestPriorityTask != NULL) {
        currentTaskIndex = tempTskIndx;
    }
    return highestPriorityTask;
}


BaseType_t startScheduler(void){
    initSystick(0);
    // uint32_t* SCB_ICSR = (uint32_t *)0xE000ED04;
    // *SCB_ICSR |= (1<<28);
    // while(1){
    //     // printTaskDetails();
    //     TaskControlBlock *nextTask = get_next_ready_task();
    //     if(nextTask != NULL)
    //         nextTask->taskFunction(NULL);
    // }
}

BaseType_t yeild_to_scheduler(){
    run_next_ready_task();
}

BaseType_t run_next_ready_task(){
    TaskControlBlock *nextTask = get_next_ready_task();
    if(nextTask != NULL){
        switch_to_task(nextTask);
    }
}


TaskControlBlock* get_current_task(){
    return *(RTOS_tasks+currentTaskIndex);
}

BaseType_t taskDelay(uint32_t delayTicks){
    TaskControlBlock *currentTask = get_current_task();

    currentTask->delay_ticks = delayTicks;
    currentTask->wake_up_tick = currentTicks + delayTicks;
    currentTask->state = TASK_DELAYED;
    uint32_t* SCB_ICSR = (uint32_t *)0xE000ED04;
    *SCB_ICSR |= (1<<28);
    // yeild_to_scheduler();
}

void tick_scheduler_callback(){
    // __asm volatile(
    //     "bl context_Switch \n"
    // );
    currentTicks++;

    for(int tskIndx = 0; tskIndx < MAX_TASKS; tskIndx++){
        TaskControlBlock *task = RTOS_tasks[tskIndx];
        if(task->state == TASK_DELAYED && currentTicks >= task->wake_up_tick){
            task->state = TASK_READY;
        }
    }


    uint32_t* SCB_ICSR = (uint32_t *)0xE000ED04;
    *SCB_ICSR |= (1<<28);
    // yeild_to_scheduler();
}



void printStackDetails(uint32_t* stackPointer){
    printf("Stack Address 0x%x\n", stackPointer);
    for(int i = 0; i<16; i++ ){
        printf("Stack Value is 0x%x\n", *(stackPointer+i));
    }
}


int first = 1;
void isr_pendsv(){
    TaskControlBlock *nextTask = get_next_ready_task();
    newStackPointer = nextTask->stackPointer;
    printStackDetails((uint32_t*)newStackPointer);
    if(!first){
        __asm volatile(
            "push {r4 - r7}     \n"
            "mov r1, r8     \n"
            "mov r2, r9     \n"
            "mov r3, r10     \n"
            "mov r4, r11     \n"
            "push {r1 - r4}     \n"
        );
    }
    first = 0;
    __asm volatile(
        "msr psp, %0" :: "r"(newStackPointer)
    );
    __asm volatile(
        "mrs r0, psp              \n"
        "ldmia r0!, {r1-r4}     \n"
    );
    __asm volatile(
        "mov r8, r1      \n"
        "mov r9, r2      \n"
        "mov r10, r3     \n"
        "mov r11, r4     \n"
    );
    __asm volatile(
        "ldmia r0!, {r4-r7}     \n"
    );
    __asm volatile(
        "bx lr     \n"
    );

}
