#ifndef INC_TASK_H
#define INC_TASK_H

#ifndef MAX_TASKS
    #define MAX_TASKS 2
#endif



typedef signed char      BaseType_t;


typedef void (* TaskFunction)( void * arg );

typedef enum {
    TASK_DELETED,
    TASK_READY,
    TASK_DELAYED,
    TASK_RUNNING,
    TASK_IDLE,
} task_state_t;


typedef struct {
    TaskFunction taskFunction; // Pointer to the task function
    uint32_t delay_ticks;         // Delay time in ticks
    uint32_t wake_up_tick;       // Tick when the task should wake up
    uint32_t *stackPointer;      // Stack pointer for task
    uint32_t *stack;
    uint8_t priority;           // Task priority
    task_state_t state;           // Task state
} TaskControlBlock;

static TaskControlBlock* RTOS_tasks[MAX_TASKS] = {0};
static uint8_t currentTaskIndex = 0;
static uint8_t taskCount = 0;
static uint32_t currentTicks = 0;


void initRTOS(uint32_t);
BaseType_t createTask( TaskFunction pxTaskCode,uint32_t stack_size, uint8_t priority);
BaseType_t startScheduler(void);
TaskControlBlock* get_next_ready_task(void);
BaseType_t yeild_to_scheduler(void);
BaseType_t run_next_ready_task(void);
void switch_to_task(TaskControlBlock *task);
TaskControlBlock* get_current_task();
BaseType_t taskDelay(uint32_t delayTicks);
void tick_scheduler_callback(void);
void printTaskDetails(void);


#endif