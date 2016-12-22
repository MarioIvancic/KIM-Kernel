// sys_init_lpc17xx.c

#include <stdint.h>
#include "boardconfig.h"
#include "lpcmcu.h"
#include "sys_init.h"


// power-down unused devices
// starts up the PLL
// sets up GPIO pins
// sets up ADC & DAC pins
// wait for PLL to lock.
// connect to PLL clock
// setup & enable the MAM
// set interrupt vectors mapping

#ifndef BIT
    #define BIT(n) (1UL << (n))
#endif // BIT



#define __FIRCOSC 4000000    // LPC17xx

// SYSPLL FCCO limits
#define __FCCOMIN 275000000
#define __FCCOMAX 550000000

#ifndef CCLK
    #error CCLK is not defined
#endif

#if (CCLK < 1000000) || (CCLK > 120000000)
    #error CCLK is out of range (1MHz - 120MHz)
#endif

#define __OSCRANGE_Val (0x00)

#ifdef FOSC
    #if (FOSC < 1000000) || (FOSC > 25000000)
        #error FOSC out of range (1MHz - 25MHz)
    #endif
    #define __SYSPLLCLKIN (FOSC)
    #if (FOSC > 20000000)
        #undef __OSCRANGE_Val
        #define __OSCRANGE_Val (0x10)
    #endif
#else
    #define __SYSPLLCLKIN (__FIRCOSC)
#endif // FOSC


// select FLASH CONFIG register
#if CCLK <= 20000000
    #define __FLASHCFG_Val (0x3a | (0 << 12))
#elif CCLK <= 40000000
    #define __FLASHCFG_Val (0x3a | (1 << 12))
#elif CCLK <= 60000000
    #define __FLASHCFG_Val (0x3a | (2 << 12))
#elif CCLK <= 80000000
    #define __FLASHCFG_Val (0x3a | (3 << 12))
#elif CCLK <= 100000000
    #define __FLASHCFG_Val (0x3a | (4 << 12))
#else
    #if (defined(LPC1759) || defined(LPC1769)) && (CCLK <= 120000000)
        #define __FLASHCFG_Val (0x3a | (4 << 12))
    #else
        #define __FLASHCFG_Val (0x3a | (5 << 12))
    #endif
#endif


// select PLL clock source: main-OSC, RTC-OSC or IRC-OSC
#if defined(FOSC)
    #define __CLKSRCSEL_Val 1 // main OSC
#else
    #define __CLKSRCSEL_Val 0 // IRC OSC
#endif // FOSC

//#if (((CCLK / __SYSPLLCLKIN) * __SYSPLLCLKIN) != CCLK)
//    #error Invalid CCLK value. Should be n * FOSC or n * IRCOSC
//#endif


#if ((__SYSPLLCLKIN) != (CCLK))
    #define __PLL_ENABLE
    #if (__SYSPLLCLKIN < 10000000) || (__SYSPLLCLKIN > 25000000)
        #error FOSC out of range (10MHz - 25MHz)
    #endif

    #if defined(M) && defined(N) && defined(CLKDIV)
        /*
         FCCO = 2 * M * FOSC / N
         CCLK = FCCO / CLKDIV
         PCLK = CCLK / PCLKDIV

         275MHz <= FCCO <= 550MHz
         10MHz <= FOSC <= 25MHz
         1 <= N <= 32
         6 <= M <= 512
         1 <= CLKDIV <= 256
         PCLKDIV = (1, 2, 4, 8)

         For FOSC = 12MHz, CCLK = 100MHz we have:
         FCCO = 300MHz, M = 25, N = 3, CLKDIV = 3

         MSEL = M - 1
         NSEL = N - 1
        */
        #define __MSEL (M - 1)
        #define __NSEL (N - 1)
        #define __CLKDIV (CLKDIV)
    #else
        #include "sys_init_lpc17xx__.h"
    #endif

    #define __PLL0CFG_Val ((__MSEL) | (__NSEL) << 16)
    #define __CCLKCFG_Val (__CLKDIV - 1)
#endif


// Set up peripheral clock
#if ! defined(PCLK)
    #define PCLK CCLK
#endif

//#if (((CCLK / PCLK) * PCLK) != CCLK)
#if (((CCLK / PCLK) != 1) && ((CCLK / PCLK) != 2) && ((CCLK / PCLK) != 4) && ((CCLK / PCLK) != 8))
    #error PCLK out of range! (CCLK / 1, 2, 4, 8)
#endif

/*
Peripheral Clocks
PWM1_CLK
I2C0_CLK
I2C1_CLK
I2C2_CLK
I2S_CLK
SPI_CLK
SSP0_CLK
SSP1_CLK
DAC_CLK
ADC_CLK
CAN1_CLK
CAN2_CLK
ACF_CLK
QEI_CLK
GPIO_CLK
PCB_CLK
TIMER0_CLK
TIMER1_CLK
TIMER2_CLK
TIMER3_CLK
UART0_CLK
UART1_CLK
UART2_CLK
UART3_CLK
RIT_CLK
SYSCON_CLK
MC_CLK
WDT_CLK

*/


#if ! defined(WDT_CLK)
    #define WDT_CLK PCLK
#endif
#define WDT_CLK_DIV (CCLK / WDT_CLK)
#if (WDT_CLK_DIV == 1)
    #define WDT_CLK_DIV_SEL 1
#elif (WDT_CLK_DIV == 2)
    #define WDT_CLK_DIV_SEL 2
#elif (WDT_CLK_DIV == 4)
    #define WDT_CLK_DIV_SEL 0
#elif (WDT_CLK_DIV == 8)
    #define WDT_CLK_DIV_SEL 3
#else
    #error WDT_CLK out of range! (CCLK / 1, 2, 4, 8)
#endif


#if ! defined(TIMER0_CLK)
    #define TIMER0_CLK PCLK
#endif
#define TIMER0_CLK_DIV (CCLK / TIMER0_CLK)
#if (TIMER0_CLK_DIV == 1)
    #define TIMER0_CLK_DIV_SEL 1
#elif (TIMER0_CLK_DIV == 2)
    #define TIMER0_CLK_DIV_SEL 2
#elif (TIMER0_CLK_DIV == 4)
    #define TIMER0_CLK_DIV_SEL 0
#elif (TIMER0_CLK_DIV == 8)
    #define TIMER0_CLK_DIV_SEL 3
#else
    #error TIMER0_CLK out of range! (CCLK / 1, 2, 4, 8)
#endif

#if ! defined(TIMER1_CLK)
    #define TIMER1_CLK PCLK
#endif
#define TIMER1_CLK_DIV (CCLK / TIMER1_CLK)
#if (TIMER1_CLK_DIV == 1)
    #define TIMER1_CLK_DIV_SEL 1
#elif (TIMER1_CLK_DIV == 2)
    #define TIMER1_CLK_DIV_SEL 2
#elif (TIMER1_CLK_DIV == 4)
    #define TIMER1_CLK_DIV_SEL 0
#elif (TIMER1_CLK_DIV == 8)
    #define TIMER1_CLK_DIV_SEL 3
#else
    #error TIMER1_CLK out of range! (CCLK / 1, 2, 4, 8)
#endif

#if ! defined(UART0_CLK)
    #define UART0_CLK PCLK
#endif
#define UART0_CLK_DIV (CCLK / UART0_CLK)
#if (UART0_CLK_DIV == 1)
    #define UART0_CLK_DIV_SEL 1
#elif (UART0_CLK_DIV == 2)
    #define UART0_CLK_DIV_SEL 2
#elif (UART0_CLK_DIV == 4)
    #define UART0_CLK_DIV_SEL 0
#elif (UART0_CLK_DIV == 8)
    #define UART0_CLK_DIV_SEL 3
#else
    #error UART0_CLK out of range! (CCLK / 1, 2, 4, 8)
#endif

#if ! defined(UART1_CLK)
    #define UART1_CLK PCLK
#endif
#define UART1_CLK_DIV (CCLK / UART1_CLK)
#if (UART1_CLK_DIV == 1)
    #define UART1_CLK_DIV_SEL 1
#elif (UART1_CLK_DIV == 2)
    #define UART1_CLK_DIV_SEL 2
#elif (UART1_CLK_DIV == 4)
    #define UART1_CLK_DIV_SEL 0
#elif (UART1_CLK_DIV == 8)
    #define UART1_CLK_DIV_SEL 3
#else
    #error UART1_CLK out of range! (CCLK / 1, 2, 4, 8)
#endif

#if ! defined(PWM1_CLK)
    #define PWM1_CLK PCLK
#endif
#define PWM1_CLK_DIV (CCLK / PWM1_CLK)
#if (PWM1_CLK_DIV == 1)
    #define PWM1_CLK_DIV_SEL 1
#elif (PWM1_CLK_DIV == 2)
    #define PWM1_CLK_DIV_SEL 2
