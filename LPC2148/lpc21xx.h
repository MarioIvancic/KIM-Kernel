/******************************************************************************
 *
 * $RCSfile: $
 * $Revision: $
 *
 * Header file for Philips LPC21xx ARM Processors
 * Copyright 2004 R O SoftWare
 *
 * No guarantees, warrantees, or promises, implied or otherwise.
 * May be used for hobby or commercial purposes provided copyright
 * notice remains intact.
 *
 *****************************************************************************/

/*
    From LPC2148 User Manual:
Both the AHB and APB peripheral areas are 2 megabyte spaces which are divided up into
128 peripherals. Each peripheral space is 16 kilobytes in size. This allows simplifying the
address decoding for each peripheral. All peripheral register addresses are word aligned
(to 32-bit boundaries) regardless of their size. This eliminates the need for byte lane
mapping hardware that would be required to allow byte (8-bit) or half-word (16-bit)
accesses to occur at smaller boundaries. An implication of this is that word and half-word
registers must be accessed all at once. For example, it is not possible to read or write the
upper byte of a word register separately.

So, all REG_8 and REG16 are changed to REG32
*/
#ifndef INC_LPC21xx_H
#define INC_LPC21xx_H


#define REG_8           volatile unsigned char
#define REG16           volatile unsigned short
#define REG32           volatile unsigned long
#define FIOREG32        union{REG32 u32; REG16 u16[2]; REG_8 u8[4];}

///////////////////////////////////////////////////////////////////////////////

// Watchdog Registers
typedef struct
{
    REG32 mod;                            // Watchdog Mode Register
    REG32 tc;                             // Watchdog Time Constant Register
    REG32 feed;                           // Watchdog Feed Register
    REG32 tv;                             // Watchdog Time Value Register
} wdRegs_t;

// Watchdog
#define WD              ((wdRegs_t *)0xE0000000)

// Watchdog Registers
#define WDMOD           WD->mod         /* Watchdog Mode Register */
#define WDTC            WD->tc          /* Watchdog Time Constant Register */
#define WDFEED          WD->feed        /* Watchdog Feed Register */
#define WDTV            WD->tv          /* Watchdog Time Value Register */

///////////////////////////////////////////////////////////////////////////////

// Timer & PWM Registers
typedef struct
{
    REG32 ir;                             // Interrupt Register
    REG32 tcr;                            // Timer Control Register
    REG32 tc;                             // Timer Counter
    REG32 pr;                             // Prescale Register
    REG32 pc;                             // Prescale Counter Register
    REG32 mcr;                            // Match Control Register
    REG32 mr0;                            // Match Register 0
    REG32 mr1;                            // Match Register 1
    REG32 mr2;                            // Match Register 2
    REG32 mr3;                            // Match Register 3
    REG32 ccr;                            // Capture Control Register
    REG32 cr0;                            // Capture Register 0
    REG32 cr1;                            // Capture Register 1
    REG32 cr2;                            // Capture Register 2
    REG32 cr3;                            // Capture Register 3
    REG32 emr;                            // External Match Register
    REG32 mr4;                            // Match Register 4
    REG32 mr5;                            // Match Register 5
    REG32 mr6;                            // Match Register 6
    REG32 pcr;                            // Control Register
    REG32 ler;                            // Latch Enable Register
} pwmTmrRegs_t;

// Timer Interrupt Register Bit Definitions
#define TIR_MR0I    (1 << 0)            // Interrupt flag for match channel 0
#define TIR_MR1I    (1 << 1)            // Interrupt flag for match channel 1
#define TIR_MR2I    (1 << 2)            // Interrupt flag for match channel 2
#define TIR_MR3I    (1 << 3)            // Interrupt flag for match channel 3
#define TIR_CR0I    (1 << 4)            // Interrupt flag for capture channel 0 event
#define TIR_CR1I    (1 << 5)            // Interrupt flag for capture channel 1 event
#define TIR_CR2I    (1 << 6)            // Interrupt flag for capture channel 2 event
#define TIR_CR3I    (1 << 7)            // Interrupt flag for capture channel 3 event

// PWM Interrupt Register Bit Definitions
#define PWMIR_MR0I  (1 << 0)            // Interrupt flag for match channel 0
#define PWMIR_MR1I  (1 << 1)            // Interrupt flag for match channel 1
#define PWMIR_MR2I  (1 << 2)            // Interrupt flag for match channel 2
#define PWMIR_MR3I  (1 << 3)            // Interrupt flag for match channel 3
#define PWMIR_MR4I  (1 << 8)            // Interrupt flag for match channel 4
#define PWMIR_MR5I  (1 << 9)            // Interrupt flag for match channel 5
#define PWMIR_MR6I  (1 << 10)           // Interrupt flag for match channel 6
#define PWMIR_MASK  (0x070F)

// Timer Control Register Bit Definitions
#define TCR_ENABLE  (1 << 0)
#define TCR_RESET   (1 << 1)

// PWM Control Register Bit Definitions
#define PWMCR_ENABLE (1 << 0)
#define PWMCR_RESET (1 << 1)

// Timer Match Control Register Bit Definitions
#define TMCR_MR0_I  (1 << 0)            // Enable Interrupt when MR0 matches TC
#define TMCR_MR0_R  (1 << 1)            // Enable Reset of TC upon MR0 match
#define TMCR_MR0_S  (1 << 2)            // Enable Stop of TC upon MR0 match
#define TMCR_MR1_I  (1 << 3)            // Enable Interrupt when MR1 matches TC
#define TMCR_MR1_R  (1 << 4)            // Enable Reset of TC upon MR1 match
#define TMCR_MR1_S  (1 << 5)            // Enable Stop of TC upon MR1 match
#define TMCR_MR2_I  (1 << 6)            // Enable Interrupt when MR2 matches TC
#define TMCR_MR2_R  (1 << 7)            // Enable Reset of TC upon MR2 match
#define TMCR_MR2_S  (1 << 8)            // Enable Stop of TC upon MR2 match
#define TMCR_MR3_I  (1 << 9)            // Enable Interrupt when MR3 matches TC
#define TMCR_MR3_R  (1 << 10)           // Enable Reset of TC upon MR3 match
#define TMCR_MR3_S  (1 << 11)           // Enable Stop of TC upon MR3 match

// Timer Capture Control Register Bit Definitions
#define TCCR_CR0_R (1 << 0)            // Enable Rising edge on CAPn.0 will load TC to CR0
#define TCCR_CR0_F (1 << 1)            // Enable Falling edge on CAPn.0 will load TC to CR0
#define TCCR_CR0_I (1 << 2)            // Enable Interrupt on load of CR0
#define TCCR_CR1_R (1 << 3)            // Enable Rising edge on CAPn.1 will load TC to CR1
#define TCCR_CR1_F (1 << 4)            // Enable Falling edge on CAPn.1 will load TC to CR1
#define TCCR_CR1_I (1 << 5)            // Enable Interrupt on load of CR1
#define TCCR_CR2_R (1 << 6)            // Enable Rising edge on CAPn.2 will load TC to CR2
#define TCCR_CR2_F (1 << 7)            // Enable Falling edge on CAPn.2 will load TC to CR2
#define TCCR_CR2_I (1 << 8)            // Enable Interrupt on load of CR2
#define TCCR_CR3_R (1 << 9)            // Enable Rising edge on CAPn.3 will load TC to CR3
#define TCCR_CR3_F (1 << 10)           // Enable Falling edge on CAPn.3 will load TC to CR3
#define TCCR_CR3_I (1 << 11)           // Enable Interrupt on load of CR3

