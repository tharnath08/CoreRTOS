#ifndef INC_RP2040_H
#define INC_RP2040_H
#include "rtos_config.h"
#include "CoreRTOS.h"
#include "scheduler.h"



// SysTick Registers (ARM Cortex-M0+ Standard)
#define SYST_CSR   (*((volatile uint32_t *)0xE000E010))  // SysTick Control and Status Register
#define SYST_RVR   (*((volatile uint32_t *)0xE000E014))  // SysTick Reload Value Register
#define SYST_CVR   (*((volatile uint32_t *)0xE000E018))  // SysTick Current Value Register
#define SYST_CALIB (*((volatile uint32_t *)0xE000E01C))  // SysTick Calibration Value Register

// SysTick CSR Register Bits
#define SYST_CSR_ENABLE     (1 << 0)  // Enable SysTick
#define SYST_CSR_TICKINT    (1 << 1)  // Enable SysTick Interrupt
#define SYST_CSR_CLKSOURCE  (1 << 2)  // Use Processor Clock

// Default System Clock for RP2040 (if not provided)
#ifndef RP2040_DEFAULT_SYS_CLOCK
#define RP2040_DEFAULT_SYS_CLOCK 125000000UL  
#endif
#define VTOR ((volatile uint32_t *)0xE000ED08)
#define VECTOR_TABLE ((volatile uint32_t *)0x20000000) // Place in RAM
#define RTOS_SYSTICK_EXCEPTION 15


void initSystick(uint32_t);
void context_Switch(void);
void isr_pendsv(void);

#endif