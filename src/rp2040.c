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

__attribute__((naked)) void switch_to_task(TaskControlBlock *newTask) {
    __asm volatile(
        "bl context_Switch \n"
        "bx lr \n"
    );
}