// Timer 0
#define TMR0            ((pwmTmrRegs_t *)0xE0004000)

// Timer 0 Registers
#define T0IR            TMR0->ir        /* Interrupt Register */
#define T0TCR           TMR0->tcr       /* Timer Control Register */
#define T0TC            TMR0->tc        /* Timer Counter */
#define T0PR            TMR0->pr        /* Prescale Register */
#define T0PC            TMR0->pc        /* Prescale Counter Register */
#define T0MCR           TMR0->mcr       /* Match Control Register */
#define T0MR0           TMR0->mr0       /* Match Register 0 */
#define T0MR1           TMR0->mr1       /* Match Register 1 */
#define T0MR2           TMR0->mr2       /* Match Register 2 */
#define T0MR3           TMR0->mr3       /* Match Register 3 */
#define T0CCR           TMR0->ccr       /* Capture Control Register */
#define T0CR0           TMR0->cr0       /* Capture Register 0 */
#define T0CR1           TMR0->cr1       /* Capture Register 1 */
#define T0CR2           TMR0->cr2       /* Capture Register 2 */
#define T0CR3           TMR0->cr3       /* Capture Register 3 */
#define T0EMR           TMR0->emr       /* External Match Register */


// Timer 1
#define TMR1            ((pwmTmrRegs_t *)0xE0008000)

// Timer 1 Registers
#define T1IR            TMR1->ir        /* Interrupt Register */
#define T1TCR           TMR1->tcr       /* Timer Control Register */
#define T1TC            TMR1->tc        /* Timer Counter */
#define T1PR            TMR1->pr        /* Prescale Register */
#define T1PC            TMR1->pc        /* Prescale Counter Register */
#define T1MCR           TMR1->mcr       /* Match Control Register */
#define T1MR0           TMR1->mr0       /* Match Register 0 */
#define T1MR1           TMR1->mr1       /* Match Register 1 */
#define T1MR2           TMR1->mr2       /* Match Register 2 */
#define T1MR3           TMR1->mr3       /* Match Register 3 */
#define T1CCR           TMR1->ccr       /* Capture Control Register */
#define T1CR0           TMR1->cr0       /* Capture Register 0 */
#define T1CR1           TMR1->cr1       /* Capture Register 1 */
#define T1CR2           TMR1->cr2       /* Capture Register 2 */
#define T1CR3           TMR1->cr3       /* Capture Register 3 */
#define T1EMR           TMR1->emr       /* External Match Register */

// Pulse Width Modulator (PWM)
#define PWM             ((pwmTmrRegs_t *)0xE0014000)

// PWM Registers
#define PWMIR           PWM->ir         /* Interrupt Register */
#define PWMTCR          PWM->tcr        /* Timer Control Register */
#define PWMTC           PWM->tc         /* Timer Counter */
#define PWMPR           PWM->pr         /* Prescale Register */
#define PWMPC           PWM->pc         /* Prescale Counter Register */
#define PWMMCR          PWM->mcr        /* Match Control Register */
#define PWMMR0          PWM->mr0        /* Match Register 0 */
#define PWMMR1          PWM->mr1        /* Match Register 1 */
#define PWMMR2          PWM->mr2        /* Match Register 2 */
#define PWMMR3          PWM->mr3        /* Match Register 3 */
#define PWMMR4          PWM->mr4        /* Match Register 4 */
#define PWMMR5          PWM->mr5        /* Match Register 5 */
#define PWMMR6          PWM->mr6        /* Match Register 6 */
#define PWMPCR          PWM->pcr        /* Control Register */
#define PWMLER          PWM->ler        /* Latch Enable Register */

///////////////////////////////////////////////////////////////////////////////

// Universal Asynchronous Receiver Transmitter Registers
typedef struct
{
    union
    {
        REG32 rbr;                          // Receive Buffer Register
        REG32 thr;                          // Transmit Holding Register
        REG32 dll;                          // Divisor Latch Register (LSB)
    };

    union
    {
        REG32 ier;                          // Interrupt Enable Register
        REG32 dlm;                          // Divisor Latch Register (MSB)
    };

    union
    {
        REG32 iir;                          // Interrupt ID Register
        REG32 fcr;                          // FIFO Control Register
    };

    REG32 lcr;                            // Line Control Registe
    REG32 mcr;                            // MODEM Control Register
    REG32 lsr;                            // Line Status Register
    REG32 msr;                            // MODEM Status Register
    REG32 scr;                            // Scratch Pad Register
} uartRegs_t;


// Universal Asynchronous Receiver Transmitter 0 (UART0)
#define UART0           ((uartRegs_t *)0xE000C000)
#define U0_PINSEL       (0x00000005)    /* PINSEL0 Value for UART0 */
#define U0_PINMASK      (0x0000000F)    /* PINSEL0 Mask for UART0 */

// UART0 Registers
#define U0RBR           UART0->rbr      /* Receive Buffer Register */
#define U0THR           UART0->thr      /* Transmit Holding Register */
#define U0IER           UART0->ier      /* Interrupt Enable Register */
#define U0IIR           UART0->iir      /* Interrupt ID Register */
#define U0FCR           UART0->fcr      /* FIFO Control Register */
#define U0LCR           UART0->lcr      /* Line Control Register */
#define U0LSR           UART0->lsr      /* Line Status Register */
#define U0SCR           UART0->scr      /* Scratch Pad Register */
#define U0DLL           UART0->dll      /* Divisor Latch Register (LSB) */
#define U0DLM           UART0->dlm      /* Divisor Latch Register (MSB) */

// Universal Asynchronous Receiver Transmitter 1 (UART1)
#define UART1           ((uartRegs_t *)0xE0010000)
#define U1_PINSEL       (0x00050000)    /* PINSEL0 Value for UART1 */
#define U1_PINMASK      (0x000F0000)    /* PINSEL0 Mask for UART1 */

// UART1 Registers
#define U1RBR           UART1->rbr      /* Receive Buffer Register */
#define U1THR           UART1->thr      /* Transmit Holding Register */
#define U1IER           UART1->ier      /* Interrupt Enable Register */
#define U1IIR           UART1->iir      /* Interrupt ID Register */
#define U1FCR           UART1->fcr      /* FIFO Control Register */
#define U1LCR           UART1->lcr      /* Line Control Register */
#define U1MCR           UART1->mcr      /* MODEM Control Register */
#define U1LSR           UART1->lsr      /* Line Status Register */
#define U1MSR           UART1->msr      /* MODEM Status Register */
#define U1SCR           UART1->scr      /* Scratch Pad Register */
#define U1DLL           UART1->dll      /* Divisor Latch Register (LSB) */
#define U1DLM           UART1->dlm      /* Divisor Latch Register (MSB) */

///////////////////////////////////////////////////////////////////////////////


// I2C Interface Registers
typedef struct
{
    REG32 conset;                         // Control Set Register
    REG32 stat;                           // Status Register
    REG32 dat;                            // Data Register
    REG32 adr;                            // Slave Address Register
    REG32 sclh;                           // SCL Duty Cycle Register (high half word)
    REG32 scll;                           // SCL Duty Cycle Register (low half word)
    REG32 conclr;                         // Control Clear Register
} i2cRegs_t;

// I2C Interface
#define I2C             ((i2cRegs_t *)0xE001C000)

// I2C Registers
#define I2CONSET        I2C->conset     /* Control Set Register */
#define I2STAT          I2C->stat       /* Status Register */
#define I2DAT           I2C->dat        /* Data Register */
#define I2ADR           I2C->adr        /* Slave Address Register */
#define I2SCLH          I2C->sclh       /* SCL Duty Cycle Register (high half word) */
#define I2SCLL          I2C->scll       /* SCL Duty Cycle Register (low half word) */
#define I2CONCLR        I2C->conclr     /* Control Clear Register */

