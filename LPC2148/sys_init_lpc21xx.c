/******************************************************************************
 *
 * sys_init.c
 *
 * Modul zaduzen za inicijalizaciju sistema
 * - POWER CONTROL BITS
 * - PLL frekvencija i preskaler
 * - GPIO smjer i stanje izlaznih pinova
 * - MAM : setuje se Memory Accelerator Modul
 * - VPBUS : setuje se brzina peripheral bus-a
 * - mapiraju se interapt vektori u SRAM ili u FLASH
 * - Inicijalizuje se sistemski tajmer
 *
 * Svi parametri definisani su u config.h
 *
 *****************************************************************************/

#include "boardconfig.h"
#include "lpcmcu.h"
#include "sys_init.h"

#ifndef BIT
    #define BIT(n) (1UL << (n))
#endif

// SYSPLL FCCO limits
#define __FCCOMIN 156000000
#define __FCCOMAX 320000000


#ifndef CCLK
    #error CCLK is not defined
#endif

#ifndef FOSC
    #error FOSC is not defined
#endif

#ifndef PCLK
    #error PCLK is not defined
#endif

#if (FOSC < 1000000) || (FOSC > 25000000)
	#error FOSC out of range (1MHz - 25MHz)
#endif

#if (PCLK > CCLK)
	#error Invalid PCLK value. Should be CCLK / n where n is 1, 2 or 4
#endif

#if (((CCLK / FOSC) * FOSC) != CCLK)
    #error Invalid CCLK value. Should be n * FOSC
#endif

#if (((CCLK / PCLK) * PCLK) != CCLK)
    #error Invalid PCLK value. Should be CCLK / n where n is 1, 2 or 4
#endif

#if (CCLK < 1000000) || (CCLK > 60000000)
    #error CCLK is out of range (1MHz - 60MHz)
#endif

#define __PBSD (CCLK / PCLK)

// APBDIV LPC21xx
// 0 --> div = 4
// 1 --> div = 1
// 2 --> div = 2
#if (__PBSD == 1)
	#define __APBDIV 1
#elif (__PBSD == 2)
	#define __APBDIV 2
#elif (__PBSD == 4)
	#define __APBDIV 0
#else
	#error Invalid PCLK value. Should be CCLK / n where n is 1, 2 or 4
#endif

#if ((FOSC) != (CCLK))
    #define __PLL_ENABLE
    #if (FOSC < 10000000) || (FOSC > 25000000)
        #error FOSC out of range (10MHz - 25MHz)
    #endif
    #define __MSEL (((CCLK) / (FOSC)) - 1)
    #if (__MSEL < 0) || (__MSEL > 31)
        #error "SYSPLL MSEL out of range"
    #endif
    #define __P (__FCCOMAX / (2 * CCLK))
    #if (__P >= 8)
        #define __PSEL 3
    #elif (__P >= 4)
        #define __PSEL 2
    #elif (__P >= 2)
        #define __PSEL 1
    #elif (__P == 1)
        #define __PSEL 0
    #else
        #error "SYSPLL PSEL out of range"
    #endif
    #define __REAL_FCCO (2 * (1 << (__PSEL)) * (CCLK))
    #if (__REAL_FCCO < __FCCOMIN) || (__REAL_FCCO > __FCCOMAX)
        #error "SYSPLL FCCO out of range"
    #endif
    #define __SYSPLLCTRL_Val ((__MSEL) | (__PSEL) << 5)
#endif


#define __MAMTIM_CYCLES (((CCLK) + 19999999) / 20000000)



