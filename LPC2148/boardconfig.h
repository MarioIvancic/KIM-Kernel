// boardconfig.h
// system config == board config


#ifndef __BOARDCONFIG_H__
#define __BOARDCONFIG_H__


// PLL setup values are computed within the LPC include file
// It relies upon the following defines

// Master Oscillator Freq.
#define FOSC                (12000000)
#define CCLK                (60000000)
#define PCLK                (30000000)
#define RIT_CLK             (12500000)
#define LPC_TIMER_CLK       (10000000)


// internal wdt modes: 0, 2: off, 1: interrupt, 3: reset
//#define WDT_MODE 3
//#define WDT_TIMEOUT (4 * 600 * ONE_MS)

// Port Bit Definitions & Macros
#include "OlimexP2148-template.h"

#endif // __BOARDCONFIG_H__