#elif (PWM1_CLK_DIV == 4)
    #define PWM1_CLK_DIV_SEL 0
#elif (PWM1_CLK_DIV == 8)
    #define PWM1_CLK_DIV_SEL 3
#else
    #error PWM1_CLK out of range! (CCLK / 1, 2, 4, 8)
#endif

#if ! defined(I2C0_CLK)
    #define I2C0_CLK PCLK
#endif
#define I2C0_CLK_DIV (CCLK / I2C0_CLK)
#if (I2C0_CLK_DIV == 1)
    #define I2C0_CLK_DIV_SEL 1
#elif (I2C0_CLK_DIV == 2)
    #define I2C0_CLK_DIV_SEL 2
#elif (I2C0_CLK_DIV == 4)
    #define I2C0_CLK_DIV_SEL 0
#elif (I2C0_CLK_DIV == 8)
    #define I2C0_CLK_DIV_SEL 3
#else
    #error I2C0_CLK out of range! (CCLK / 1, 2, 4, 8)
#endif

#if ! defined(SPI_CLK)
    #define SPI_CLK PCLK
#endif
#define SPI_CLK_DIV (CCLK / SPI_CLK)
#if (SPI_CLK_DIV == 1)
    #define SPI_CLK_DIV_SEL 1
#elif (SPI_CLK_DIV == 2)
    #define SPI_CLK_DIV_SEL 2
#elif (SPI_CLK_DIV == 4)
    #define SPI_CLK_DIV_SEL 0
#elif (SPI_CLK_DIV == 8)
    #define SPI_CLK_DIV_SEL 3
#else
    #error SPI_CLK out of range! (CCLK / 1, 2, 4, 8)
#endif

#if ! defined(SSP1_CLK)
    #define SSP1_CLK PCLK
#endif
#define SSP1_CLK_DIV (CCLK / SSP1_CLK)
#if (SSP1_CLK_DIV == 1)
    #define SSP1_CLK_DIV_SEL 1
#elif (SSP1_CLK_DIV == 2)
    #define SSP1_CLK_DIV_SEL 2
#elif (SSP1_CLK_DIV == 4)
    #define SSP1_CLK_DIV_SEL 0
#elif (SSP1_CLK_DIV == 8)
    #define SSP1_CLK_DIV_SEL 3
#else
    #error SSP1_CLK out of range! (CCLK / 1, 2, 4, 8)
#endif

#if ! defined(DAC_CLK)
    #define DAC_CLK PCLK
#endif
#define DAC_CLK_DIV (CCLK / DAC_CLK)
#if (DAC_CLK_DIV == 1)
    #define DAC_CLK_DIV_SEL 1
#elif (DAC_CLK_DIV == 2)
    #define DAC_CLK_DIV_SEL 2
#elif (DAC_CLK_DIV == 4)
    #define DAC_CLK_DIV_SEL 0
#elif (DAC_CLK_DIV == 8)
    #define DAC_CLK_DIV_SEL 3
#else
    #error DAC_CLK out of range! (CCLK / 1, 2, 4, 8)
#endif

#if ! defined(ADC_CLK)
    #define ADC_CLK PCLK
#endif
#define ADC_CLK_DIV (CCLK / ADC_CLK)
#if (ADC_CLK_DIV == 1)
    #define ADC_CLK_DIV_SEL 1
#elif (ADC_CLK_DIV == 2)
    #define ADC_CLK_DIV_SEL 2
#elif (ADC_CLK_DIV == 4)
    #define ADC_CLK_DIV_SEL 0
#elif (ADC_CLK_DIV == 8)
    #define ADC_CLK_DIV_SEL 3
#else
    #error ADC_CLK out of range! (CCLK / 1, 2, 4, 8)
#endif

#if ! defined(CAN1_CLK)
    #define CAN1_CLK PCLK
#endif
#define CAN1_CLK_DIV (CCLK / CAN1_CLK)
#if (CAN1_CLK_DIV == 1)
    #define CAN1_CLK_DIV_SEL 1
#elif (CAN1_CLK_DIV == 2)
    #define CAN1_CLK_DIV_SEL 2
#elif (CAN1_CLK_DIV == 4)
    #define CAN1_CLK_DIV_SEL 0
#elif (CAN1_CLK_DIV == 6)
    #define CAN1_CLK_DIV_SEL 3
#else
    #error CAN1_CLK out of range! (CCLK / 1, 2, 4, 6)
#endif

#if ! defined(CAN2_CLK)
    #define CAN2_CLK PCLK
#endif
#define CAN2_CLK_DIV (CCLK / CAN2_CLK)
#if (CAN2_CLK_DIV == 1)
    #define CAN2_CLK_DIV_SEL 1
#elif (CAN2_CLK_DIV == 2)
    #define CAN2_CLK_DIV_SEL 2
#elif (CAN2_CLK_DIV == 4)
    #define CAN2_CLK_DIV_SEL 0
#elif (CAN2_CLK_DIV == 6)
    #define CAN2_CLK_DIV_SEL 3
#else
    #error CAN2_CLK out of range! (CCLK / 1, 2, 4, 6)
#endif

#if ! defined(ACF_CLK)
    #define ACF_CLK PCLK
#endif
#define ACF_CLK_DIV (CCLK / ACF_CLK)
#if (ACF_CLK_DIV == 1)
    #define ACF_CLK_DIV_SEL 1
#elif (ACF_CLK_DIV == 2)
    #define ACF_CLK_DIV_SEL 2
#elif (ACF_CLK_DIV == 4)
    #define ACF_CLK_DIV_SEL 0
#elif (ACF_CLK_DIV == 6)
    #define ACF_CLK_DIV_SEL 3
#else
    #error ACF_CLK out of range! (CCLK / 1, 2, 4, 6)
#endif

#if ! defined(QEI_CLK)
    #define QEI_CLK PCLK
#endif
#define QEI_CLK_DIV (CCLK / QEI_CLK)
#if (QEI_CLK_DIV == 1)
    #define QEI_CLK_DIV_SEL 1
#elif (QEI_CLK_DIV == 2)
    #define QEI_CLK_DIV_SEL 2
#elif (QEI_CLK_DIV == 4)
    #define QEI_CLK_DIV_SEL 0
#elif (QEI_CLK_DIV == 8)
    #define QEI_CLK_DIV_SEL 3
#else
    #error QEI_CLK out of range! (CCLK / 1, 2, 4, 8)
#endif

#if ! defined(GPIO_CLK)
    #define GPIO_CLK PCLK
#endif
#define GPIO_CLK_DIV (CCLK / GPIO_CLK)
#if (GPIO_CLK_DIV == 1)
    #define GPIO_CLK_DIV_SEL 1
#elif (GPIO_CLK_DIV == 2)
    #define GPIO_CLK_DIV_SEL 2
#elif (GPIO_CLK_DIV == 4)
    #define GPIO_CLK_DIV_SEL 0
#elif (GPIO_CLK_DIV == 8)
    #define GPIO_CLK_DIV_SEL 3
#else
    #error GPIO_CLK out of range! (CCLK / 1, 2, 4, 8)
#endif

#if ! defined(PCB_CLK)
    #define PCB_CLK PCLK
#endif
#define PCB_CLK_DIV (CCLK / PCB_CLK)
#if (PCB_CLK_DIV == 1)
    #define PCB_CLK_DIV_SEL 1
#elif (PCB_CLK_DIV == 2)
    #define PCB_CLK_DIV_SEL 2
#elif (PCB_CLK_DIV == 4)
    #define PCB_CLK_DIV_SEL 0
#elif (PCB_CLK_DIV == 8)
    #define PCB_CLK_DIV_SEL 3
#else
    #error PCB_CLK out of range! (CCLK / 1, 2, 4, 8)
#endif

#if ! defined(I2C1_CLK)
    #define I2C1_CLK PCLK
#endif
#define I2C1_CLK_DIV (CCLK / I2C1_CLK)
#if (I2C1_CLK_DIV == 1)
    #define I2C1_CLK_DIV_SEL 1
#elif (I2C1_CLK_DIV == 2)
    #define I2C1_CLK_DIV_SEL 2
#elif (I2C1_CLK_DIV == 4)
    #define I2C1_CLK_DIV_SEL 0
#elif (I2C1_CLK_DIV == 8)
    #define I2C1_CLK_DIV_SEL 3
#else
    #error I2C1_CLK out of range! (CCLK / 1, 2, 4, 8)
#endif

#if ! defined(SSP0_CLK)
    #define SSP0_CLK PCLK
#endif
#define SSP0_CLK_DIV (CCLK / SSP0_CLK)
#if (SSP0_CLK_DIV == 1)
    #define SSP0_CLK_DIV_SEL 1
#elif (SSP0_CLK_DIV == 2)
    #define SSP0_CLK_DIV_SEL 2
#elif (SSP0_CLK_DIV == 4)
    #define SSP0_CLK_DIV_SEL 0
