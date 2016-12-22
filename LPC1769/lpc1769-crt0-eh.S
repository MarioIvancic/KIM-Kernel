/*****************************************************************************/
/* startup_LPC17xx.s: Startup file for LPC17xx device series                 */
/*****************************************************************************/
/* Version: CodeSourcery Sourcery G++ Lite (with CS3)                        */
/*****************************************************************************/

/* mthomas:
    - replaced CM3 _start with data-copy, bss-init
    - added syntax unified
    - moved NMI to UsageFault handlers in reset section to minimize checksum
    modifications
*/

/* Mario Ivancic, v3:
    - changed interface to ld script
    - changed stack init code
*/

#ifndef __CRT0_STARTUP_DELAY
#define __CRT0_STARTUP_DELAY 1
#endif

#ifndef __CODE_READ_PROTECTION_WORD
#define __CODE_READ_PROTECTION_WORD 0
#endif

.global  __interrupt_vector
.global  __interrupt_vector2
.global  _start
.global  __Default_IRQ_Handler
.global  __Default_Exception_Handler
.global str_serial_number
.global code_read_protection_word
.global abort

.global HardFault_Handler
.global MemManage_Handler
.global BusFault_Handler
.global UsageFault_Handler


.extern _etext
.extern _sdata
.extern _edata
.extern _sbss
.extern _ebss
.extern _end
.extern _stack


.syntax unified

    /* Vector Table */

    .section ".cm3.interrupt_vector"
    .type   __interrupt_vector, %object
__interrupt_vector:
    .long   _stack                      /* Top of Stack                 */
    .long   _start                      /* Reset Handler                */
    .long   NMI_Handler                 /* NMI Handler                  */
    .long   HardFault_Handler           /* Hard Fault Handler           */
    .long   MemManage_Handler           /* MPU Fault Handler            */
    .long   BusFault_Handler            /* Bus Fault Handler            */
    .long   UsageFault_Handler          /* Usage Fault Handler          */

    /* Checksum, adjusted in linker script, see LPC1xxx manual
    crc = 0 - (_stack + _start + NMI_Handler + HardFault_Handler
        + MemManage_Handler + BusFault_Handler + UsageFault_Handler)
    .long   0xefffd9aa                                                   */

    .size   __interrupt_vector, . - __interrupt_vector

    .section ".cm3.interrupt_vector2"
    .type   __interrupt_vector2, %object
__interrupt_vector2:
    .long   0                           /* Reserved                     */
    .long   0                           /* Reserved                     */
    .long   0                           /* Reserved                     */
    .long   SVC_Handler                 /* SVCall Handler               */
    .long   DebugMon_Handler            /* Debug Monitor Handler        */
    .long   0                           /* Reserved                     */
    .long   PendSV_Handler              /* PendSV Handler               */
    .long   SysTick_Handler             /* SysTick Handler              */

    /* External Interrupts */
    .long   WDT_IRQHandler              /* 16: Watchdog Timer               */
    .long   TIMER0_IRQHandler           /* 17: Timer0                       */
    .long   TIMER1_IRQHandler           /* 18: Timer1                       */
    .long   TIMER2_IRQHandler           /* 19: Timer2                       */
    .long   TIMER3_IRQHandler           /* 20: Timer3                       */
    .long   UART0_IRQHandler            /* 21: UART0                        */
    .long   UART1_IRQHandler            /* 22: UART1                        */
    .long   UART2_IRQHandler            /* 23: UART2                        */
    .long   UART3_IRQHandler            /* 24: UART3                        */
    .long   PWM1_IRQHandler             /* 25: PWM1                         */
    .long   I2C0_IRQHandler             /* 26: I2C0                         */
    .long   I2C1_IRQHandler             /* 27: I2C1                         */
    .long   I2C2_IRQHandler             /* 28: I2C2                         */
    .long   SPI_IRQHandler              /* 29: SPI                          */
    .long   SSP0_IRQHandler             /* 30: SSP0                         */
    .long   SSP1_IRQHandler             /* 31: SSP1                         */
    .long   PLL0_IRQHandler             /* 32: PLL0 Lock (Main PLL)         */
    .long   RTC_IRQHandler              /* 33: Real Time Clock              */
    .long   EINT0_IRQHandler            /* 34: External Interrupt 0         */
    .long   EINT1_IRQHandler            /* 35: External Interrupt 1         */
    .long   EINT2_IRQHandler            /* 36: External Interrupt 2         */
    .long   EINT3_IRQHandler            /* 37: External Interrupt 3         */
    .long   ADC_IRQHandler              /* 38: A/D Converter                */
    .long   BOD_IRQHandler              /* 39: Brown-Out Detect             */
    .long   USB_IRQHandler              /* 40: USB                          */
    .long   CAN_IRQHandler              /* 41: CAN                          */
    .long   DMA_IRQHandler              /* 42: General Purpose DMA          */
    .long   I2S_IRQHandler              /* 43: I2S                          */
    .long   ENET_IRQHandler             /* 44: Ethernet                     */
    .long   RIT_IRQHandler              /* 45: Repetitive Interrupt Timer   */
    .long   MCPWM_IRQHandler            /* 46: Motor Control PWM            */
    .long   QEI_IRQHandler              /* 47: Quadrature Encoder Interface */
    .long   PLL1_IRQHandler             /* 48: PLL1 Lock (USB PLL)          */

    .size   __interrupt_vector2, . - __interrupt_vector2


    .thumb


    /* Reset Handler */

    .section .cm3.reset, "x", %progbits
    .thumb_func
    .type   _start, %function
