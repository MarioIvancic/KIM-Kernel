/*  Copyright (c) 2012, Mario Ivančić
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
    ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

// boardconfig.h
// system config == board config


#ifndef __BOARDCONFIG_H__
#define __BOARDCONFIG_H__


// PLL setup values are computed within the LPC include file
// It relies upon the following defines

// Master Oscillator Freq.
#define FOSC                (12000000)
#define CCLK                (100000000)
#define PCLK                (50000000)
#define RIT_CLK             (12500000)
#define LPC_TIMER_CLK       (10000000)


// internal wdt modes: 0, 2: off, 1: interrupt, 3: reset
//#define WDT_MODE 3
//#define WDT_TIMEOUT (4 * 600 * ONE_MS)

// Port Bit Definitions & Macros
#include "lpcxpresso.h"

#endif // __BOARDCONFIG_H__