#elif (SSP0_CLK_DIV == 8)
    #define SSP0_CLK_DIV_SEL 3
#else
    #error SSP0_CLK out of range! (CCLK / 1, 2, 4, 8)
#endif

#if ! defined(TIMER2_CLK)
    #define TIMER2_CLK PCLK
#endif
#define TIMER2_CLK_DIV (CCLK / TIMER2_CLK)
#if (TIMER2_CLK_DIV == 1)
    #define TIMER2_CLK_DIV_SEL 1
#elif (TIMER2_CLK_DIV == 2)
    #define TIMER2_CLK_DIV_SEL 2
#elif (TIMER2_CLK_DIV == 4)
    #define TIMER2_CLK_DIV_SEL 0
#elif (TIMER2_CLK_DIV == 8)
    #define TIMER2_CLK_DIV_SEL 3
#else
    #error TIMER2_CLK out of range! (CCLK / 1, 2, 4, 8)
#endif

#if ! defined(TIMER3_CLK)
    #define TIMER3_CLK PCLK
#endif
#define TIMER3_CLK_DIV (CCLK / TIMER3_CLK)
#if (TIMER3_CLK_DIV == 1)
    #define TIMER3_CLK_DIV_SEL 1
#elif (TIMER3_CLK_DIV == 2)
    #define TIMER3_CLK_DIV_SEL 2
#elif (TIMER3_CLK_DIV == 4)
    #define TIMER3_CLK_DIV_SEL 0
#elif (TIMER3_CLK_DIV == 8)
    #define TIMER3_CLK_DIV_SEL 3
#else
    #error TIMER3_CLK out of range! (CCLUART2K / 1, 2, 4, 8)
#endif

#if ! defined(UART2_CLK)
    #define UART2_CLK PCLK
#endif
#define UART2_CLK_DIV (CCLK / UART2_CLK)
#if (UART2_CLK_DIV == 1)
    #define UART2_CLK_DIV_SEL 1
#elif (UART2_CLK_DIV == 2)
    #define UART2_CLK_DIV_SEL 2
#elif (UART2_CLK_DIV == 4)
    #define UART2_CLK_DIV_SEL 0
#elif (UART2_CLK_DIV == 8)
    #define UART2_CLK_DIV_SEL 3
#else
    #error UART2_CLK out of range! (CCLK / 1, 2, 4, 8)
#endif

#if ! defined(UART3_CLK)
    #define UART3_CLK PCLK
#endif
#define UART3_CLK_DIV (CCLK / UART3_CLK)
#if (UART3_CLK_DIV == 1)
    #define UART3_CLK_DIV_SEL 1
#elif (UART3_CLK_DIV == 2)
    #define UART3_CLK_DIV_SEL 2
#elif (UART3_CLK_DIV == 4)
    #define UART3_CLK_DIV_SEL 0
#elif (UART3_CLK_DIV == 8)
    #define UART3_CLK_DIV_SEL 3
#else
    #error UART3_CLK out of range! (CCLK / 1, 2, 4, 8)
#endif

#if ! defined(I2C2_CLK)
    #define I2C2_CLK PCLK
#endif
#define I2C2_CLK_DIV (CCLK / I2C2_CLK)
#if (I2C2_CLK_DIV == 1)
    #define I2C2_CLK_DIV_SEL 1
#elif (I2C2_CLK_DIV == 2)
    #define I2C2_CLK_DIV_SEL 2
#elif (I2C2_CLK_DIV == 4)
    #define I2C2_CLK_DIV_SEL 0
#elif (I2C2_CLK_DIV == 8)
    #define I2C2_CLK_DIV_SEL 3
#else
    #error I2C2_CLK out of range! (CCLK / 1, 2, 4, 8)
#endif

#if ! defined(I2S_CLK)
    #define I2S_CLK PCLK
#endif
#define I2S_CLK_DIV (CCLK / I2S_CLK)
#if (I2S_CLK_DIV == 1)
    #define I2S_CLK_DIV_SEL 1
#elif (I2S_CLK_DIV == 2)
    #define I2S_CLK_DIV_SEL 2
#elif (I2S_CLK_DIV == 4)
    #define I2S_CLK_DIV_SEL 0
#elif (I2S_CLK_DIV == 8)
    #define I2S_CLK_DIV_SEL 3
#else
    #error I2S_CLK out of range! (CCLK / 1, 2, 4, 8)
#endif

#if ! defined(RIT_CLK)
    #define RIT_CLK PCLK
#endif
#define RIT_CLK_DIV (CCLK / RIT_CLK)
#if (RIT_CLK_DIV == 1)
    #define RIT_CLK_DIV_SEL 1
#elif (RIT_CLK_DIV == 2)
    #define RIT_CLK_DIV_SEL 2
#elif (RIT_CLK_DIV == 4)
    #define RIT_CLK_DIV_SEL 0
#elif (RIT_CLK_DIV == 8)
    #define RIT_CLK_DIV_SEL 3
#else
    #error RIT_CLK out of range! (CCLK / 1, 2, 4, 8)
#endif

#if ! defined(SYSCON_CLK)
    #define SYSCON_CLK PCLK
#endif
#define SYSCON_CLK_DIV (CCLK / SYSCON_CLK)
#if (SYSCON_CLK_DIV == 1)
    #define SYSCON_CLK_DIV_SEL 1
#elif (SYSCON_CLK_DIV == 2)
    #define SYSCON_CLK_DIV_SEL 2
#elif (SYSCON_CLK_DIV == 4)
    #define SYSCON_CLK_DIV_SEL 0
#elif (SYSCON_CLK_DIV == 8)
    #define SYSCON_CLK_DIV_SEL 3
#else
    #error SYSCON_CLK out of range! (CCLK / 1, 2, 4, 8)
#endif

#if ! defined(MC_CLK)
    #define MC_CLK PCLK
#endif
#define MC_CLK_DIV (CCLK / MC_CLK)
#if (MC_CLK_DIV == 1)
    #define MC_CLK_DIV_SEL 1
#elif (MC_CLK_DIV == 2)
    #define MC_CLK_DIV_SEL 2
#elif (MC_CLK_DIV == 4)
    #define MC_CLK_DIV_SEL 0
#elif (MC_CLK_DIV == 8)
    #define MC_CLK_DIV_SEL 3
#else
    #error MC_CLK out of range! (CCLK / 1, 2, 4, 8)
#endif

#if (CAN1_CLK_DIV_SEL != CAN2_CLK_DIV_SEL) || (CAN1_CLK_DIV_SEL != ACF_CLK_DIV_SEL)
    #error CAN1, CAN2 and ACF must have the same PCLK divide value when the CAN function is used
#endif

#define __PCLKSEL0_Val ((WDT_CLK_DIV_SEL << 0) | (TIMER0_CLK_DIV_SEL << 2) | \
    (TIMER1_CLK_DIV_SEL << 4) | (UART0_CLK_DIV_SEL << 6) | (UART1_CLK_DIV_SEL << 8) | \
    (PWM1_CLK_DIV_SEL << 12) | (I2C0_CLK_DIV_SEL << 14) | (SPI_CLK_DIV_SEL << 16) | \
    (SSP1_CLK_DIV_SEL << 20) | (DAC_CLK_DIV_SEL << 22) | (ADC_CLK_DIV_SEL << 24) | \
    (CAN1_CLK_DIV_SEL << 26) | (CAN2_CLK_DIV_SEL << 28) | (ACF_CLK_DIV_SEL << 30))

#define __PCLKSEL1_Val ((QEI_CLK_DIV_SEL << 0) | (GPIO_CLK_DIV_SEL << 2) | \
    (PCB_CLK_DIV_SEL << 4) | (I2C1_CLK_DIV_SEL << 6) | (SSP0_CLK_DIV_SEL << 10) | \
    (TIMER2_CLK_DIV_SEL << 12) | (TIMER3_CLK_DIV_SEL << 14) | (UART2_CLK_DIV_SEL << 16) | \
    (UART3_CLK_DIV_SEL << 18) | (I2C2_CLK_DIV_SEL << 20) | (I2S_CLK_DIV_SEL << 22) | \
    (RIT_CLK_DIV_SEL << 26) | (SYSCON_CLK_DIV_SEL << 28) | (MC_CLK_DIV_SEL << 30))


// disable all peripherals, power on just GPIO
#define __PCONP_Val PCONP_GPIO

// PINMODE register values
#define PULL_DOWN   (3)
#define PULL_UP     (0)
#define REPEATER    (1)
#define NORMAL_IO   (2)
#define CLRMASK     (~3)

// somewhere in board config files must be defined
// PIOx_PULLUP_BITS
// PIOx_PULLDOWN_BITS
// PIOx_REPEATER_BITS
// PIOx_OPENDRAIN_BITS
// PIOx_ADC_BITS
// PIOx_DAC_BITS
// PIOx_OUTPUT_BITS