// I2C0 Interface
#define I2C0             ((i2cRegs_t *)0xE001C000)

// I2C0 Registers
#define I2C0CONSET        I2C0->conset     /* Control Set Register */
#define I2C0STAT          I2C0->stat       /* Status Register */
#define I2C0DAT           I2C0->dat        /* Data Register */
#define I2C0ADR           I2C0->adr        /* Slave Address Register */
#define I2C0SCLH          I2C0->sclh       /* SCL Duty Cycle Register (high half word) */
#define I2C0SCLL          I2C0->scll       /* SCL Duty Cycle Register (low half word) */
#define I2C0CONCLR        I2C0->conclr     /* Control Clear Register */

// I2C1 Interface
#define I2C1             ((i2cRegs_t *)0xE005C000)

// I2C1 Registers
#define I2C1CONSET        I2C1->conset     /* Control Set Register */
#define I2C1STAT          I2C1->stat       /* Status Register */
#define I2C1DAT           I2C1->dat        /* Data Register */
#define I2C1ADR           I2C1->adr        /* Slave Address Register */
#define I2C1SCLH          I2C1->sclh       /* SCL Duty Cycle Register (high half word) */
#define I2C1SCLL          I2C1->scll       /* SCL Duty Cycle Register (low half word) */
#define I2C1CONCLR        I2C1->conclr     /* Control Clear Register */


///////////////////////////////////////////////////////////////////////////////


// Serial Peripheral Interface Registers (SPI)
typedef struct
{
    REG32 cr;                             // Control Register
    REG32 sr;                             // Status Register
    REG32 dr;                             // Data Register
    REG32 ccr;                            // Clock Counter Register
    REG32 tcr;                            // Test Control Register
    REG32 tsr;                            // Test Status Register
    REG32 tor;                            // Test Observe Register
    REG32 flag;                           // Interrupt Flag Register
} spiRegs_t;

// Serial Peripheral Interface 0 (SPI0)
#define SPI0            ((spiRegs_t *)0xE0020000)

// SPI0 Registers
#define S0SPCR          SPI0->cr        /* Control Register */
#define S0SPSR          SPI0->sr        /* Status Register */
#define S0SPDR          SPI0->dr        /* Data Register */
#define S0SPCCR         SPI0->ccr       /* Clock Counter Register */
#define S0SPINT         SPI0->flag      /* Interrupt Flag Register */

// Serial Peripheral Interface 1 (SPI1)
#define SPI1            ((spiRegs_t *)0xE0030000)

// SPI1 Registers
#define S1SPCR          SPI1->cr        /* Control Register */
#define S1SPSR          SPI1->sr        /* Status Register */
#define S1SPDR          SPI1->dr        /* Data Register */
#define S1SPCCR         SPI1->ccr       /* Clock Counter Register */
#define S1SPINT         SPI1->flag      /* Interrupt Flag Register */

///////////////////////////////////////////////////////////////////////////////

// RTC registers
typedef struct
{
    REG32 ilr;                            // Interrupt Location Register
    REG32 ctc;                            // Clock Tick Counter
    REG32 ccr;                            // Clock Control Register
    REG32 ciir;                           // Counter Increment Interrupt Register
    REG32 amr;                            // Alarm Mask Register
    REG32 ctime0;                         // Consolidated Time Register 0
    REG32 ctime1;                         // Consolidated Time Register 1
    REG32 ctime2;                         // Consolidated Time Register 2
    REG32 sec;                            // Seconds Register
    REG32 min;                            // Minutes Register
    REG32 hour;                           // Hours Register
    REG32 dom;                            // Day Of Month Register
    REG32 dow;                            // Day Of Week Register
    REG32 doy;                            // Day Of Year Register
    REG32 month;                          // Months Register
    REG32 year;                           // Years Register
    REG32 alsec;                          // Alarm Seconds Register
    REG32 almin;                          // Alarm Minutes Register
    REG32 alhour;                         // Alarm Hours Register
    REG32 aldom;                          // Alarm Day Of Month Register
    REG32 aldow;                          // Alarm Day Of Week Register
    REG32 aldoy;                          // Alarm Day Of Year Register
    REG32 almon;                          // Alarm Months Register
    REG32 alyear;                         // Alarm Years Register
    REG32 preint;                         // Prescale Value Register (integer)
    REG32 prefrac;                        // Prescale Value Register (fraction)
} rtcRegs_t;

// Real Time Clock
#define RTC             ((rtcRegs_t *)0xE0024000)

// RTC Registers
#define RTCILR          RTC->ilr        /* Interrupt Location Register */
#define RTCCTC          RTC->ctc        /* Clock Tick Counter */
#define RTCCCR          RTC->ccr        /* Clock Control Register */
#define RTCCIIR         RTC->ciir       /* Counter Increment Interrupt Register */
#define RTCAMR          RTC->amr        /* Alarm Mask Register */
#define RTCCTIME0       RTC->ctime0     /* Consolidated Time Register 0 */
#define RTCCTIME1       RTC->ctime1     /* Consolidated Time Register 1 */
#define RTCCTIME2       RTC->ctime2     /* Consolidated Time Register 2 */
#define RTCSEC          RTC->sec        /* Seconds Register */
#define RTCMIN          RTC->min        /* Minutes Register */
#define RTCHOUR         RTC->hour       /* Hours Register */
#define RTCDOM          RTC->dom        /* Day Of Month Register */
#define RTCDOW          RTC->dow        /* Day Of Week Register */
#define RTCDOY          RTC->doy        /* Day Of Year Register */
#define RTCMONTH        RTC->month      /* Months Register */
#define RTCYEAR         RTC->year       /* Years Register */
#define RTCALSEC        RTC->alsec      /* Alarm Seconds Register */
#define RTCALMIN        RTC->almin      /* Alarm Minutes Register */
#define RTCALHOUR       RTC->alhour     /* Alarm Hours Register */
#define RTCALDOM        RTC->aldom      /* Alarm Day Of Month Register */
#define RTCALDOW        RTC->aldow      /* Alarm Day Of Week Register */
#define RTCALDOY        RTC->aldoy      /* Alarm Day Of Year Register */
#define RTCALMON        RTC->almon      /* Alarm Months Register */
#define RTCALYEAR       RTC->alyear     /* Alarm Years Register */
#define RTCPREINT       RTC->preint     /* Prescale Value Register (integer) */
#define RTCPREFRAC      RTC->prefrac    /* Prescale Value Register (fraction) */

///////////////////////////////////////////////////////////////////////////////

// General Purpose Input/Output Registers (GPIO)
typedef struct
{
    REG32 in0;                            // P0 Pin Value Register
    REG32 set0;                           // P0 Pin Output Set Register
    REG32 dir0;                           // P0 Pin Direction Register
    REG32 clr0;                           // P0 Pin Output Clear Register

    REG32 in1;                            // P1 Pin Value Register
    REG32 set1;                           // P1 Pin Output Set Register
    REG32 dir1;                           // P1 Pin Direction Register
    REG32 clr1;                           // P1 Pin Output Clear Register

    REG32 in2;                            // P2 Pin Value Register
    REG32 set2;                           // P2 Pin Output Set Register
    REG32 dir2;                           // P2 Pin Direction Register
    REG32 clr2;                           // P2 Pin Output Clear Register

    REG32 in3;                            // P3 Pin Value Register
    REG32 set3;                           // P3 Pin Output Set Register
    REG32 dir3;                           // P3 Pin Direction Register
    REG32 clr3;                           // P3 Pin Output Clear Register
} gpioRegs_t;

