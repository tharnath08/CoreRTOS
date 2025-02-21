#ifndef CORE_RTOS_H
#define CORE_RTOS_H

#include <stdint.h>
#include <stddef.h>
#include "rtos_config.h"

#ifdef TARGET_RP2040
#include "rp2040.h"
#endif

#ifdef TARGET_CM4
#include "cm4.h"
#endif


#endif