// some of these settings are mutually exclusive
#if (PIO0_PULLUP_BITS & PIO0_PULLDOWN_BITS) | (PIO0_PULLUP_BITS & PIO0_REPEATER_BITS) | (PIO0_REPEATER_BITS & PIO0_PULLDOWN_BITS)
	#error "Bad pin configuration for port 0"
#endif

#if (PIO1_PULLUP_BITS & PIO1_PULLDOWN_BITS) | (PIO1_PULLUP_BITS & PIO1_REPEATER_BITS) | (PIO1_REPEATER_BITS & PIO1_PULLDOWN_BITS)
	#error "Bad pin configuration for port 1"
#endif

#if (PIO2_PULLUP_BITS & PIO2_PULLDOWN_BITS) | (PIO2_PULLUP_BITS & PIO2_REPEATER_BITS) | (PIO2_REPEATER_BITS & PIO2_PULLDOWN_BITS)
	#error "Bad pin configuration for port 2"
#endif

#if (PIO3_PULLUP_BITS & PIO3_PULLDOWN_BITS) | (PIO3_PULLUP_BITS & PIO3_REPEATER_BITS) | (PIO3_REPEATER_BITS & PIO3_PULLDOWN_BITS)
	#error "Bad pin configuration for port 3"
#endif

#if (PIO4_PULLUP_BITS & PIO4_PULLDOWN_BITS) | (PIO4_PULLUP_BITS & PIO4_REPEATER_BITS) | (PIO4_REPEATER_BITS & PIO4_PULLDOWN_BITS)
	#error "Bad pin configuration for port 4"
#endif

// all special IO options and all ADC/DAC IO options are mutually exclusive
#if ((PIO0_PULLUP_BITS | PIO0_PULLDOWN_BITS | PIO0_REPEATER_BITS) & (PIO0_ADC_BITS | PIO0_DAC_BITS))
	#error "Bad pin configuration for port 0"
#endif

#if ((PIO1_PULLUP_BITS | PIO1_PULLDOWN_BITS | PIO1_REPEATER_BITS) & (PIO1_ADC_BITS | PIO1_DAC_BITS))
	#error "Bad pin configuration for port 1"
#endif

#if ((PIO2_PULLUP_BITS | PIO2_PULLDOWN_BITS | PIO2_REPEATER_BITS) & (PIO2_ADC_BITS | PIO2_DAC_BITS))
	#error "Bad pin configuration for port 2"
#endif

#if ((PIO3_PULLUP_BITS | PIO3_PULLDOWN_BITS | PIO3_REPEATER_BITS) & (PIO3_ADC_BITS | PIO3_DAC_BITS))
	#error "Bad pin configuration for port 3"
#endif

#if ((PIO4_PULLUP_BITS | PIO4_PULLDOWN_BITS | PIO4_REPEATER_BITS) & (PIO4_ADC_BITS | PIO4_DAC_BITS))
	#error "Bad pin configuration for port 4"
#endif


// configure pin mode
#if (PIO0_PULLUP_BITS & BIT(0))
	#define __PINMODE0_0 (PULL_UP << 0)
#elif (PIO0_PULLDOWN_BITS & BIT(0))
	#define __PINMODE0_0 (PULL_DOWN << 0)
#elif (PIO0_REPEATER_BITS & BIT(0))
	#define __PINMODE0_0 (REPEATER << 0)
#else
	#define __PINMODE0_0 (NORMAL_IO << 0)
#endif

#if (PIO0_PULLUP_BITS & BIT(1))
	#define __PINMODE0_2 (PULL_UP << 2)
#elif (PIO0_PULLDOWN_BITS & BIT(1))
	#define __PINMODE0_2 (PULL_DOWN << 2)
#elif (PIO0_REPEATER_BITS & BIT(1))
	#define __PINMODE0_2 (REPEATER << 2)
#else
	#define __PINMODE0_2 (NORMAL_IO << 2)
#endif

#if (PIO0_PULLUP_BITS & BIT(2))
	#define __PINMODE0_4 (PULL_UP << 4)
#elif (PIO0_PULLDOWN_BITS & BIT(2))
	#define __PINMODE0_4 (PULL_DOWN << 4)
#elif (PIO0_REPEATER_BITS & BIT(2))
	#define __PINMODE0_4 (REPEATER << 4)
#else
	#define __PINMODE0_4 (NORMAL_IO << 4)
#endif

#if (PIO0_PULLUP_BITS & BIT(3))
	#define __PINMODE0_6 (PULL_UP << 6)
#elif (PIO0_PULLDOWN_BITS & BIT(3))
	#define __PINMODE0_6 (PULL_DOWN << 6)
#elif (PIO0_REPEATER_BITS & BIT(3))
	#define __PINMODE0_6 (REPEATER << 6)
#else
	#define __PINMODE0_6 (NORMAL_IO << 6)
#endif

#if defined(LPC176x)
    #if (PIO0_PULLUP_BITS &     BIT(4))
        #define __PINMODE0_8 (PULL_UP <<   8)
    #elif (PIO0_PULLDOWN_BITS & BIT(4))
        #define __PINMODE0_8 (PULL_DOWN << 8)
    #elif (PIO0_REPEATER_BITS & BIT(4))
        #define __PINMODE0_8 (REPEATER <<  8)
    #else
        #define __PINMODE0_8 (NORMAL_IO << 8)
    #endif

    #if (PIO0_PULLUP_BITS &     BIT(5))
        #define __PINMODE0_10 (PULL_UP <<   10)
    #elif (PIO0_PULLDOWN_BITS & BIT(5))
        #define __PINMODE0_10 (PULL_DOWN << 10)
    #elif (PIO0_REPEATER_BITS & BIT(5))
        #define __PINMODE0_10 (REPEATER <<  10)
    #else
        #define __PINMODE0_10 (NORMAL_IO << 10)
    #endif
#else
	#define __PINMODE0_8  0
	#define __PINMODE0_10 0
#endif // defined(LPC176x)

#if (PIO0_PULLUP_BITS &     BIT(6))
	#define __PINMODE0_12 (PULL_UP <<   12)
#elif (PIO0_PULLDOWN_BITS & BIT(6))
	#define __PINMODE0_12 (PULL_DOWN << 12)
#elif (PIO0_REPEATER_BITS & BIT(6))
	#define __PINMODE0_12 (REPEATER <<  12)
#else
	#define __PINMODE0_12 (NORMAL_IO << 12)
#endif

#if (PIO0_PULLUP_BITS &     BIT(7))
	#define __PINMODE0_14 (PULL_UP <<   14)
#elif (PIO0_PULLDOWN_BITS & BIT(7))
	#define __PINMODE0_14 (PULL_DOWN << 14)
#elif (PIO0_REPEATER_BITS & BIT(7))
	#define __PINMODE0_14 (REPEATER <<  14)
#else
	#define __PINMODE0_14 (NORMAL_IO << 14)
#endif


#if (PIO0_PULLUP_BITS &     BIT(8))
	#define __PINMODE0_16 (PULL_UP <<   16)
#elif (PIO0_PULLDOWN_BITS & BIT(8))
	#define __PINMODE0_16 (PULL_DOWN << 16)
#elif (PIO0_REPEATER_BITS & BIT(8))
	#define __PINMODE0_16 (REPEATER <<  16)
#else
	#define __PINMODE0_16 (NORMAL_IO << 16)
#endif

#if (PIO0_PULLUP_BITS &     BIT(9))
	#define __PINMODE0_18 (PULL_UP <<   18)
#elif (PIO0_PULLDOWN_BITS & BIT(9))
	#define __PINMODE0_18 (PULL_DOWN << 18)
#elif (PIO0_REPEATER_BITS & BIT(9))
	#define __PINMODE0_18 (REPEATER <<  18)
#else
	#define __PINMODE0_18 (NORMAL_IO << 18)
#endif

#if (PIO0_PULLUP_BITS &     BIT(10))
	#define __PINMODE0_20 (PULL_UP <<   20)
#elif (PIO0_PULLDOWN_BITS & BIT(10))
	#define __PINMODE0_20 (PULL_DOWN << 20)
#elif (PIO0_REPEATER_BITS & BIT(10))
	#define __PINMODE0_20 (REPEATER <<  20)
#else
	#define __PINMODE0_20 (NORMAL_IO << 20)
#endif

#if (PIO0_PULLUP_BITS &     BIT(11))
	#define __PINMODE0_22 (PULL_UP <<   22)
#elif (PIO0_PULLDOWN_BITS & BIT(11))
	#define __PINMODE0_22 (PULL_DOWN << 22)
#elif (PIO0_REPEATER_BITS & BIT(11))
	#define __PINMODE0_22 (REPEATER <<  22)
#else
	#define __PINMODE0_22 (NORMAL_IO << 22)
#endif


#if (PIO0_PULLUP_BITS &     BIT(15))
	#define __PINMODE0_30 (PULL_UP <<   30)