// Fast General Purpose Input/Output Registers (FIO)
// System Control and Status flags register
typedef struct
{
    unsigned int gpio0m:1; // bit 0
    unsigned int gpio1m:1; // bit 1
    unsigned int _unused:30;
} scsReg_t;

typedef struct
{
    FIOREG32 dir0;                         // P0 Pin Direction Register
    FIOREG32 _pad0[3];
    FIOREG32 mask0;                        // P0 Pin Mask Register
    FIOREG32 in0;                          // P0 Pin Value Register
    FIOREG32 set0;                         // P0 Pin Output Set Register
    FIOREG32 clr0;                         // P0 Pin Output Clear Register

    FIOREG32 dir1;                         // P1 Pin Direction Register
    FIOREG32 _pad1[3];
    FIOREG32 mask1;                        // P1 Pin Mask Register
    FIOREG32 in1;                          // P1 Pin Value Register
    FIOREG32 set1;                         // P1 Pin Output Set Register
    FIOREG32 clr1;                         // P1 Pin Output Clear Register

    FIOREG32 dir2;                         // P2 Pin Direction Register
    FIOREG32 _pad2[3];
    FIOREG32 mask2;                        // P2 Pin Mask Register
    FIOREG32 in2;                          // P2 Pin Value Register
    FIOREG32 set2;                         // P2 Pin Output Set Register
    FIOREG32 clr2;                         // P2 Pin Output Clear Register

    FIOREG32 dir3;                         // P3 Pin Direction Register
    FIOREG32 _pad3[3];
    FIOREG32 mask3;                        // P3 Pin Mask Register
    FIOREG32 in3;                          // P3 Pin Value Register
    FIOREG32 set3;                         // P3 Pin Output Set Register
    FIOREG32 clr3;                         // P3 Pin Output Clear Register
} fioRegs_t;


// General Purpose Input/Output
#define GPIO            ((gpioRegs_t *)0xE0028000)

// GPIO Registers
#define IO0PIN          GPIO->in0       /* P0 Pin Value Register */
#define IO0SET          GPIO->set0      /* P0 Pin Output Set Register */
#define IO0DIR          GPIO->dir0      /* P0 Pin Direction Register */
#define IO0CLR          GPIO->clr0      /* P0 Pin Output Clear Register */

#define IO1PIN          GPIO->in1       /* P1 Pin Value Register */
#define IO1SET          GPIO->set1      /* P1 Pin Output Set Register */
#define IO1DIR          GPIO->dir1      /* P1 Pin Direction Register */
#define IO1CLR          GPIO->clr1      /* P1 Pin Output Clear Register */

// System Control and Status flags register
#define SCS             ((scsReg_t *)0xE01FC1A0)
#define GPIO0M          SCS->gpio0m     /* P0 mode bit */
#define GPIO1M          SCS->gpio1m     /* P1 mode bit */
#define FIO_ENABLE      0x1UL

// Fast IO
#define FIO              ((fioRegs_t *)0x3FFFC000)

// FIO 32-bit registers
#define FIO0PIN         FIO->in0.u32    /* P0 Pin Value Register */
#define FIO0SET         FIO->set0.u32   /* P0 Pin Output Set Register */
#define FIO0DIR         FIO->dir0.u32   /* P0 Pin Direction Register */
#define FIO0CLR         FIO->clr0.u32   /* P0 Pin Output Clear Register */
#define FIO0MASK        FIO->mask0.u32  /* P0 Pin Mask Register */

#define FIO1PIN         FIO->in1.u32    /* P1 Pin Value Register */
#define FIO1SET         FIO->set1.u32   /* P1 Pin Output Set Register */
#define FIO1DIR         FIO->dir1.u32   /* P1 Pin Direction Register */
#define FIO1CLR         FIO->clr1.u32   /* P1 Pin Output Clear Register */
#define FIO1MASK        FIO->mask1.u32  /* P1 Pin Mask Register */

// FIO 16-bit registers
#define FIO0PINL        FIO->in0.u16[0]  /* P0 Pin Value Register */
#define FIO0SETL        FIO->set0.u16[0] /* P0 Pin Output Set Register */
#define FIO0DIRL        FIO->dir0.u16[0] /* P0 Pin Direction Register */
#define FIO0CLRL        FIO->clr0.u16[0] /* P0 Pin Output Clear Register */
#define FIO0MASKL       FIO->mask0.u16[0]/* P0 Pin Mask Register */

#define FIO1PINL        FIO->in1.u16[0]  /* P1 Pin Value Register */
#define FIO1SETL        FIO->set1.u16[0] /* P1 Pin Output Set Register */
#define FIO1DIRL        FIO->dir1.u16[0] /* P1 Pin Direction Register */
#define FIO1CLRL        FIO->clr1.u16[0] /* P1 Pin Output Clear Register */
#define FIO1MASKL       FIO->mask1.u16[0]/* P1 Pin Mask Register */

#define FIO0PINU        FIO->in0.u16[1]  /* P0 Pin Value Register */
#define FIO0SETU        FIO->set0.u16[1] /* P0 Pin Output Set Register */
#define FIO0DIRU        FIO->dir0.u16[1] /* P0 Pin Direction Register */
#define FIO0CLRU        FIO->clr0.u16[1] /* P0 Pin Output Clear Register */
#define FIO0MASKU       FIO->mask0.u16[1]/* P0 Pin Mask Register */

#define FIO1PINU        FIO->in1.u16[1]  /* P1 Pin Value Register */
#define FIO1SETU        FIO->set1.u16[1] /* P1 Pin Output Set Register */
#define FIO1DIRU        FIO->dir1.u16[1] /* P1 Pin Direction Register */
#define FIO1CLRU        FIO->clr1.u16[1] /* P1 Pin Output Clear Register */
#define FIO1MASKU       FIO->mask1.u16[1]/* P1 Pin Mask Register */

// FIO 8-bit registers
#define FIO0PIN0        FIO->in0.u8[0]  /* P0 Pin Value Register */
#define FIO0SET0        FIO->set0.u8[0] /* P0 Pin Output Set Register */
#define FIO0DIR0        FIO->dir0.u8[0] /* P0 Pin Direction Register */
#define FIO0CLR0        FIO->clr0.u8[0] /* P0 Pin Output Clear Register */
#define FIO0MASK0       FIO->mask0.u8[0]/* P0 Pin Mask Register */

#define FIO1PIN0        FIO->in1.u8[0]  /* P1 Pin Value Register */
#define FIO1SET0        FIO->set1.u8[0] /* P1 Pin Output Set Register */
#define FIO1DIR0        FIO->dir1.u8[0] /* P1 Pin Direction Register */
#define FIO1CLR0        FIO->clr1.u8[0] /* P1 Pin Output Clear Register */
#define FIO1MASK0       FIO->mask1.u8[0]/* P1 Pin Mask Register */

#define FIO0PIN1        FIO->in0.u8[1]  /* P0 Pin Value Register */
#define FIO0SET1        FIO->set0.u8[1] /* P0 Pin Output Set Register */
#define FIO0DIR1        FIO->dir0.u8[1] /* P0 Pin Direction Register */
#define FIO0CLR1        FIO->clr0.u8[1] /* P0 Pin Output Clear Register */
#define FIO0MASK1       FIO->mask0.u8[1]/* P0 Pin Mask Register */

