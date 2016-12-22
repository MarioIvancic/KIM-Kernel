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