_start:
    .fnstart

    /* delay at startup to let JTAG-connection catch-up                     */
	LDR R0, =__CRT0_STARTUP_DELAY
1:
    subs r0, #1
	bne 1b

    /* Copy the data segment initializers from flash to SRAM */
    ldr   r1, =_etext
    ldr   r2, =_sdata
    ldr   r3, =_edata
1:  cmp   r2, r3
/*    ittt  lo
    ldrlo r0, [r1], #4
    strlo r0, [r2], #4
    blo   1b
*/
    bhs 1f
    ldr r0, [r1], #4
    str r0, [r2], #4
    b   1b
1:


    /* Zero fill the bss segment. */
    movs  r0, #0
    ldr   r1, =_sbss
    ldr   r2, =_ebss
1:  cmp   r1, r2
/*    itt   lo
    strlo r0, [r1], #4
    blo   1b
*/
    bhs 1f
    str r0, [r1], #4
    b   1b
1:

    /* call __libc_init_array - needed for C++ support                      */
#ifdef __CPLUSPLUS_SUPPORT
    LDR     R0, =__libc_init_array
    BLX     R0
#endif

    /* call system init in NXP's Device CMSIS code                          */
#ifdef __CMSIS_SUPPORT
    LDR     R0, =SystemInit
    BLX     R0
#endif

    /* call main() */
    LDR     R0,=main
    BX      R0

    B .

    .pool
    .cantunwind
    .fnend
    .size   _start, . - _start

    /* abort function -- disable interrupts anf faults and wait forever */
    .type   abort, %function
abort:
    cpsid i
    cpsid f
    nop
    wfi
    b .
    .size   abort, . - abort



    /* do nothing function -- just returns */
    .type   __do_nothing_and_return, %function
__do_nothing_and_return:
    bx lr
    .size   __do_nothing_and_return, . - __do_nothing_and_return

    /* fault exception handler */
    .weak fault_exception_handler_c
    .set fault_exception_handler_c, __do_nothing_and_return
    .type __Default_Exception_Handler, %function
__Default_Exception_Handler:
    TST LR, #4      /* test where is the stack before exception */
	ITE EQ
	MRSEQ R0, MSP   /* main stack */
	MRSNE R0, PSP   /* process stack */
	STMDB sp!, {r4-r11} /* this works only if main stack is the only stack in use */
	LDR R1, =fault_exception_handler_c     /* call exception handler C function, if defined */
	BX R1
    .size   __Default_Exception_Handler, . - __Default_Exception_Handler


    /* Default IRQ handler -- just returns */
    .type   __Default_IRQ_Handler, %function
__Default_IRQ_Handler:
    bx lr
    .size   __Default_IRQ_Handler, . - __Default_IRQ_Handler


    /* Default exception and IRQ handlers */
    .macro  EXCEPTION handler
    .weak   \handler
    .set    \handler, __Default_Exception_Handler
    .endm

    .macro  IRQ handler
    .weak   \handler
    .set    \handler, __Default_IRQ_Handler
    .endm

    /* Exception Handlers */

    EXCEPTION HardFault_Handler
    EXCEPTION MemManage_Handler
    EXCEPTION BusFault_Handler
    EXCEPTION UsageFault_Handler


    /* IRQ Handlers */

    IRQ     NMI_Handler
    IRQ     SVC_Handler
    IRQ     DebugMon_Handler
    IRQ     PendSV_Handler
    IRQ     SysTick_Handler

    IRQ     WDT_IRQHandler
    IRQ     TIMER0_IRQHandler
    IRQ     TIMER1_IRQHandler
    IRQ     TIMER2_IRQHandler
    IRQ     TIMER3_IRQHandler
    IRQ     UART0_IRQHandler
    IRQ     UART1_IRQHandler
    IRQ     UART2_IRQHandler
    IRQ     UART3_IRQHandler
    IRQ     PWM1_IRQHandler
    IRQ     I2C0_IRQHandler
    IRQ     I2C1_IRQHandler
    IRQ     I2C2_IRQHandler
    IRQ     SPI_IRQHandler
    IRQ     SSP0_IRQHandler
    IRQ     SSP1_IRQHandler
    IRQ     PLL0_IRQHandler
    IRQ     RTC_IRQHandler
    IRQ     EINT0_IRQHandler
    IRQ     EINT1_IRQHandler
    IRQ     EINT2_IRQHandler
    IRQ     EINT3_IRQHandler
    IRQ     ADC_IRQHandler
    IRQ     BOD_IRQHandler
    IRQ     USB_IRQHandler
    IRQ     CAN_IRQHandler
    IRQ     DMA_IRQHandler
    IRQ     I2S_IRQHandler
    IRQ     ENET_IRQHandler
    IRQ     RIT_IRQHandler
    IRQ     MCPWM_IRQHandler
    IRQ     QEI_IRQHandler
    IRQ     PLL1_IRQHandler

	/* hard fault excaption handler
	.type   HardFault_Handler, %function
HardFault_Handler:
	TST LR, #4
	ITE EQ
	MRSEQ R0, MSP
	MRSNE R0, PSP
	B fault_exception_handler_c
    .size   HardFault_Handler, . - HardFault_Handler */


    /* 32 bytes for serial number.
    Must be on known location 0x02dc because it's patched before flashing   */
    .section .cm3.serial_number
str_serial_number:
	.space 32, 0

    /* code read protection on known location 0x02fc, defined in makefile   */
code_read_protection_word:
	.word __CODE_READ_PROTECTION_WORD

    .end
