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

// lpcmcu.h

#ifndef __LPCMCU_H__
#define __LPCMCU_H__

#include <stdint.h>

#if defined(LPC2131) || defined(LPC2132) || defined(LPC2134) || defined(LPC2136) || defined(LPC2138)
    #define LPC213x
	#include "lpc21xx.h"
#endif // LPC213x

#if defined(LPC2141) || defined(LPC2142) || defined(LPC2144) || defined(LPC2146) || defined(LPC2148)
    #define LPC214x
	#include "lpc21xx.h"
#endif // LPC214x

#if defined(LPC1311) || defined(LPC1313) || defined(LPC1342) || defined(LPC1343)
    #define LPC13xx
	#include "LPC13xx.h"
#endif // LPC13xx

#if defined(LPC1751) || defined(LPC1752) || defined(LPC1754) || defined(LPC1756) || defined(LPC1758) || defined(LPC1759)
    #define LPC175x
    #define LPC17xx
	#include "LPC17xx.h"
#endif // LPC175x

#if defined(LPC1763) || defined(LPC1764) || defined(LPC1765) || defined(LPC1766) || defined(LPC1767) || defined(LPC1768) || defined(LPC1769)
    #define LPC176x
    #define LPC17xx
	#include "LPC17xx.h"
#endif // LPC176x

#endif // __LPCMCU_H__