#elif (PIO0_PULLDOWN_BITS & BIT(15))
	#define __PINMODE0_30 (PULL_DOWN << 30)
#elif (PIO0_REPEATER_BITS & BIT(15))
	#define __PINMODE0_30 (REPEATER <<  30)
#else
	#define __PINMODE0_30 (NORMAL_IO << 30)
#endif




#if (PIO0_PULLUP_BITS &     BIT(16))
	#define __PINMODE1_0 (PULL_UP <<   0)
#elif (PIO0_PULLDOWN_BITS & BIT(16))
	#define __PINMODE1_0 (PULL_DOWN << 0)
#elif (PIO0_REPEATER_BITS & BIT(16))
	#define __PINMODE1_0 (REPEATER <<  0)
#else
	#define __PINMODE1_0 (NORMAL_IO << 0)
#endif

#if (PIO0_PULLUP_BITS &     BIT(17))
	#define __PINMODE1_2 (PULL_UP <<   2)
#elif (PIO0_PULLDOWN_BITS & BIT(17))
	#define __PINMODE1_2 (PULL_DOWN << 2)
#elif (PIO0_REPEATER_BITS & BIT(17))
	#define __PINMODE1_2 (REPEATER <<  2)
#else
	#define __PINMODE1_2 (NORMAL_IO << 2)
#endif

#if (PIO0_PULLUP_BITS &     BIT(18))
	#define __PINMODE1_4 (PULL_UP <<   4)
#elif (PIO0_PULLDOWN_BITS & BIT(18))
	#define __PINMODE1_4 (PULL_DOWN << 4)
#elif (PIO0_REPEATER_BITS & BIT(18))
	#define __PINMODE1_4 (REPEATER <<  4)
#else
	#define __PINMODE1_4 (NORMAL_IO << 4)
#endif

#if defined(LPC176x)
    #if (PIO0_PULLUP_BITS &     BIT(19))
        #define __PINMODE1_6 (PULL_UP <<   6)
    #elif (PIO0_PULLDOWN_BITS & BIT(19))
        #define __PINMODE1_6 (PULL_DOWN << 6)
    #elif (PIO0_REPEATER_BITS & BIT(19))
        #define __PINMODE1_6 (REPEATER <<  6)
    #else
        #define __PINMODE1_6 (NORMAL_IO << 6)
    #endif

    #if (PIO0_PULLUP_BITS &     BIT(20))
        #define __PINMODE1_8 (PULL_UP <<   8)
    #elif (PIO0_PULLDOWN_BITS & BIT(20))
        #define __PINMODE1_8 (PULL_DOWN << 8)
    #elif (PIO0_REPEATER_BITS & BIT(20))
        #define __PINMODE1_8 (REPEATER <<  8)
    #else
        #define __PINMODE1_8 (NORMAL_IO << 8)
    #endif

    #if (PIO0_PULLUP_BITS &     BIT(21))
        #define __PINMODE1_10 (PULL_UP <<   10)
    #elif (PIO0_PULLDOWN_BITS & BIT(21))
        #define __PINMODE1_10 (PULL_DOWN << 10)
    #elif (PIO0_REPEATER_BITS & BIT(21))
        #define __PINMODE1_10 (REPEATER <<  10)
    #else
        #define __PINMODE1_10 (NORMAL_IO << 10)
    #endif
#else
	#define __PINMODE1_6  0
	#define __PINMODE1_8  0
	#define __PINMODE1_10 0
#endif // defined(LPC176x)

#if (PIO0_PULLUP_BITS &     BIT(22))
	#define __PINMODE1_12 (PULL_UP <<   12)
#elif (PIO0_PULLDOWN_BITS & BIT(22))
	#define __PINMODE1_12 (PULL_DOWN << 12)
#elif (PIO0_REPEATER_BITS & BIT(22))
	#define __PINMODE1_12 (REPEATER <<  12)
#else
	#define __PINMODE1_12 (NORMAL_IO << 12)
#endif

#if defined(LPC176x)
    #if (PIO0_PULLUP_BITS &     BIT(23))
        #define __PINMODE1_14 (PULL_UP <<   14)
    #elif (PIO0_PULLDOWN_BITS & BIT(23))
        #define __PINMODE1_14 (PULL_DOWN << 14)
    #elif (PIO0_REPEATER_BITS & BIT(23))
        #define __PINMODE1_14 (REPEATER <<  14)
    #else
        #define __PINMODE1_14 (NORMAL_IO << 14)
    #endif

    #if (PIO0_PULLUP_BITS &     BIT(24))
        #define __PINMODE1_16 (PULL_UP <<   16)
    #elif (PIO0_PULLDOWN_BITS & BIT(24))
        #define __PINMODE1_16 (PULL_DOWN << 16)
    #elif (PIO0_REPEATER_BITS & BIT(24))
        #define __PINMODE1_16 (REPEATER <<  16)
    #else
        #define __PINMODE1_16 (NORMAL_IO << 16)
    #endif
#else
	#define __PINMODE1_14 0
	#define __PINMODE1_16 0
#endif // defined(LPC176x)


#if (PIO0_PULLUP_BITS &     BIT(25))
	#define __PINMODE1_18 (PULL_UP <<   18)
#elif (PIO0_PULLDOWN_BITS & BIT(25))
	#define __PINMODE1_18 (PULL_DOWN << 18)
#elif (PIO0_REPEATER_BITS & BIT(25))
	#define __PINMODE1_18 (REPEATER <<  18)
#else
	#define __PINMODE1_18 (NORMAL_IO << 18)
#endif

#if (PIO0_PULLUP_BITS &     BIT(26))
	#define __PINMODE1_20 (PULL_UP <<   20)
#elif (PIO0_PULLDOWN_BITS & BIT(26))
	#define __PINMODE1_20 (PULL_DOWN << 20)
#elif (PIO0_REPEATER_BITS & BIT(26))
	#define __PINMODE1_20 (REPEATER <<  20)
#else
	#define __PINMODE1_20 (NORMAL_IO << 20)
#endif



// PIO1

#if (PIO1_PULLUP_BITS &     BIT(0))
	#define __PINMODE2_0 (PULL_UP <<   0)
#elif (PIO1_PULLDOWN_BITS & BIT(0))
	#define __PINMODE2_0 (PULL_DOWN << 0)
#elif (PIO1_REPEATER_BITS & BIT(0))
	#define __PINMODE2_0 (REPEATER <<  0)
#else
	#define __PINMODE2_0 (NORMAL_IO << 0)
#endif

#if (PIO1_PULLUP_BITS &     BIT(1))
	#define __PINMODE2_2 (PULL_UP <<   2)
#elif (PIO1_PULLDOWN_BITS & BIT(1))
	#define __PINMODE2_2 (PULL_DOWN << 2)
#elif (PIO1_REPEATER_BITS & BIT(1))
	#define __PINMODE2_2 (REPEATER <<  2)
#else
	#define __PINMODE2_2 (NORMAL_IO << 2)
#endif

#if (PIO1_PULLUP_BITS &     BIT(4))
	#define __PINMODE2_8 (PULL_UP <<   8)
#elif (PIO1_PULLDOWN_BITS & BIT(4))
	#define __PINMODE2_8 (PULL_DOWN << 8)
#elif (PIO1_REPEATER_BITS & BIT(4))
	#define __PINMODE2_8 (REPEATER <<  8)
#else
	#define __PINMODE2_8 (NORMAL_IO << 8)
#endif

#if (PIO1_PULLUP_BITS &     BIT(8))
	#define __PINMODE2_16 (PULL_UP <<   16)
#elif (PIO1_PULLDOWN_BITS & BIT(8))
	#define __PINMODE2_16 (PULL_DOWN << 16)
#elif (PIO1_REPEATER_BITS & BIT(8))
	#define __PINMODE2_16 (REPEATER <<  16)
#else
	#define __PINMODE2_16 (NORMAL_IO << 16)
#endif

#if (PIO1_PULLUP_BITS &     BIT(9))
	#define __PINMODE2_18 (PULL_UP <<   18)
#elif (PIO1_PULLDOWN_BITS & BIT(9))
	#define __PINMODE2_18 (PULL_DOWN << 18)
#elif (PIO1_REPEATER_BITS & BIT(9))
	#define __PINMODE2_18 (REPEATER <<  18)
#else
	#define __PINMODE2_18 (NORMAL_IO << 18)
#endif

#if (PIO1_PULLUP_BITS &     BIT(10))
	#define __PINMODE2_20 (PULL_UP <<   20)
#elif (PIO1_PULLDOWN_BITS & BIT(10))
	#define __PINMODE2_20 (PULL_DOWN << 20)
#elif (PIO1_REPEATER_BITS & BIT(10))
	#define __PINMODE2_20 (REPEATER <<  20)
#else
	#define __PINMODE2_20 (NORMAL_IO << 20)
#endif

