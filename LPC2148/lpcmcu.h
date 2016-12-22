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

