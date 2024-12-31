#ifndef INC_TASK_H
#define INC_TASK_H

#define MAX_TASKS 2


typedef signed char      BaseType_t;


typedef void (* TaskFunction)( void * arg );

typedef enum {
    TASK_READY,
    TASK_DELAYED,
    TASK_RUNNING,
    TASK_IDLE,
    TASK_DELETED
} task_state_t;


typedef struct {
    TaskFunction taskFunction; // Pointer to the task function
    uint32_t delay_ticks;         // Delay time in ticks
    uint32_t wake_up_tick;       // Tick when the task should wake up
    uint32_t stackPointer;      // Stack pointer for task
    uint8_t priority;           // Task priority
    task_state_t state;           // Task state
} TaskControlBlock;

static TaskControlBlock RTOS_tasks[MAX_TASKS];
static uint8_t currentTaskIndex = 0;
static uint8_t taskCount = 0;
static uint32_t currentTicks = 0;



BaseType_t createTask( TaskFunction pxTaskCode, uint8_t priority);
BaseType_t startScheduler(void);
TaskControlBlock getNextTask(void);

#endif