#if (PIO1_PULLUP_BITS &     BIT(14))
	#define __PINMODE2_28 (PULL_UP <<   28)
#elif (PIO1_PULLDOWN_BITS & BIT(14))
	#define __PINMODE2_28 (PULL_DOWN << 28)
#elif (PIO1_REPEATER_BITS & BIT(14))
	#define __PINMODE2_28 (REPEATER <<  28)
#else
	#define __PINMODE2_28 (NORMAL_IO << 28)
#endif

#if (PIO1_PULLUP_BITS &     BIT(15))
	#define __PINMODE2_30 (PULL_UP <<   30)
#elif (PIO1_PULLDOWN_BITS & BIT(15))
	#define __PINMODE2_30 (PULL_DOWN << 30)
#elif (PIO1_REPEATER_BITS & BIT(15))
	#define __PINMODE2_30 (REPEATER <<  30)
#else
	#define __PINMODE2_30 (NORMAL_IO << 30)
#endif



#if defined(LPC176x)
    #if (PIO1_PULLUP_BITS &     BIT(16))
        #define __PINMODE3_0 (PULL_UP <<   0)
    #elif (PIO1_PULLDOWN_BITS & BIT(16))
        #define __PINMODE3_0 (PULL_DOWN << 0)
    #elif (PIO1_REPEATER_BITS & BIT(16))
        #define __PINMODE3_0 (REPEATER <<  0)
    #else
        #define __PINMODE3_0 (NORMAL_IO << 0)
    #endif

    #if (PIO1_PULLUP_BITS &     BIT(17))
        #define __PINMODE3_2 (PULL_UP <<   2)
    #elif (PIO1_PULLDOWN_BITS & BIT(17))
        #define __PINMODE3_2 (PULL_DOWN << 2)
    #elif (PIO1_REPEATER_BITS & BIT(17))
        #define __PINMODE3_2 (REPEATER <<  2)
    #else
        #define __PINMODE3_2 (NORMAL_IO << 2)
    #endif
#else
	#define __PINMODE3_0 0
	#define __PINMODE3_2 0
#endif // defined(LPC176x)

#if (PIO1_PULLUP_BITS &     BIT(18))
	#define __PINMODE3_4 (PULL_UP <<   4)
#elif (PIO1_PULLDOWN_BITS & BIT(18))
	#define __PINMODE3_4 (PULL_DOWN << 4)
#elif (PIO1_REPEATER_BITS & BIT(18))
	#define __PINMODE3_4 (REPEATER <<  4)
#else
	#define __PINMODE3_4 (NORMAL_IO << 4)
#endif

#if (PIO1_PULLUP_BITS &     BIT(19))
	#define __PINMODE3_6 (PULL_UP <<   6)
#elif (PIO1_PULLDOWN_BITS & BIT(19))
	#define __PINMODE3_6 (PULL_DOWN << 6)
#elif (PIO1_REPEATER_BITS & BIT(19))
	#define __PINMODE3_6 (REPEATER <<  6)
#else
	#define __PINMODE3_6 (NORMAL_IO << 6)
#endif

#if (PIO1_PULLUP_BITS &     BIT(20))
	#define __PINMODE3_8 (PULL_UP <<   8)
#elif (PIO1_PULLDOWN_BITS & BIT(20))
	#define __PINMODE3_8 (PULL_DOWN << 8)
#elif (PIO1_REPEATER_BITS & BIT(20))
	#define __PINMODE3_8 (REPEATER <<  8)
#else
	#define __PINMODE3_8 (NORMAL_IO << 8)
#endif

#if defined(LPC176x)
    #if (PIO1_PULLUP_BITS &     BIT(21))
        #define __PINMODE3_10 (PULL_UP <<   10)
    #elif (PIO1_PULLDOWN_BITS & BIT(21))
        #define __PINMODE3_10 (PULL_DOWN << 10)
    #elif (PIO1_REPEATER_BITS & BIT(21))
        #define __PINMODE3_10 (REPEATER <<  10)
    #else
        #define __PINMODE3_10 (NORMAL_IO << 10)
    #endif
#else
	#define __PINMODE3_10 0
#endif // defined(LPC176x)

#if (PIO1_PULLUP_BITS &     BIT(22))
	#define __PINMODE3_12 (PULL_UP <<   12)
#elif (PIO1_PULLDOWN_BITS & BIT(22))
	#define __PINMODE3_12 (PULL_DOWN << 12)
#elif (PIO1_REPEATER_BITS & BIT(22))
	#define __PINMODE3_12 (REPEATER <<  12)
#else
	#define __PINMODE3_12 (NORMAL_IO << 12)
#endif

#if (PIO1_PULLUP_BITS &     BIT(23))
	#define __PINMODE3_14 (PULL_UP <<   14)
#elif (PIO1_PULLDOWN_BITS & BIT(23))
	#define __PINMODE3_14 (PULL_DOWN << 14)
#elif (PIO1_REPEATER_BITS & BIT(23))
	#define __PINMODE3_14 (REPEATER <<  14)
#else
	#define __PINMODE3_14 (NORMAL_IO << 14)
#endif

#if (PIO1_PULLUP_BITS &     BIT(24))
	#define __PINMODE3_16 (PULL_UP <<   16)
#elif (PIO1_PULLDOWN_BITS & BIT(24))
	#define __PINMODE3_16 (PULL_DOWN << 16)
#elif (PIO1_REPEATER_BITS & BIT(24))
	#define __PINMODE3_16 (REPEATER <<  16)
#else
	#define __PINMODE3_16 (NORMAL_IO << 16)
#endif

#if (PIO1_PULLUP_BITS &     BIT(25))
	#define __PINMODE3_18 (PULL_UP <<   18)
#elif (PIO1_PULLDOWN_BITS & BIT(25))
	#define __PINMODE3_18 (PULL_DOWN << 18)
#elif (PIO1_REPEATER_BITS & BIT(25))
	#define __PINMODE3_18 (REPEATER <<  18)
#else
	#define __PINMODE3_18 (NORMAL_IO << 18)
#endif

#if (PIO1_PULLUP_BITS &     BIT(26))
	#define __PINMODE3_20 (PULL_UP <<   20)
#elif (PIO1_PULLDOWN_BITS & BIT(26))
	#define __PINMODE3_20 (PULL_DOWN << 20)
#elif (PIO1_REPEATER_BITS & BIT(26))
	#define __PINMODE3_20 (REPEATER <<  20)
#else
	#define __PINMODE3_20 (NORMAL_IO << 20)
#endif

#if defined(LPC176x)
    #if (PIO1_PULLUP_BITS &     BIT(27))
        #define __PINMODE3_22 (PULL_UP <<   22)
    #elif (PIO1_PULLDOWN_BITS & BIT(27))
        #define __PINMODE3_22 (PULL_DOWN << 22)
    #elif (PIO1_REPEATER_BITS & BIT(27))
        #define __PINMODE3_22 (REPEATER <<  22)
    #else
        #define __PINMODE3_22 (NORMAL_IO << 22)
    #endif
#else
	#define __PINMODE3_22 0
#endif // defined(LPC176x)

#if (PIO1_PULLUP_BITS &     BIT(28))
	#define __PINMODE3_24 (PULL_UP <<   24)
#elif (PIO1_PULLDOWN_BITS & BIT(28))
	#define __PINMODE3_24 (PULL_DOWN << 24)
#elif (PIO1_REPEATER_BITS & BIT(28))
	#define __PINMODE3_24 (REPEATER <<  24)
#else
	#define __PINMODE3_24 (NORMAL_IO << 24)
#endif

#if (PIO1_PULLUP_BITS &     BIT(29))
	#define __PINMODE3_26 (PULL_UP <<   26)
#elif (PIO1_PULLDOWN_BITS & BIT(29))
	#define __PINMODE3_26 (PULL_DOWN << 26)
#elif (PIO1_REPEATER_BITS & BIT(29))
	#define __PINMODE3_26 (REPEATER <<  26)
#else
	#define __PINMODE3_26 (NORMAL_IO << 26)
#endif

#if (PIO1_PULLUP_BITS &     BIT(30))
	#define __PINMODE3_28 (PULL_UP <<   28)
#elif (PIO1_PULLDOWN_BITS & BIT(30))
	#define __PINMODE3_28 (PULL_DOWN << 28)
#elif (PIO1_REPEATER_BITS & BIT(30))
	#define __PINMODE3_28 (REPEATER <<  28)
#else
	#define __PINMODE3_28 (NORMAL_IO << 28)
#endif

#if (PIO1_PULLUP_BITS &     BIT(31))
	#define __PINMODE3_30 (PULL_UP <<   30)
#elif (PIO1_PULLDOWN_BITS & BIT(31))
	#define __PINMODE3_30 (PULL_DOWN << 30)
#elif (PIO1_REPEATER_BITS & BIT(31))
	#define __PINMODE3_30 (REPEATER <<  30)
