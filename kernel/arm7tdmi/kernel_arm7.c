// kernel_arm7.c
// arm7tdmi specific stuff
// lpc2148

//#include "appconfig.h"
#include "lpcmcu.h"
#include "kernel_arch.h"
#include "kernel.h"

// for debugging, to slow down kernel_tick_proc rate
// undefine for release
//#define KERNEL_SYSTICK_PRESCALLER 100

// available range is from 0 (highest priority) to 15 (lowest priority)
#ifndef KERNEL_TIMER_VIC_SLOT
#define KERNEL_TIMER_VIC_SLOT 8
#endif

// available range for LPC2148 is 4 (TIMER0), 5 (TIMER1), 8 (PWM0)
#ifndef KERNEL_TIMER_VIC_TYPE
#define KERNEL_TIMER_VIC_TYPE 4
#endif

// available range: TMR0, TMR1, PWM0
#ifndef KERNEL_TIMER_BASE_ADDR
#define KERNEL_TIMER_BASE_ADDR TMR0
#endif


// timer PCLK divider (TimerCLK = PCLK / KERNEL_TIMER_PCLK_DIV)
#ifndef KERNEL_TIMER_PCLK_DIV
#define KERNEL_TIMER_PCLK_DIV 3
#endif

// timer match channel (0, 1, 2 or 3)
#ifndef KERNEL_TIMER_MATCH_CH
#define KERNEL_TIMER_MATCH_CH 0
#endif


#define _T_MR(NAME) mr ## NAME
#define __T_MR(NAME) _T_MR(NAME)
#define T_MR __T_MR(KERNEL_TIMER_MATCH_CH)


/*
    The following diagram shows the chronology of the stack pointer when a
    switch happens between task one and task two.

    |Task1 Stack|                        |Task2 Stack|
    |           |                        |           |
    |           |                        |           |
    |           |     |    SP     |      |           |      |     SP    |
    |    PC     |           |            |    PC     |            |
    |    r12    |           |            |    r12    |            |
    |    r5     |           |            |    r5     |            |
    |    r4     |           |            |    r4     |            |
    |    r3     |           |            |    r3     |            |
    |    r2     |           |            |    r2     |            |
    |    r1     |           |            |    r1     |            |
    |    r0     |           v            |    r0     |            |
    |    LR     |           |            |    LR     |            |
    |    r11    |           |            |    r11    |            |
    |    r10    |           |            |    r10    |            |
    |    r9     |           |            |    r9     |            |
    |    r8     |   pushed by switcher   |    r8     |    poped by switcher
    |    r7     |           |            |    r7     |            |
    |    r6     |           v            |    r6     |            |
    |   CPSR    |     |    SP     |      |   CPSR    |      |    SP     |
    |                       |                                     |
    |                       |                                     |
    |                       +-----> switcher re-assigns SP -------+
*/



// This defines the stack frame that must be saved by the software
typedef struct
{
    uint32_t cpsr;
    uint32_t r6;
    uint32_t r7;
    uint32_t r8;
    uint32_t r9;
    uint32_t r10;
    uint32_t r11;
    uint32_t lr;
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r4;
    uint32_t r5;
    uint32_t r12;
    uint32_t pc;
} stack_frame_t;


// static volatile uatomic_t hs_ticks;
static volatile uint32_t tick_period;
//static volatile uint32_t last_val;

volatile atomic_t kernel_do_cs;


void SysTick_Handler(void);

extern uint32_t _kernel_get_cpsr(void);


// SVC for kernel startup
static inline void SVC_kernel_statup(void){ asm volatile("\tSWI 0\n"); }
#define SVCN_kernel_statup 0


// svc for exit_critical and context switch
static inline void SVC_exit_critical_cs(atomic_t* state){ asm volatile("\tSWI 1\n"); }
#define SVCN_exit_critical_cs 1


#define T_BIT 0x20
#define F_BIT 0x40		// when F bit is set, FIQ is disabled
#define I_BIT 0x80		// when I bit is set, IRQ is disabled