#define FIO1PIN1        FIO->in1.u8[1]  /* P1 Pin Value Register */
#define FIO1SET1        FIO->set1.u8[1] /* P1 Pin Output Set Register */
#define FIO1DIR1        FIO->dir1.u8[1] /* P1 Pin Direction Register */
#define FIO1CLR1        FIO->clr1.u8[1] /* P1 Pin Output Clear Register */
#define FIO1MASK1       FIO->mask1.u8[1]/* P1 Pin Mask Register */

#define FIO0PIN2        FIO->in0.u8[2]  /* P0 Pin Value Register */
#define FIO0SET2        FIO->set0.u8[2] /* P0 Pin Output Set Register */
#define FIO0DIR2        FIO->dir0.u8[2] /* P0 Pin Direction Register */
#define FIO0CLR2        FIO->clr0.u8[2] /* P0 Pin Output Clear Register */
#define FIO0MASK2       FIO->mask0.u8[2]/* P0 Pin Mask Register */

#define FIO1PIN2        FIO->in1.u8[2]  /* P1 Pin Value Register */
#define FIO1SET2        FIO->set1.u8[2] /* P1 Pin Output Set Register */
#define FIO1DIR2        FIO->dir1.u8[2] /* P1 Pin Direction Register */
#define FIO1CLR2        FIO->clr1.u8[2] /* P1 Pin Output Clear Register */
#define FIO1MASK2       FIO->mask1.u8[2]/* P1 Pin Mask Register */

#define FIO0PIN3        FIO->in0.u8[3]  /* P0 Pin Value Register */
#define FIO0SET3        FIO->set0.u8[3] /* P0 Pin Output Set Register */
#define FIO0DIR3        FIO->dir0.u8[3] /* P0 Pin Direction Register */
#define FIO0CLR3        FIO->clr0.u8[3] /* P0 Pin Output Clear Register */
#define FIO0MASK3       FIO->mask0.u8[3]/* P0 Pin Mask Register */

#define FIO1PIN3        FIO->in1.u8[3]  /* P1 Pin Value Register */
#define FIO1SET3        FIO->set1.u8[3] /* P1 Pin Output Set Register */
#define FIO1DIR3        FIO->dir1.u8[3] /* P1 Pin Direction Register */
#define FIO1CLR3        FIO->clr1.u8[3] /* P1 Pin Output Clear Register */
#define FIO1MASK3       FIO->mask1.u8[3]/* P1 Pin Mask Register */


// GPIO / FIO independent definitions
#ifdef FAST_GPIO
    #ifndef FIO0_MASK_BITS
        #define FIO0_MASK_BITS 0
    #endif
    #define P0PIN FIO0PIN
    #define P0SET FIO0SET
    #define P0CLR FIO0CLR
    #define P0DIR FIO0DIR
    #define P0MASK FIO0MASK
    #define P1PIN FIO1PIN
    #define P1SET FIO1SET
    #define P1CLR FIO1CLR
    #define P1DIR FIO1DIR
    #define P1MASK FIO1MASK
    #undef IO0PIN
    #undef IO0SET
    #undef IO0CLR
    #undef IO0DIR
    #undef I10PIN
    #undef I10SET
    #undef I10CLR
    #undef I10DIR
#else
    #define P0PIN IO0PIN
    #define P0SET IO0SET
    #define P0CLR IO0CLR
    #define P0DIR IO0DIR
    #define P1PIN IO1PIN
    #define P1SET IO1SET
    #define P1CLR IO1CLR
    #define P1DIR IO1DIR
    #undef FIO0PIN
    #undef FIO0SET
    #undef FIO0CLR
    #undef FIO0DIR
    #undef FI10PIN
    #undef FI10SET
    #undef FI10CLR
    #undef FI10DIR
#endif // FAST_GPIO

#define FIO0_ZERO_BITS    PIO0_ZERO_BITS
#define FIO0_ONE_BITS     PIO0_ONE_BITS
#define FIO0_OUTPUT_BITS  PIO0_OUTPUT_BITS

#define FIO1_ZERO_BITS    PIO1_ZERO_BITS
#define FIO1_ONE_BITS     PIO1_ONE_BITS
#define FIO1_OUTPUT_BITS  PIO1_OUTPUT_BITS

// malo default vrijednosti
#ifndef FIO0_MASK_BITS
    #define FIO0_MASK_BITS 0
#endif // FIO0_MASK_BITS

#ifndef FIO1_MASK_BITS
    #define FIO1_MASK_BITS 0
#endif // FIO1_MASK_BITS

///////////////////////////////////////////////////////////////////////////////


// Pin Connect Block Registers
typedef struct
{
    REG32 sel0;                           // Pin Function Select Register 0
    REG32 sel1;                           // Pin Function Select Register 1
    REG32 _pad[3];
    REG32 sel2;                           // Pin Function Select Register 2
} pinRegs_t;

// Pin Connect Block
#define PINSEL          ((pinRegs_t *)0xE002C000)

// Pin Connect Block Registers
#define PINSEL0         PINSEL->sel0    /* Pin Function Select Register 0 */
#define PINSEL1         PINSEL->sel1    /* Pin Function Select Register 1 */
#define PINSEL2         PINSEL->sel2    /* Pin Function Select Register 2 */

///////////////////////////////////////////////////////////////////////////////

// A/D Converter Registers
typedef struct
{
    REG32 cr;                            // Control Register, offset 0x00
    REG32 dr;                            // Data Register, offset 0x04
    REG32 start;                        // ADC global start reg., offset 0x08, ADC0 only
    REG32 adinten;                        // ADC interrupt enable reg, offset 0x0c
    REG32 addr0;                        // ADC result reg. for CH0, offset 0x10
    REG32 addr1;                        // ADC result reg. for CH1, offset 0x14
    REG32 addr2;                        // ADC result reg. for CH2, offset 0x18
    REG32 addr3;                        // ADC result reg. for CH3, offset 0x1c
    REG32 addr4;                        // ADC result reg. for CH4, offset 0x20
    REG32 addr5;                        // ADC result reg. for CH5, offset 0x24
    REG32 addr6;                        // ADC result reg. for CH6, offset 0x28
    REG32 addr7;                        // ADC result reg. for CH7, offset 0x2c
    REG32 stat;                            // status register, offset 0x30
} adcRegs_t;

// OVERRUN BIT in dr & addr0:7
#define ADC_OVERRUN        BIT(30)

// DONE BIT
#define ADC_DONE        BIT(31)

// A/D Converter
#define ADC             ((adcRegs_t *)0xE0034000)

// A/D 0 Converter Registers
#define ADCR            ADC->cr         /* Control Register */
#define ADDR            ADC->dr         /* Data Register */

// ADC0
#define ADC0            ((adcRegs_t *)0xE0034000)

#define AD0CR            ADC0->cr        /* Control Register */
#define AD0GDR            ADC0->dr        /* Data Register */
#define AD0GSR            ADC0->start        /* Global Start reg */
#define AD0INTEN        ADC0->adinten    /* interrupt enable reg */
#define AD0DR0            ADC0->addr0        /* result reg. for ch0 */
#define AD0DR1            ADC0->addr1        /* result reg. for ch1 */
#define AD0DR2            ADC0->addr2        /* result reg. for ch2 */
#define AD0DR3            ADC0->addr3        /* result reg. for ch3 */
#define AD0DR4            ADC0->addr4        /* result reg. for ch4 */
#define AD0DR5            ADC0->addr5        /* result reg. for ch5 */
#define AD0DR6            ADC0->addr6        /* result reg. for ch6 */
#define AD0DR7            ADC0->addr7        /* result reg. for ch7 */
#define AD0STAT            ADC0->stat        /* status register */