#else
	#define __PINMODE3_30 (NORMAL_IO << 30)
#endif



// PIO2

#if (PIO2_PULLUP_BITS &     BIT(0))
	#define __PINMODE4_0 (PULL_UP <<   0)
#elif (PIO2_PULLDOWN_BITS & BIT(0))
	#define __PINMODE4_0 (PULL_DOWN << 0)
#elif (PIO2_REPEATER_BITS & BIT(0))
	#define __PINMODE4_0 (REPEATER <<  0)
#else
	#define __PINMODE4_0 (NORMAL_IO << 0)
#endif

#if (PIO2_PULLUP_BITS &     BIT(1))
	#define __PINMODE4_2 (PULL_UP <<   2)
#elif (PIO2_PULLDOWN_BITS & BIT(1))
	#define __PINMODE4_2 (PULL_DOWN << 2)
#elif (PIO2_REPEATER_BITS & BIT(1))
	#define __PINMODE4_2 (REPEATER <<  2)
#else
	#define __PINMODE4_2 (NORMAL_IO << 2)
#endif

#if (PIO2_PULLUP_BITS &     BIT(2))
	#define __PINMODE4_4 (PULL_UP <<   4)
#elif (PIO2_PULLDOWN_BITS & BIT(2))
	#define __PINMODE4_4 (PULL_DOWN << 4)
#elif (PIO2_REPEATER_BITS & BIT(2))
	#define __PINMODE4_4 (REPEATER <<  4)
#else
	#define __PINMODE4_4 (NORMAL_IO << 4)
#endif

#if (PIO2_PULLUP_BITS &     BIT(3))
	#define __PINMODE4_6 (PULL_UP <<   6)
#elif (PIO2_PULLDOWN_BITS & BIT(3))
	#define __PINMODE4_6 (PULL_DOWN << 6)
#elif (PIO2_REPEATER_BITS & BIT(3))
	#define __PINMODE4_6 (REPEATER <<  6)
#else
	#define __PINMODE4_6 (NORMAL_IO << 6)
#endif

#if (PIO2_PULLUP_BITS &     BIT(4))
	#define __PINMODE4_8 (PULL_UP <<   8)
#elif (PIO2_PULLDOWN_BITS & BIT(4))
	#define __PINMODE4_8 (PULL_DOWN << 8)
#elif (PIO2_REPEATER_BITS & BIT(4))
	#define __PINMODE4_8 (REPEATER <<  8)
#else
	#define __PINMODE4_8 (NORMAL_IO << 8)
#endif

#if (PIO2_PULLUP_BITS &     BIT(5))
	#define __PINMODE4_10 (PULL_UP <<   10)
#elif (PIO2_PULLDOWN_BITS & BIT(5))
	#define __PINMODE4_10 (PULL_DOWN << 10)
#elif (PIO2_REPEATER_BITS & BIT(5))
	#define __PINMODE4_10 (REPEATER <<  10)
#else
	#define __PINMODE4_10 (NORMAL_IO << 10)
#endif

#if (PIO2_PULLUP_BITS &     BIT(6))
	#define __PINMODE4_12 (PULL_UP <<   12)
#elif (PIO2_PULLDOWN_BITS & BIT(6))
	#define __PINMODE4_12 (PULL_DOWN << 12)
#elif (PIO2_REPEATER_BITS & BIT(6))
	#define __PINMODE4_12 (REPEATER <<  12)
#else
	#define __PINMODE4_12 (NORMAL_IO << 12)
#endif

#if (PIO2_PULLUP_BITS &     BIT(7))
	#define __PINMODE4_14 (PULL_UP <<   14)
#elif (PIO2_PULLDOWN_BITS & BIT(7))
	#define __PINMODE4_14 (PULL_DOWN << 14)
#elif (PIO2_REPEATER_BITS & BIT(7))
	#define __PINMODE4_14 (REPEATER <<  14)
#else
	#define __PINMODE4_14 (NORMAL_IO << 14)
#endif

#if (PIO2_PULLUP_BITS &     BIT(8))
	#define __PINMODE4_16 (PULL_UP <<   16)
#elif (PIO2_PULLDOWN_BITS & BIT(8))
	#define __PINMODE4_16 (PULL_DOWN << 16)
#elif (PIO2_REPEATER_BITS & BIT(8))
	#define __PINMODE4_16 (REPEATER <<  16)
#else
	#define __PINMODE4_16 (NORMAL_IO << 16)
#endif

#if (PIO2_PULLUP_BITS &     BIT(9))
	#define __PINMODE4_18 (PULL_UP <<   18)
#elif (PIO2_PULLDOWN_BITS & BIT(9))
	#define __PINMODE4_18 (PULL_DOWN << 18)
#elif (PIO2_REPEATER_BITS & BIT(9))
	#define __PINMODE4_18 (REPEATER <<  18)
#else
	#define __PINMODE4_18 (NORMAL_IO << 18)
#endif

#if (PIO2_PULLUP_BITS &     BIT(10))
	#define __PINMODE4_20 (PULL_UP <<   20)
#elif (PIO2_PULLDOWN_BITS & BIT(10))
	#define __PINMODE4_20 (PULL_DOWN << 20)
#elif (PIO2_REPEATER_BITS & BIT(10))
	#define __PINMODE4_20 (REPEATER <<  20)
#else
	#define __PINMODE4_20 (NORMAL_IO << 20)
#endif

#if defined(LPC176x)
    #if (PIO2_PULLUP_BITS &     BIT(11))
        #define __PINMODE4_22 (PULL_UP <<   22)
    #elif (PIO2_PULLDOWN_BITS & BIT(11))
        #define __PINMODE4_22 (PULL_DOWN << 22)
    #elif (PIO2_REPEATER_BITS & BIT(11))
        #define __PINMODE4_22 (REPEATER <<  22)
    #else
        #define __PINMODE4_22 (NORMAL_IO << 22)
    #endif

    #if (PIO2_PULLUP_BITS &     BIT(12))
        #define __PINMODE4_24 (PULL_UP <<   24)
    #elif (PIO2_PULLDOWN_BITS & BIT(12))
        #define __PINMODE4_24 (PULL_DOWN << 24)
    #elif (PIO2_REPEATER_BITS & BIT(12))
        #define __PINMODE4_24 (REPEATER <<  24)
    #else
        #define __PINMODE4_24 (NORMAL_IO << 24)
    #endif

    #if (PIO2_PULLUP_BITS &     BIT(13))
        #define __PINMODE4_26 (PULL_UP <<   26)
    #elif (PIO2_PULLDOWN_BITS & BIT(13))
        #define __PINMODE4_26 (PULL_DOWN << 26)
    #elif (PIO2_REPEATER_BITS & BIT(13))
        #define __PINMODE4_26 (REPEATER <<  26)
    #else
        #define __PINMODE4_26 (NORMAL_IO << 26)
    #endif
#else
	#define __PINMODE4_22 0
	#define __PINMODE4_24 0
	#define __PINMODE4_26 0
#endif // defined(LPC176x)


// PIO3

#if defined(LPC176x)
    #if (PIO3_PULLUP_BITS &     BIT(25))
        #define __PINMODE7_18 (PULL_UP <<   18)
    #elif (PIO3_PULLDOWN_BITS & BIT(25))
        #define __PINMODE7_18 (PULL_DOWN << 18)
    #elif (PIO3_REPEATER_BITS & BIT(25))
        #define __PINMODE7_18 (REPEATER <<  18)
    #else
        #define __PINMODE7_18 (NORMAL_IO << 18)
    #endif

    #if (PIO3_PULLUP_BITS &     BIT(26))
        #define __PINMODE7_20 (PULL_UP <<   20)
    #elif (PIO3_PULLDOWN_BITS & BIT(26))
        #define __PINMODE7_20 (PULL_DOWN << 20)
    #elif (PIO3_REPEATER_BITS & BIT(26))
        #define __PINMODE7_20 (REPEATER <<  20)
    #else
        #define __PINMODE7_20 (NORMAL_IO << 20)
    #endif
#else
	#define __PINMODE7_18 0
	#define __PINMODE7_20 0
#endif // defined(LPC176x)


// PIO4

#if (PIO4_PULLUP_BITS &     BIT(28))
	#define __PINMODE9_24 (PULL_UP <<   24)
#elif (PIO4_PULLDOWN_BITS & BIT(28))
	#define __PINMODE9_24 (PULL_DOWN << 24)
#elif (PIO4_REPEATER_BITS & BIT(28))
	#define __PINMODE9_24 (REPEATER <<  24)
#else
	#define __PINMODE9_24 (NORMAL_IO << 24)
#endif

#if (PIO4_PULLUP_BITS &     BIT(29))
	#define __PINMODE9_26 (PULL_UP <<   26)
#elif (PIO4_PULLDOWN_BITS & BIT(29))
	#define __PINMODE9_26 (PULL_DOWN << 26)
