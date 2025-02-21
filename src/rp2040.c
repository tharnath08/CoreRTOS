#include "rp2040.h"
#include <stdio.h>

void initSystick(uint32_t system_clock_hz){
    if (system_clock_hz == 0) {
        system_clock_hz = RP2040_DEFAULT_SYS_CLOCK;
    }    

    // Stop and reset the SysTick timer
    SYST_CSR = 0;
    SYST_CVR = 0;

    // Set reload value for 1ms tick
    SYST_RVR = (system_clock_hz / 1000) - 1;

    // Enable SysTick with system clock and interrupt
    SYST_CSR = SYST_CSR_ENABLE | SYST_CSR_TICKINT | SYST_CSR_CLKSOURCE;

    *VTOR = (uint32_t)VECTOR_TABLE;
    ((uint32_t *)VECTOR_TABLE)[RTOS_SYSTICK_EXCEPTION] = (uint32_t)tick_scheduler_callback;
}

BaseType_t switch_to_task(TaskControlBlock *nextTask) {
    TaskControlBlock *currentTask = get_current_task();

    // Step 1: Save the current task's context (stack pointer)
    __asm volatile (
        "MRS R0, MSP\n"                 // Read the current MSP (Main Stack Pointer) into R0
        "STR R0, [%0]\n"                // Store the current stack pointer into the current task's TCB
        : /* no output */
        : "r" (currentTask->stackPointer)  // Input: address of current task's stack pointer
        : "r0"
    );

    // Step 2: Set the MSP to the next task's stack pointer
    __asm volatile (
        "LDR R0, [%0]\n"                // Load the next task's stack pointer into R0
        "MSR MSP, R0\n"                 // Set the MSP to the next task's stack pointer
        : /* no output */
        : "r" (nextTask->stackPointer)  // Input: next task's stack pointer
        : "r0"
    );

    // Step 3: Return from the interrupt - switch to the next task
    // This will cause the program counter (PC) to be loaded from the stack of the next task
    __asm volatile (
        "BX LR\n"  // Branch to the address stored in the Link Register (LR)
    );

    // This should never be reached since BX LR should jump to the next task's code
    return 1;  // If we get here, something went wrong
}






