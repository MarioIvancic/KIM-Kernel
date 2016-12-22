// appconfig.h

#ifndef __APP_CONFIG_H__
#define __APP_CONFIG_H__

#include "boardconfig.h"
#include "mcu_macros.h"


// #################################

#include "lpcmcu.h"
#include "crt0.h"


// features

// WDT
//#define WDT_ENABLE

// Fast IO
// #define FAST_GPIO

#define UART0_BAUD           (115200)
#define UART1_BAUD           (115200)

#endif // __APP_CONFIG_H__