#ifndef REG32
    #define REG32 volatile uint32_t
#endif
#ifndef BIT
    #define BIT(n) (1UL << (n))
#endif

// Timer & PWM Registers (only used one)
typedef struct
{
    REG32 ir;                             // 0x00 Interrupt Register
    REG32 tcr;                            // 0x04 Timer Control Register
    REG32 tc;                             // 0x08 Timer Counter
    REG32 pr;                             // 0x0c Prescale Register
	REG32 pc;                             // 0x10 Prescale Counter Register
    REG32 mcr;                            // 0x14 Match Control Register
    REG32 mr0;                            // 0x18 Match Register 0
    REG32 mr1;                            // 0x1c Match Register 1
    REG32 mr2;                            // 0x20 Match Register 2
    REG32 mr3;                            // 0x24 Match Register 3
} lpc_timer_pwm_t;

// pointer to system timer
static lpc_timer_pwm_t *timer;


void user_timer_handler(void) __attribute__((weak));


// C part of SVC handler.
// It's called by ASM part with pointer to stack frame in arg
// it implements core kernel system calls and modify task table
// but it must not change kernel_current_task
void SVC_HandlerC(stack_frame_t* arg)
{
    unsigned svc_number;
    // Stack frame pointed by arg contains:
    // r0, r1 .. r12, the return address and CPSR
    // So, r0=arg[0], r1=arg[1], ...
    if(arg->cpsr & T_BIT) svc_number = *((unsigned short*)(arg->pc - 2)) & 0x0ff;
    else svc_number = *((unsigned*)(arg->pc - 4)) & 0x00ff;

    switch(svc_number)
    {
        case SVCN_kernel_statup:    // handler mode kernel_start code
            kernel_running = 1;
            
            // main idle loop context is saved to kernel_current_task
            // now we have to call kernel_context_switcher() to find
            // highest priority task to run
            kernel_context_switcher();
            // ASM part of the handler will do the rest
        break;

        case SVCN_exit_critical_cs: // exit_critical
        {
            // critical section on ARM7TDMI is implemented by disabling IRQ
            // in r0 is pointer to original interrupt enable/disable state
			unsigned t = (*(unsigned*)arg->r0) & I_BIT;
			arg->cpsr &= ~I_BIT;
            arg->cpsr |= t;
            kernel_context_switcher();
        }
        break;

        default:    // unknown, do nothing
        break;
    }
}



// This function initialises and start the system tick timer and its interrupt.
// Counter is in free running mode to generate periodical interrupts.
int kernel_arch_init(int ticks_for_interrupt)
{
	// last_val = ticks_for_interrupt - 1;
    
    // setup handler in the VIC
    
    // Now disable interrupts at the VIC.
	VICIntEnClear = VIC_BIT(KERNEL_TIMER_VIC_TYPE);
    
    // Set up vector to point at service routine
    VICVectAddrList[KERNEL_TIMER_VIC_SLOT] = (unsigned long) &SysTick_Handler;
    
    // Set up the source. Tag the vector with the source
    // number and flag it as enabled.
    VICVectCntlList[KERNEL_TIMER_VIC_SLOT] = KERNEL_TIMER_VIC_TYPE | VIC_ENABLE;
    
    // Flag interrupt as an IRQ by clearing the corresponding bit.
    VICIntSelect &= ~(VIC_BIT(KERNEL_TIMER_VIC_TYPE));
    
    // Interrupt has been setup, now enable it.  Interrupt may now
	// be flagged to the CPU.
 	VICIntEnable = VIC_BIT(KERNEL_TIMER_VIC_TYPE);
            
    tick_period = ticks_for_interrupt;

    timer = (lpc_timer_pwm_t*) KERNEL_TIMER_BASE_ADDR;
    unsigned pv = 0;

    switch((int)timer)
    {
        case (int)TMR0: pv = PCONP_TIMER0; break;
        case (int)TMR1: pv = PCONP_TIMER1; break;
        case (int)PWM:  pv = PCONP_PWM0;   break;
        default: pv = 0;
    }

    PCONP = (PCONP & PCONP_MASK) | pv;
	

	// setup Timer to count forever
	// reset & disable timer
	timer->tcr = BIT(1);
	
	// set the prescale divider
	timer->pr = KERNEL_TIMER_PCLK_DIV - 1;
	
	timer->tc = 0;
	
	// set timer match register
    timer->T_MR  = ticks_for_interrupt;
	
	// enable interrupt-on-match
	timer->mcr |= (1 << (3 * KERNEL_TIMER_MATCH_CH));
	
	// reset interrupt requests
	timer->ir = timer->ir;
	
	// enable timer
	timer->tcr = BIT(0);
	
    return 0;
}