// power-down unused devices
// starts up the PLL
// sets up GPIO pins
// sets up ADC & DAC pins
// wait for PLL to lock.
// connect to PLL clock
// setup & enable the MAM
// set interrup vectors mapping
void sys_init(void)
{
	// set POWER CONTROL BITS
	// PCONP = POWER_CONTROL_WORD;
	// disable clock to all peripherals, it should be enabled in peripheral init function
	PCONP = 0;

	// set PLL multiplier & divisor.
	// values computed from config.h
#ifdef __PLL_ENABLE
	PLLCFG = __SYSPLLCTRL_Val;

	// enable PLL
	PLLCON = PLLCON_PLLE;
	PLLFEED = 0xAA;                       // Make it happen.  These two updates
	PLLFEED = 0x55;                       // MUST occur in sequence.
#endif // __PLL_ENABLE

	// setup the parallel port pin
#ifdef FAST_GPIO
	GPIO0M = 1; // enable fast IO for P0
	GPIO1M = 1; // enable fast IO for P1
	P0MASK = FIO0_MASK_BITS;				 // Set bit mask
	P1MASK = FIO1_MASK_BITS;				 // Set bit mask
#endif // FAST_GPIO
	P0PIN = PIO0_ONE_BITS;                 // set the ONEs output
	P0DIR = PIO0_OUTPUT_BITS;              // set the output bit direction

	P1PIN = PIO1_ONE_BITS;                 // set the ONEs output
	P1DIR = PIO1_OUTPUT_BITS;              // set the output bit direction

	// ADC & DAC pins
#if (PIO0_ADC_BITS & BIT(4))    // AD0.6
	PINSEL0 |= (BIT(9)  | BIT(8));
#endif
#if (PIO0_ADC_BITS & BIT(5))   // AD0.7
	PINSEL0 |= (BIT(11) | BIT(10));
#endif
#if (PIO0_ADC_BITS & BIT(6))   // AD1.0
	PINSEL0 |= (BIT(13) | BIT(12));
#endif
#if (PIO0_ADC_BITS & BIT(8))   // AD1.1
	PINSEL0 |= (BIT(17) | BIT(16));
#endif
#if (PIO0_ADC_BITS & BIT(10))  // AD1.2
	PINSEL0 |= (BIT(21) | BIT(20));
#endif
#if (PIO0_ADC_BITS & BIT(12))  // AD1.3
	PINSEL0 |= (BIT(25) | BIT(24));
#endif
#if (PIO0_ADC_BITS & BIT(13))  // AD1.4
	PINSEL0 |= (BIT(27) | BIT(26));
#endif
#if (PIO0_ADC_BITS & BIT(15))  // AD1.5
	PINSEL0 |= (BIT(31) | BIT(30));
#endif
#if (PIO0_ADC_BITS & BIT(21))  // AD1.6
	PINSEL1 |= (BIT(11) | 0);
#endif
#if (PIO0_ADC_BITS & BIT(22))  // AD1.7
	PINSEL1 |= (BIT(12) | 0);
#endif
#if (PIO0_ADC_BITS & BIT(25))  // AD0.4
	PINSEL1 |= (BIT(18) | 0);
#endif
#if (PIO0_ADC_BITS & BIT(28))  // AD0.1
	PINSEL1 |= (BIT(24) | 0);
#endif
#if (PIO0_ADC_BITS & BIT(29))  // AD0.2
	PINSEL1 |= (BIT(26) | 0);
#endif
#if (PIO0_ADC_BITS & BIT(30))  // AD0.3
	PINSEL1 |= (BIT(28) | 0);
#endif

#if (PIO0_DAC_BITS & BIT(25))
	PINSEL1 |= (BIT(19) | 0);
#endif

	// enable / disable DEBUG & TRACE on PIO1
#ifndef ENABLE_PIO1_DEBUG_TRACE
	PINSEL2 &= ~(BIT(2) | BIT(3));
#endif

	// set the peripheral bus speed
	// value computed from config.h
	VPBDIV = __APBDIV;                // set the peripheral bus clock speed

#ifdef __PLL_ENABLE
	// wait for PLL lock
	while (!(PLLSTAT & PLLSTAT_LOCK)) continue;

	// enable & connect PLL
	PLLCON = PLLCON_PLLE | PLLCON_PLLC;
	PLLFEED = 0xAA;                       // Make it happen.  These two updates
	PLLFEED = 0x55;                       // MUST occur in sequence.
#endif // __PLL_ENABLE

	// setup & enable the MAM
	MAMTIM = __MAMTIM_CYCLES;
	MAMCR = 2; // MAMCR_FULL;

	// set the interrupt controller defaults
#ifdef RAM_RUN
	MEMMAP = 2; // MEMMAP_SRAM;                 // map interrupt vectors space into SRAM
#elif defined(ROM_RUN)
	MEMMAP = 1; // MEMMAP_FLASH;                // map interrupt vectors space into FLASH
#else
	#error RUN_MODE not defined!
#endif
}