// ADC1
#define ADC1            ((adcRegs_t *)0xE0060000)

#define AD1CR            ADC1->cr        /* Control Register */
#define AD1GDR            ADC1->dr        /* Global Data Register */
#define AD1INTEN        ADC1->adinten    /* interrupt enable reg */
#define AD1DR0            ADC1->addr0        /* result reg. for ch0 */
#define AD1DR1            ADC1->addr1        /* result reg. for ch1 */
#define AD1DR2            ADC1->addr2        /* result reg. for ch2 */
#define AD1DR3            ADC1->addr3        /* result reg. for ch3 */
#define AD1DR4            ADC1->addr4        /* result reg. for ch4 */
#define AD1DR5            ADC1->addr5        /* result reg. for ch5 */
#define AD1DR6            ADC1->addr6        /* result reg. for ch6 */
#define AD1DR7            ADC1->addr7        /* result reg. for ch7 */
#define AD1STAT            ADC1->stat        /* status register */

///////////////////////////////////////////////////////////////////////////////

// System Control Block Registers
typedef struct
{
    // Memory Accelerator Module Registers (MAM)
    struct
    {
        REG32 cr;                           // Control Register
        REG32 tim;                          // Timing Control Register
    } mam;

    REG32 _pad1[14];

    // Memory Mapping Control Register
    REG32 memmap;
    REG32 _pad2[15];

    // Phase Locked Loop Registers (PLL)
    struct
    {
        REG32 con;                          // Control Register
        REG32 cfg;                          // Configuration Register
        REG32 stat;                         // Status Register
        REG32 feed;                         // Feed Register
    } pll;

    REG32 _pad3[4];

    // USB Phase Locked Loop Registers (USBPLL)
    struct
    {
        REG32 con;                          // Control Register
        REG32 cfg;                          // Configuration Register
        REG32 stat;                         // Status Register
        REG32 feed;                         // Feed Register
    } usbpll;

    REG32 _pad4[4];

    // Power Control Registers
    struct
    {
        REG32 con;                          // Control Register
        REG32 conp;                         // Peripherals Register
    } p;

    REG32 _pad5[14];

    // VPB Divider Register
    REG32 vpbdiv;
    REG32 _pad6[15];

    // External Interrupt Registers
    struct
    {
        REG32 flag;                         // Flag Register
        REG32 wake;                         // Wakeup Register
        REG32 mode;                         // Mode Register
        REG32 polar;                        // Polarity Register
    } ext;

    REG32 _pad7[12];

    // reset ID register
    REG32 rsid;

    // code securiti protection reg
    REG32 cspr;

    REG32 _pad8[6];
    REG32 scs;
} scbRegs_t;


// Phase Locked Loop1 Registers (PLL1 == PLL48 == USBPLL)
typedef struct
{
    REG32 con;                          // Control Register
    REG32 cfg;                          // Configuration Register
    REG32 stat;                         // Status Register
    REG32 feed;                         // Feed Register
} pllRegs_t;


// System Contol Block
#define SCB             ((scbRegs_t *)0xE01FC000)

// Memory Accelerator Module Registers (MAM)
#define MAMCR           SCB->mam.cr     /* Control Register */
#define MAMTIM          SCB->mam.tim    /* Timing Control Register */

// Memory Mapping Control Register
#define MEMMAP          SCB->memmap

// Phase Locked Loop Registers (PLL)
#define PLLCON          SCB->pll.con    /* Control Register */
#define PLLCFG          SCB->pll.cfg    /* Configuration Register */
#define PLLSTAT         SCB->pll.stat   /* Status Register */
#define PLLFEED         SCB->pll.feed   /* Feed Register */

// PLL1 for USB (PLL48)
#define PLL48CON        SCB->usbpll.con      /* Control Register */
#define PLL48CFG        SCB->usbpll.cfg      /* Configuration Register */
#define PLL48STAT       SCB->usbpll.stat     /* Status Register */
#define PLL48FEED       SCB->usbpll.feed     /* Feed Register */

#define PLL1CON  PLL48CON
#define PLL1CFG  PLL48CFG
#define PLL1STAT PLL48STAT
#define PLL1FEED PLL48FEED

// Power Control Registers
#define PCON            SCB->p.con      /* Control Register */
#define PCONP           SCB->p.conp     /* Peripherals Register */

// VPB Divider Register
#define VPBDIV          SCB->vpbdiv

// External Interrupt Registers
#define EXTINT          SCB->ext.flag   /* Flag Register */
#define EXTWAKE         SCB->ext.wake   /* Wakeup Register */
#define EXTMODE         SCB->ext.mode   /* Mode Register */
#define EXTPOLAR        SCB->ext.polar  /* Polarity Register */

// reset ID register
#define RSID            SCB->rsid

// code securiti protection register
#define CSPR            SBC->cspr

// MAM defines
#define MAMCR_OFF     0
#define MAMCR_PART    1
#define MAMCR_FULL    2


// MEMMAP defines
#define MEMMAP_BBLK   0                 // Interrupt Vectors in Boot Block
#define MEMMAP_FLASH  1                 // Interrupt Vectors in Flash
#define MEMMAP_SRAM   2                 // Interrupt Vectors in SRAM

// PLL defines & computations
// PLLCON Register Bit Definitions
#define PLLCON_PLLE   (1 << 0)          // PLL Enable
#define PLLCON_PLLC   (1 << 1)          // PLL Connect

// PLLSTAT Register Bit Definitions
#define PLLSTAT_LOCK  (1 << 10)         // PLL Lock Status Bit

// RSID konstante
#define RSID_POR  (1 << 0)
#define RSID_EXTR (1 << 1)
#define RSID_WDTR (1 << 2)
#define RSID_BODR (1 << 3)


// power control bits in PCONP register
#define PCONP_TIMER0    (1 << 1)
#define PCONP_TIMER1    (1 << 2)
#define PCONP_UART0        (1 << 3)
#define PCONP_UART1        (1 << 4)
#define PCONP_PWM0        (1 << 5)
#define PCONP_I2C0        (1 << 7)
#define PCONP_SPI0        (1 << 8)
#define PCONP_RTC        (1 << 9)
#define PCONP_SPI1        (1 << 10)
#define PCONP_AD0        (1 << 12)
#define PCONP_I2C1        (1 << 19)
#define PCONP_AD1        (1 << 20)
#define PCONP_USB        (1 << 31)

#define PCONP_SSP  PCONP_SPI1
#define PCONP_SSP0 PCONP_SSP

#define PCONP_MASK (PCONP_TIMER0 | PCONP_TIMER1 | PCONP_UART0 | PCONP_UART1 | PCONP_PWM0 | PCONP_I2C0 | \
    PCONP_SPI0 | PCONP_RTC | PCONP_SSP | PCONP_AD0 | PCONP_I2C1 | PCONP_AD1 | PCONP_USB)

///////////////////////////////////////////////////////////////////////////////

