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

// mcu_macros.h

#ifndef __MCU_MACROS_H__
#define __MCU_MACROS_H__

#define INTERRUPT_IRQ __attribute__((interrupt("IRQ")))
#define INTERRUPT_FIQ __attribute__((interrupt("FIQ")))

#define BSSNOINIT  __attribute__((section(".bssnoinit")))
#define FLASH      __attribute__((section(".flash")))
#define RAMFCN     __attribute__((section(".ramfcn")))
#define PACKED     __attribute__((packed))
#define ALIGNED(x) __attribute__((aligned(x)))

#ifndef BIT
    #define BIT(n) (1L << (n))
#endif


// NOP
#ifndef NOP
    #define NOP() asm volatile ("nop\n")
#endif

/* some other useful macros */
#ifndef MIN
    #define MIN(x,y)	((x)<(y)?(x):(y))	/**< MIN */
#endif
#ifndef MAX
    #define MAX(x,y)	((x)>(y)?(x):(y))	/**< MAX */
#endif

#ifndef NULL
    #define NULL	((void*)0)			/**< NULL pointer */
#endif

#endif // __MCU_MACROS_H__