// kernel_current_task points to first highest priority task
int kernel_arch_start(void)
{
    // enable SysTick interrupt

    // enable all interrupts
    // not needed on Cortex-M3
    //__enable_irq();

    // switch to handler mode to start kernel
    // we have to adjust PSP as if it points to hw_stack_frame
    SVC_kernel_statup();

    return 0;           // not reached
}




void kernel_arch_init_task_stack(TCB_t* tcb, void* fcn, void* arg)
{
    // adjust top of stack
    tcb->sp -= sizeof(stack_frame_t);
    stack_frame_t* sf = (stack_frame_t*) (tcb->sp);
    sf->r0  = (uint32_t) arg;
    sf->r1  = 0x11111111;
    sf->r2  = 0x22222222;
    sf->r3  = 0x33333333;
    sf->r4  = 0x44444444;
    sf->r5  = 0x55555555;
    sf->r6  = 0x66666666;
    sf->r7  = 0x77777777;
    sf->r8  = 0x88888888;
    sf->r9  = 0x99999999;
    sf->r10 = 0xaaaaaaaa;
    sf->r11 = 0xbbbbbbbb;
    sf->r12 = 0xcccccccc;
    sf->lr  = (uint32_t)&task_exit;
    sf->pc  = (uint32_t)fcn;
	
	// default CPSR value is same as of current task;
    sf->cpsr = _kernel_get_cpsr();
}







void user_timer_handler(void)
{
	// just clear interrupt request
	timer->ir = timer->ir;
}



#ifdef KERNEL_SYSTICK_PRESCALLER
// systick_prescaller is used to slow down kernel_tick_proc rate, for debugging
uint32_t systick_prescaller = KERNEL_SYSTICK_PRESCALLER;
#endif  // KERNEL_SYSTICK_PRESCALLER

void SysTick_Handler(void)
{
	if(timer->ir & BIT(KERNEL_TIMER_MATCH_CH))
	{
		timer->ir = BIT(KERNEL_TIMER_MATCH_CH);
		
		// reinit timer match register
		timer->T_MR += tick_period;
		
#ifdef KERNEL_SYSTICK_PRESCALLER
		if(! --systick_prescaller)
		{
			systick_prescaller = KERNEL_SYSTICK_PRESCALLER;
			kernel_tick_proc();
		}
#else
		kernel_tick_proc();
#endif  // KERNEL_SYSTICK_PRESCALLER

	}
	else user_timer_handler();
}



void kernel_exit_critical_cs(atomic_t* state)
{
    SVC_exit_critical_cs(state);
}


// this is not portable, on some arch where sizeof(word) < sizeof(int)
// interrupts should be disabled during reading of kernel_ticks
unsigned int kernel_get_ticks(void)
{
    return kernel_ticks;
}


unsigned kernel_get_elapsed_ticks(unsigned start_time)
{
    return kernel_ticks - start_time;
}


unsigned kernel_get_hsticks(void)
{
    return timer->tc;
}


unsigned kernel_get_elapsed_hsticks(unsigned start_time)
{
    return timer->tc - start_time;
}


void kernel_trigger_cs(void)
{
    kernel_do_cs = 1;
}