// Vectored Interrupt Controller Registers (VIC)
typedef struct
{
    REG32 irqStatus;                      // IRQ Status Register
    REG32 fiqStatus;                      // FIQ Status Register
    REG32 rawIntr;                        // Raw Interrupt Status Register
    REG32 intSelect;                      // Interrupt Select Register
    REG32 intEnable;                      // Interrupt Enable Register
    REG32 intEnClear;                     // Interrupt Enable Clear Register
    REG32 softInt;                        // Software Interrupt Register
    REG32 softIntClear;                   // Software Interrupt Clear Register
    REG32 protection;                     // Protection Enable Register
    REG32 _pad0[3];
    REG32 vectAddr;                       // Vector Address Register
    REG32 defVectAddr;                    // Default Vector Address Register
    REG32 _pad1[50];
    REG32 vectAddr0;                      // Vector Address 0 Register
    REG32 vectAddr1;                      // Vector Address 1 Register
    REG32 vectAddr2;                      // Vector Address 2 Register
    REG32 vectAddr3;                      // Vector Address 3 Register
    REG32 vectAddr4;                      // Vector Address 4 Register
    REG32 vectAddr5;                      // Vector Address 5 Register
    REG32 vectAddr6;                      // Vector Address 6 Register
    REG32 vectAddr7;                      // Vector Address 7 Register
    REG32 vectAddr8;                      // Vector Address 8 Register
    REG32 vectAddr9;                      // Vector Address 9 Register
    REG32 vectAddr10;                     // Vector Address 10 Register
    REG32 vectAddr11;                     // Vector Address 11 Register
    REG32 vectAddr12;                     // Vector Address 12 Register
    REG32 vectAddr13;                     // Vector Address 13 Register
    REG32 vectAddr14;                     // Vector Address 14 Register
    REG32 vectAddr15;                     // Vector Address 15 Register
    REG32 _pad2[48];
    REG32 vectCntl0;                      // Vector Control 0 Register
    REG32 vectCntl1;                      // Vector Control 1 Register
    REG32 vectCntl2;                      // Vector Control 2 Register
    REG32 vectCntl3;                      // Vector Control 3 Register
    REG32 vectCntl4;                      // Vector Control 4 Register
    REG32 vectCntl5;                      // Vector Control 5 Register
    REG32 vectCntl6;                      // Vector Control 6 Register
    REG32 vectCntl7;                      // Vector Control 7 Register
    REG32 vectCntl8;                      // Vector Control 8 Register
    REG32 vectCntl9;                      // Vector Control 9 Register
    REG32 vectCntl10;                     // Vector Control 10 Register
    REG32 vectCntl11;                     // Vector Control 11 Register
    REG32 vectCntl12;                     // Vector Control 12 Register
    REG32 vectCntl13;                     // Vector Control 13 Register
    REG32 vectCntl14;                     // Vector Control 14 Register
    REG32 vectCntl15;                     // Vector Control 15 Register
} vicRegs_t;

// VIC Channel Assignments
#define VIC_WDT         0
#define VIC_TIMER0      4
#define VIC_TIMER1      5
#define VIC_UART0       6
#define VIC_UART1       7
#define VIC_PWM         8
#define VIC_PWM0        8
#define VIC_I2C         9
#define VIC_SPI         10
#define VIC_SPI0        10
#define VIC_SPI1        11
#define VIC_PLL         12
#define VIC_RTC         13
#define VIC_EINT0       14
#define VIC_EINT1       15
#define VIC_EINT2       16
#define VIC_EINT3       17
#define VIC_ADC         18
#define VIC_ADC0        18
#define VIC_I2C1        19
#define VIC_BOD         20
#define VIC_ADC1        21
#define VIC_USB         22

// Vector Control Register bit definitions
#define VIC_ENABLE      (1 << 5)

// Convert Channel Number to Bit Value
#define VIC_BIT(chan)   (1 << (chan))

// Vectored Interrupt Controller
#define VIC             ((vicRegs_t *)0xFFFFF000)

// Vectored Interrupt Controller Registers
#define VICIRQStatus    VIC->irqStatus  /* IRQ Status Register */
#define VICFIQStatus    VIC->fiqStatus  /* FIQ Status Register */
#define VICRawIntr      VIC->rawIntr    /* Raw Interrupt Status Register */
#define VICIntSelect    VIC->intSelect  /* Interrupt Select Register */
#define VICIntEnable    VIC->intEnable  /* Interrupt Enable Register */
#define VICIntEnClear   VIC->intEnClear /* Interrupt Enable Clear Register */
#define VICSoftInt      VIC->softInt    /* Software Interrupt Register */
#define VICSoftIntClear VIC->softIntClear /* Software Interrupt Clear Register */
#define VICProtection   VIC->protection /* Protection Enable Register */
#define VICVectAddr     VIC->vectAddr   /* Vector Address Register */
#define VICDefVectAddr  VIC->defVectAddr /* Default Vector Address Register */
#define VICVectAddr0    VIC->vectAddr0  /* Vector Address 0 Register */
#define VICVectAddr1    VIC->vectAddr1  /* Vector Address 1 Register */
#define VICVectAddr2    VIC->vectAddr2  /* Vector Address 2 Register */
#define VICVectAddr3    VIC->vectAddr3  /* Vector Address 3 Register */
#define VICVectAddr4    VIC->vectAddr4  /* Vector Address 4 Register */
#define VICVectAddr5    VIC->vectAddr5  /* Vector Address 5 Register */
#define VICVectAddr6    VIC->vectAddr6  /* Vector Address 6 Register */
#define VICVectAddr7    VIC->vectAddr7  /* Vector Address 7 Register */
#define VICVectAddr8    VIC->vectAddr8  /* Vector Address 8 Register */
#define VICVectAddr9    VIC->vectAddr9  /* Vector Address 9 Register */
#define VICVectAddr10   VIC->vectAddr10 /* Vector Address 10 Register */
#define VICVectAddr11   VIC->vectAddr11 /* Vector Address 11 Register */
#define VICVectAddr12   VIC->vectAddr12 /* Vector Address 12 Register */
#define VICVectAddr13   VIC->vectAddr13 /* Vector Address 13 Register */
#define VICVectAddr14   VIC->vectAddr14 /* Vector Address 14 Register */
#define VICVectAddr15   VIC->vectAddr15 /* Vector Address 15 Register */
#define VICVectCntl0    VIC->vectCntl0  /* Vector Control 0 Register */
#define VICVectCntl1    VIC->vectCntl1  /* Vector Control 1 Register */
#define VICVectCntl2    VIC->vectCntl2  /* Vector Control 2 Register */
#define VICVectCntl3    VIC->vectCntl3  /* Vector Control 3 Register */
#define VICVectCntl4    VIC->vectCntl4  /* Vector Control 4 Register */
#define VICVectCntl5    VIC->vectCntl5  /* Vector Control 5 Register */
#define VICVectCntl6    VIC->vectCntl6  /* Vector Control 6 Register */
#define VICVectCntl7    VIC->vectCntl7  /* Vector Control 7 Register */
#define VICVectCntl8    VIC->vectCntl8  /* Vector Control 8 Register */
#define VICVectCntl9    VIC->vectCntl9  /* Vector Control 9 Register */
#define VICVectCntl10   VIC->vectCntl10 /* Vector Control 10 Register */
#define VICVectCntl11   VIC->vectCntl11 /* Vector Control 11 Register */
#define VICVectCntl12   VIC->vectCntl12 /* Vector Control 12 Register */
#define VICVectCntl13   VIC->vectCntl13 /* Vector Control 13 Register */
#define VICVectCntl14   VIC->vectCntl14 /* Vector Control 14 Register */
#define VICVectCntl15   VIC->vectCntl15 /* Vector Control 15 Register */

// preko nizova
#define VICVectAddrList ((unsigned long*) &VICVectAddr0)
#define VICVectCntlList ((unsigned long*) &VICVectCntl0)


///////////////////////////////////////////////////////////////////////////////