#elif (PIO4_REPEATER_BITS & BIT(29))
	#define __PINMODE9_26 (REPEATER <<  26)
#else
	#define __PINMODE9_26 (NORMAL_IO << 26)
#endif



#define __PINMODE0_val (__PINMODE0_0 | __PINMODE0_2 | __PINMODE0_4 | __PINMODE0_6 | \
__PINMODE0_8 | __PINMODE0_10 | __PINMODE0_12 | __PINMODE0_14 | __PINMODE0_16 | __PINMODE0_18 | \
__PINMODE0_20 | __PINMODE0_22 | __PINMODE0_30)

#define __PINMODE1_val (__PINMODE1_0 | __PINMODE1_2 | __PINMODE1_4 | __PINMODE1_6 | \
__PINMODE1_8 | __PINMODE1_10 | __PINMODE1_12 | __PINMODE1_14 | __PINMODE1_16 | __PINMODE1_18 | \
__PINMODE1_20)

#define __PINMODE2_val (__PINMODE2_0 | __PINMODE2_2 | \
__PINMODE2_8 | __PINMODE2_16 | __PINMODE2_18 | \
__PINMODE2_20 | __PINMODE2_28 | __PINMODE2_30)

#define __PINMODE3_val (__PINMODE3_0 | __PINMODE3_2 | __PINMODE3_4 | __PINMODE3_6 | \
__PINMODE3_8 | __PINMODE3_10 | __PINMODE3_12 | __PINMODE3_14 | __PINMODE3_16 | __PINMODE3_18 | \
__PINMODE3_20 | __PINMODE3_22 | __PINMODE3_24 | __PINMODE3_26 | __PINMODE3_28 | __PINMODE3_30)

#define __PINMODE4_val (__PINMODE4_0 | __PINMODE4_2 | __PINMODE4_4 | __PINMODE4_6 | \
__PINMODE4_8 | __PINMODE4_10 | __PINMODE4_12 | __PINMODE4_14 | __PINMODE4_16 | __PINMODE4_18 | \
__PINMODE4_20 | __PINMODE4_22 | __PINMODE4_24 | __PINMODE4_26)

#define __PINMODE7_val (__PINMODE7_18 | __PINMODE7_20)
#define __PINMODE9_val (__PINMODE9_24 | __PINMODE9_26)



#if (PIO0_ADC_BITS & BIT(2))    // AD0.7
	#define __PINSEL0_4  (2 << 4) // function 2 is AD0.7
#else
    #define __PINSEL0_4 0
#endif
#if (PIO0_ADC_BITS & BIT(3))    // AD0.6
	#define __PINSEL0_6  (2 << 6) // function 2 is AD0.6
#else
    #define __PINSEL0_6 0
#endif
#if (PIO0_ADC_BITS & BIT(23))   // AD0.0
	#define __PINSEL1_14  (1 << 14) // function 1 is AD0.0
#else
    #define __PINSEL1_14 0
#endif
#if (PIO0_ADC_BITS & BIT(24))   // AD0.1
	#define __PINSEL1_16  (1 << 16) // function 1 is AD0.1
#else
    #define __PINSEL1_16 0
#endif
#if (PIO0_ADC_BITS & BIT(25))   // AD0.2
	#define __PINSEL1_18  (1 << 18) // function 1 is AD0.2
#else
    #define __PINSEL1_18 0
#endif
#if (PIO0_ADC_BITS & BIT(26))   // AD0.3
	#define __PINSEL1_20  (1 << 20) // function 1 is AD0.3
#else
    #define __PINSEL1_20 0
#endif

#if (PIO1_ADC_BITS & BIT(30))   // AD0.4
	#define __PINSEL3_28  (3 << 28) // function 3 is AD0.4
#else
    #define __PINSEL3_28 0
#endif
#if (PIO1_ADC_BITS & BIT(31))   // AD0.5
	#define __PINSEL3_30  (3 << 30) // function 3 is AD0.5
#else
    #define __PINSEL3_30 0
#endif



#define __PINSEL0_val (__PINSEL0_4 | __PINSEL0_6)

#define __PINSEL1_val (__PINSEL1_14 | __PINSEL1_16 | __PINSEL1_18 | __PINSEL1_20)

#define __PINSEL2_val 0

#define __PINSEL3_val (__PINSEL3_28 | __PINSEL3_30)

#define __PINSEL4_val 0
#define __PINSEL7_val 0
#define __PINSEL9_val 0



void sys_init(void)
{
#ifdef FOSC
    LPC_SC->SCS = (1 << 5) | __OSCRANGE_Val;        /* Power-up System Osc and set OSCRANGE  */
    while ((LPC_SC->SCS & (1<<6)) == 0);            /* Wait for Oscillator to be ready    */
#endif // FOSC

    LPC_SC->CCLKCFG   = __CCLKCFG_Val;      /* Setup Clock Divider                */
    LPC_SC->CLKSRCSEL = __CLKSRCSEL_Val;    /* Select Clock Source for PLL0       */

    /* Periphral clock must be selected before PLL0 enabling and connecting
     * - according errata.lpc1768-16.March.2010 -
     */
    LPC_SC->PCLKSEL0  = __PCLKSEL0_Val;     /* Peripheral Clock Selection         */
    LPC_SC->PCLKSEL1  = __PCLKSEL1_Val;

    PCONP     = __PCONP_Val;                /* Power Control for Peripherals      */

#ifdef __PLL_ENABLE
    LPC_SC->PLL0CFG   = __PLL0CFG_Val;      /* configure PLL0                     */
    LPC_SC->PLL0FEED  = 0xAA;
    LPC_SC->PLL0FEED  = 0x55;

    LPC_SC->PLL0CON   = 0x01;               /* PLL0 Enable                        */
    LPC_SC->PLL0FEED  = 0xAA;
    LPC_SC->PLL0FEED  = 0x55;
    while (!(LPC_SC->PLL0STAT & (1<<26)));  /* Wait for PLOCK0                    */

    LPC_SC->PLL0CON   = 0x03;               /* PLL0 Enable & Connect              */
    LPC_SC->PLL0FEED  = 0xAA;
    LPC_SC->PLL0FEED  = 0x55;
    while (!(LPC_SC->PLL0STAT & ((1<<25) | (1<<24))));/* Wait for PLLC0_STAT & PLLE0_STAT */
#endif // __PLL_ENABLE

    LPC_SC->FLASHCFG  = __FLASHCFG_Val;

//  Set Vector table offset value
#if (__RAM_MODE__==1)
    SCB->VTOR  = 0x10000000 & 0x3FFFFF80;
#else   // FLASH mode
    SCB->VTOR  = 0x00000000 & 0x3FFFFF80;
#endif

    // setup the parallel port pin
	LPC_GPIO0->FIOPIN = PIO0_ONE_BITS;                 // set the ONEs output
	LPC_GPIO1->FIOPIN = PIO1_ONE_BITS;                 // set the ONEs output
	LPC_GPIO2->FIOPIN = PIO2_ONE_BITS;                 // set the ONEs output
	LPC_GPIO3->FIOPIN = PIO3_ONE_BITS;                 // set the ONEs output
	LPC_GPIO4->FIOPIN = PIO4_ONE_BITS;                 // set the ONEs output

	LPC_GPIO0->FIODIR = PIO0_OUTPUT_BITS;              // set the output bit direction
	LPC_GPIO1->FIODIR = PIO1_OUTPUT_BITS;              // set the output bit direction
	LPC_GPIO2->FIODIR = PIO2_OUTPUT_BITS;              // set the output bit direction
	LPC_GPIO3->FIODIR = PIO3_OUTPUT_BITS;              // set the output bit direction
	LPC_GPIO4->FIODIR = PIO4_OUTPUT_BITS;              // set the output bit direction


	// set all inputs with pull-up
	// set all outpus and ADC pins without pull-up
	PINMODE0 = __PINMODE0_val;
	PINMODE1 = __PINMODE1_val;
	PINMODE2 = __PINMODE2_val;
	PINMODE3 = __PINMODE3_val;
	PINMODE4 = __PINMODE4_val;
	PINMODE7 = __PINMODE7_val;
	PINMODE9 = __PINMODE9_val;

	PINMODE_OD0 = PIO0_OPENDRAIN_BITS;
	PINMODE_OD1 = PIO1_OPENDRAIN_BITS;
	PINMODE_OD2 = PIO2_OPENDRAIN_BITS;
	PINMODE_OD3 = PIO3_OPENDRAIN_BITS;
	PINMODE_OD4 = PIO4_OPENDRAIN_BITS;

	PINSEL0 = __PINSEL0_val;
	PINSEL1 = __PINSEL1_val;
	PINSEL2 = __PINSEL2_val;
	PINSEL3 = __PINSEL3_val;
	PINSEL4 = __PINSEL4_val;
	PINSEL7 = __PINSEL7_val;
	PINSEL9 = __PINSEL9_val;
}

