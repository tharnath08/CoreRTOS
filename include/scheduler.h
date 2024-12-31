#ifndef INC_TASK_H
#define INC_TASK_H

#define MAX_TASKS 2

#define TASK_READY    1
#define TASK_RUNNING  2
#define TASK_IDLE     3

typedef signed char      BaseType_t;


typedef void (* TaskFunction)( void * arg );



typedef struct {
    TaskFunction taskFunction; // Pointer to the task function
    uint32_t stackPointer;      // Stack pointer for task
    uint8_t priority;           // Task priority
    uint8_t status;           // Task state
} TaskControlBlock;

static TaskControlBlock RTOS_tasks[MAX_TASKS];
static uint8_t currentTask = 0;



BaseType_t CreateTask( TaskFunction pxTaskCode, uint8_t priority);
BaseType_t startScheduler(void);
TaskControlBlock getNextTask(void);

#endif