// SSP Registers
typedef struct
{
    REG32 cr0;                             // Control Register 0
    REG32 cr1;                             // Control Register 1
    REG32 dr;                             // Data Register
    REG32 sr;                             // Status Register
    REG32 cpsr;                            // Clock Prescale Register
    REG32 imsc;                            // Interrupt Mask Set and Clear Register
    REG32 ris;                            // Raw Interrupt Status Register
    REG32 mis;                            // Masked Interrupt Status Register
    REG32 icr;                           // SSPICR Interrupt Clear Register
} sspRegs_t;


// SSPCR0 bits
#define SSP_DSS     0x0f   // Data Size Select.
#define SSP_FRF     0x30   // Frame Format.
#define SSP_FRF_SPI 0x00   // Frame Format SPI
#define SSP_FRF_SSI 0x10   // Frame Format SSI
#define SSP_FRF_MWI 0x20   // Frame Format Microwire
#define SSP_CPOL    0x40   // Clock Out Polarity.
#define SSP_CPHA    0x0080 // Clock Out Phase.
#define SSP_SCR     0xff00 // Serial Clock Rate.

// SSPCR1 bits
#define SSP_LBM     0x01   // Loop Back Mode.
#define SSP_SSE     0x02   // SSP Enable.
#define SSP_MS      0x04   // Master/Slave Mode.
#define SSP_SOD     0x08   // Slave Output Disable.

// SSPSR bits
#define SSP_TFE     0x01   // Transmit FIFO Empty.
#define SSP_TNF     0x02   // Transmit FIFO Not Full.
#define SSP_RNE     0x04   // Receive FIFO Not Empty.
#define SSP_RFF     0x08   // Receive FIFO Full.
#define SSP_BSY     0x10   // Busy.

// SSPCPSR bits
#define SSP_CPSDVSR 0x00ff

// SSPIMSC bits
#define SSP_RORIM   0x01   // Receive Overrun
#define SSP_RTIM    0x02   // Receive Timeout
#define SSP_RXIM    0x04   // Rx FIFO is at least half full
#define SSP_TXIM    0x08   // Tx FIFO is at least half empty.

// SSPRIS bits
#define SSP_RORRIS  0x01   // 1 if another frame was completely received while the RxFIFO was full
#define SSP_RTRIS   0x02   // 1 if when there is a Receive Timeout condition.
#define SSP_RXRIS   0x04   // 1 if the Rx FIFO is at least half full.
#define SSP_TXRIS   0x08   // 1 if the Tx FIFO is at least half empty.

// SSPMIS bits
#define SSP_RORMIS  0x01   // 1 if another frame was completely received while the RxFIFO was full
#define SSP_RTMIS   0x02   // 1 when there is a Receive Timeout condition
#define SSP_RXMIS   0x04   // 1 if the Rx FIFO is at least half full,
#define SSP_TXMIS   0x08   // 1 if the Tx FIFO is at least half empty

// SSPICR bits
#define SSP_RORIC   0x01   // clears the “frame was received when RxFIFO was full” interrupt.
#define SSP_RTIC    0x02   // clears the Receive Timeout interrupt.

// Synchronous Serial Port (SSP - SPI1)
#define SSP            ((sspRegs_t *)0xE0068000)

// SSP Registers
#define SSPCR0          SSP->cr0        /* Control Register 0 */
#define SSPCR1          SSP->cr1        /* Control Register 1 */
#define SSPDR           SSP->dr         /* Data Register */
#define SSPSR           SSP->sr         /* Status Register */
#define SSPCPSR         SSP->cpsr       /* Clock Prescale Register */
#define SSPIMSC         SSP->imsc       /* Interrupt Mask Set and Clear Register */
#define SSPRIS          SSP->ris        /* Raw Interrupt Status Register */
#define SSPMIS          SSP->mis        /* Masked Interrupt Status Register */
#define SSPICR          SSP->icr        /* SSPICR Interrupt Clear Register */

// SSP Registers
#define SSP0CR0          SSP->cr0        /* Control Register 0 */
#define SSP0CR1          SSP->cr1        /* Control Register 1 */
#define SSP0DR           SSP->dr         /* Data Register */
#define SSP0SR           SSP->sr         /* Status Register */
#define SSP0CPSR         SSP->cpsr       /* Clock Prescale Register */
#define SSP0IMSC         SSP->imsc       /* Interrupt Mask Set and Clear Register */
#define SSP0RIS          SSP->ris        /* Raw Interrupt Status Register */
#define SSP0MIS          SSP->mis        /* Masked Interrupt Status Register */
#define SSP0ICR          SSP->icr        /* SSPICR Interrupt Clear Register */


///////////////////////////////////////////////////////////////////////////////

/* USB Controller */
// USB interrupt register
#define USB_INT_STAT    (*(volatile unsigned long*)(0xe01fc1c0))

#define USB_BASE_ADDR        0xE0090000            /* USB Base Address */
/* Device Interrupt Registers */
#define DEV_INT_STAT    (*(volatile unsigned long *)(USB_BASE_ADDR + 0x00))
#define DEV_INT_EN      (*(volatile unsigned long *)(USB_BASE_ADDR + 0x04))
#define DEV_INT_CLR     (*(volatile unsigned long *)(USB_BASE_ADDR + 0x08))
#define DEV_INT_SET     (*(volatile unsigned long *)(USB_BASE_ADDR + 0x0C))
#define DEV_INT_PRIO    (*(volatile unsigned long *)(USB_BASE_ADDR + 0x2C))

/* Endpoint Interrupt Registers */
#define EP_INT_STAT     (*(volatile unsigned long *)(USB_BASE_ADDR + 0x30))
#define EP_INT_EN       (*(volatile unsigned long *)(USB_BASE_ADDR + 0x34))
#define EP_INT_CLR      (*(volatile unsigned long *)(USB_BASE_ADDR + 0x38))
#define EP_INT_SET      (*(volatile unsigned long *)(USB_BASE_ADDR + 0x3C))
#define EP_INT_PRIO     (*(volatile unsigned long *)(USB_BASE_ADDR + 0x40))

/* Endpoint Realization Registers */
#define REALIZE_EP      (*(volatile unsigned long *)(USB_BASE_ADDR + 0x44))
#define EP_INDEX        (*(volatile unsigned long *)(USB_BASE_ADDR + 0x48))
#define MAXPACKET_SIZE  (*(volatile unsigned long *)(USB_BASE_ADDR + 0x4C))

/* Command Reagisters */
#define CMD_CODE        (*(volatile unsigned long *)(USB_BASE_ADDR + 0x10))
#define CMD_DATA        (*(volatile unsigned long *)(USB_BASE_ADDR + 0x14))

/* Data Transfer Registers */
#define RX_DATA         (*(volatile unsigned long *)(USB_BASE_ADDR + 0x18))
#define TX_DATA         (*(volatile unsigned long *)(USB_BASE_ADDR + 0x1C))
#define RX_PLENGTH      (*(volatile unsigned long *)(USB_BASE_ADDR + 0x20))
#define TX_PLENGTH      (*(volatile unsigned long *)(USB_BASE_ADDR + 0x24))
#define USB_CTRL        (*(volatile unsigned long *)(USB_BASE_ADDR + 0x28))

///////////////////////////////////////////////////////////////////////////////

// External Memory Controller Registers
typedef struct
{
  REG32 bcfg0;                          // Bank 0 Configuration Register
  REG32 bcfg1;                          // Bank 1 Configuration Register
  REG32 bcfg2;                          // Bank 2 Configuration Register
  REG32 bcfg3;                          // Bank 3 Configuration Register
} emcRegs_t;

///////////////////////////////////////////////////////////////////////////////

#endif
