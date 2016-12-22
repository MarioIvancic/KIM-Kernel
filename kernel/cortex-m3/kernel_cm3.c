// kernel_cm3.c
// ARM Cortex-M3 specific stuff
// Copyright (c) 2012, Mario Ivančić. See LINCENSE.txt for more details.
// Based on article http://coactionos.com/embedded%20design%20tips/2013/10/09/Tips-Context-Switching-on-the-Cortex-M3/

//#include "appconfig.h"
#include "lpcmcu.h"
#include "kernel_arch.h"
#include "kernel.h"

// for debugging, to slow down kernel_tick_proc rate
// undefine for release
//#define KERNEL_SYSTICK_PRESCALLER 100


/*
    The following diagram shows the chronology of the stack pointer when a
    switch happens between task one and task two.

    |Task1 Stack|                        |Task2 Stack|
    |           |                        |           |
    |           |                        |           |
    |           |     |    PSP    |      |           |      |    PSP    |
    |   xPSR    |           |            |   xPSR    |            |
    |    PC     |           |            |    PC     |            |
    |    LR     |           |            |    LR     |            |
    |    r12    |    pushed by NVIC      |    r12    |      poped by NVIC
    |    r3     |           |            |    r3     |            |
    |    r2     |           |            |    r2     |            |
    |    r1     |           |            |    r1     |            |
    |    r0     |           v            |    r0     |            |
---------------------       |        ---------------------        |
    |    r11    |     |    PSP    |      |    r11    |      |    PSP    |
    |    r10    |           |            |    r10    |            |
    |    r9     |           |            |    r9     |            |
    |    r8     |           |            |    r8     |            |
    |    r7     |   pushed by switcher   |    r7     |    poped by switcher
    |    r6     |           |            |    r6     |            |
    |    r5     |           v            |    r5     |            |
    |    r4     |     |    PSP    |      |    r4     |      |    PSP    |
    |                       |                                     |
    |                       |                                     |
    |                       +-----> switcher re-assigns PSP ------+
*/


// This defines the stack frame that is saved  by the hardware
typedef struct
{
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r12;
    uint32_t lr;
    uint32_t pc;
    uint32_t psr;
} hw_stack_frame_t;

// This defines the stack frame that must be saved by the software
typedef struct
{
    uint32_t r4;
    uint32_t r5;
    uint32_t r6;
    uint32_t r7;
    uint32_t r8;
    uint32_t r9;
    uint32_t r10;
    uint32_t r11;
} sw_stack_frame_t;


typedef struct
{
    sw_stack_frame_t swsf;
    hw_stack_frame_t hwsf;
} stack_frame_t;


static volatile unsigned hs_ticks;
static volatile uint32_t tick_period;
static volatile uint32_t last_val;



// this works only from priviledged code
#define Trigger_PendSV() *((uint32_t*)0xe000ed04) = (1UL << 28)
#define Trigger_SysTick() *((uint32_t*)0xe000ed04) = (1UL << 26)


/*
    From Pico[OS:
    Calculate suitable priorities for SVCall, SysTick and PendSV.
    PendSV is always lowest possible.
    SVCall priority is set to halfway of available priority range to
    leave room for high-speed interrupts that don't need Pico[OS access.
    Systick priority is always one step lower than SVCall
    (to ensure that timekeeping is done correctly).

    For systems that have 4 or 2 implemented priority bits
    this results in:

    Exception    Priority with 4 bits      Priority with 2 bits
    SVCall                7                         1
    SysTick               8                         2
    PendSV                15                        3

    Priority numbers here are those that CMSIS NVIC_SetPriority
    function accepts (ie. not that gets written into registers).

    Cortex-M0 has 2 bits always, I think.
    To block timer interrupts, basepri is set to SysTick level
    in Cortex-M3 (In -M0 primask is used to enable/disable interrupts).

    PendSV_prio = ((1<<__NVIC_PRIO_BITS) - 1);      // 31
    SVC_prio = PendSV_prio / 2;                     // 15
    SysTick_prio = SVC_prio + 1;                    // 16
*/

// PENDSV_PRIO is lowest, SVC_PRIO is in the middle of the range
// and SYSTICK_PRIO is one step lower than SVC_PRIO
#define PENDSV_PRIO     ((1<<__NVIC_PRIO_BITS) - 1)     // 31
#define SVC_PRIO        (PENDSV_PRIO / 2)               // 15
#define SYSTICK_PRIO    (SVC_PRIO + 1)                  // 16


// SVC for kernel startup
static inline void SVC_kernel_statup(void){ asm volatile("\tSVC 0\n"); }
#define SVCN_kernel_statup 0

// svc for exit_critical and context switch
static inline void SVC_exit_critical_cs(void){ asm volatile("\tSVC 1\n"); }
#define SVCN_exit_critical_cs 1

void kernel_trigger_cs(void)
{
    Trigger_PendSV();
}


// C part of SVC handler.
// It's called by ASM part with pointer to hw stack frame in arg
// it implements core kernel system calls and modify task table
// but it must not change kernel_current_task
static void SVC_HandlerC(hw_stack_frame_t* arg)
{
    int svc_number;
    // Stack frame pointed by arg contains:
    // r0, r1, r2, r3, r12, r14, the return address and xPSR
    // So, r0=arg[0], r1=arg[1], ...
    svc_number = ((char *)arg->pc)[-2];

    switch(svc_number)
    {
        case SVCN_kernel_statup:    // handler mode kernel_start code
            __ISB();
            kernel_running = 1;
            
            // main idle loop context is saved to kernel_current_task
            // now we have to call kernel_context_switcher() to find
            // highest priority task to run
            Trigger_PendSV();
        break;

        case SVCN_exit_critical_cs: // exit_critical
        {
            // critical section on ARM Cortex-M3 is implemented by disabling
            // SysTick and lower interrupts in basepri registers
            __set_BASEPRI(*((atomic_t*)arg[0]));
            Trigger_PendSV();
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
    // avoid impossible SysTick reload value
    if(ticks_for_interrupt > SysTick_LOAD_RELOAD_Msk) return -1;

    tick_period = ticks_for_interrupt;

    // set stack alignment on exeption entry to 8
    // so, there is no need for __atribute__((interrupt))
    // for hendler functions
    SCB->CCR |= (1UL << 9);

    // set Priority for Cortex-M3 System Interrupts
    // lowest priority for PendSV, then SysTick and SVCall
    NVIC_SetPriority (PendSV_IRQn,  PENDSV_PRIO);
    NVIC_SetPriority (SysTick_IRQn, SYSTICK_PRIO);
    NVIC_SetPriority (SVCall_IRQn,  SVC_PRIO);

    // setup SysTick Timer to count forever
    // disble SysTick IRQ and SysTick Timer
    SysTick->CTRL  = 0;

    // set reload register
    SysTick->LOAD  = ticks_for_interrupt - 1;

    // Load the SysTick Counter Value
    SysTick->VAL   = ticks_for_interrupt - 1;

    last_val = ticks_for_interrupt - 1;
    hs_ticks = 0;

    return 0;
}


// kernel_current_task points to first highest priority task
int kernel_arch_start(void)
{
    // enable SysTick interrupt

    // Enable SysTick IRQ and SysTick Timer
    SysTick->CTRL;      // read CTRL to clear SysTick_CTRL_COUNTFLAG
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
                   SysTick_CTRL_TICKINT_Msk   |
                   SysTick_CTRL_ENABLE_Msk;

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
    sf->swsf.r4  = 0x44444444;
    sf->swsf.r5  = 0x55555555;
    sf->swsf.r6  = 0x66666666;
    sf->swsf.r7  = 0x77777777;
    sf->swsf.r8  = 0x88888888;
    sf->swsf.r9  = 0x99999999;
    sf->swsf.r10 = 0xaaaaaaaa;
    sf->swsf.r11 = 0xbbbbbbbb;
    sf->hwsf.r0  = (uint32_t) arg;
    sf->hwsf.r1  = 0x11111111;
    sf->hwsf.r2  = 0x22222222;
    sf->hwsf.r3  = 0x33333333;
    sf->hwsf.r12 = 0xcccccccc;
    sf->hwsf.lr  = (uint32_t)&task_exit;
    sf->hwsf.pc  = (uint32_t)fcn;
    //sf->hwsf.psr = 0x21000000;            // default PSR value;
    sf->hwsf.psr = 0x01000000;            // default PSR value;
}





// SVC handler have 2 parts: ASM part and C part
// This is ASM part of SVC handler. It copy active SP to R0 and
// jumps to C part of SVC handler
void SVC_Handler(void) __attribute__((naked));
void SVC_Handler(void)
{
    asm volatile
    (
        "\n\t"
        "TST lr, 4\n\t"
        "ite eq\n\t"
        "MRSEQ r0, MSP\n\t"
        "MRSNE r0, PSP\n\t"
        "B SVC_HandlerC\n\t"
        :
        :
        :"r0"
    );
}




// PendSV handler is used for context switch
// this is lowest priority so it must be guarded
// other IRQ handlers that calls kernel functions
void PendSV_Handler(void) __attribute__((naked));
void PendSV_Handler(void)
{
    // The context is immediately saved
    // This saves sw_stack_frame_t on the curent tasks stack and returns updated PSP value
    // the Cortex-M3 pushes hw_stack_frame_t using hardware
    // - save current BASEPRI into r4
    // - set basepri to 255 (disable all interrupts)
    // - get current task PSP in r0
    // - store sw_stack_frame_t on current task stack using r0
    // - updated PSP is in r0
    // - copy updated PSP from r0 to kernel_current_task->sp
    // - call kernel_context_switcher()
    // - get current task PSP from kernel_current_task->sp in t (this is from new task)
    // - set lr to 0xfffffffd to force return using task stack
    // - load sw_stack_frame_t from current task stack using t
    // - copy updated t to current task PSP
    // - restore BASEPRI to original value from basepri (this will enable interrupts)
    // - return from PendSV handler
    asm volatile
    (
        "\n\t"
        "MOV r0, 0xff\n\t"
        "MRS r4, basepri\n\t"
        "MSR basepri, r0\n\t"
        "MRS r0, psp\n\t"
        "STMDB r0!, {r4-r11}\n\t"
        "LDR r5, =kernel_current_task\n\t"
        "LDR r1, [r5]\n\t"
        "STR r0, [r1]\n\t"
        "BL kernel_context_switcher\n\t"
        // Since the kernel_current_task has been updated
        // we have to load the last saved state of the new task
        // This loads sw_stack_frame_t from the task stack
        // the Cortex-M3 loads hw_stack_frame_t using hardware
        "LDR r1, [r5]\n\t"
        "LDR r1, [r1]\n\t"
        "MVN lr, 2\n\t"
        "MOV r0, r4\n\t"
        "LDMFD r1!, {r4-r11}\n\t"
        "MSR psp, r1\n\t"
        "MSR basepri, r0\n\t"
        "BX lr\n\t"
        ".align 4\n\t"
        :
        :
        :"r0", "r1", "r4", "r5", "lr"
    );
}



#ifdef KERNEL_SYSTICK_PRESCALLER
// systick_prescaller is used to slow down kernel_tick_proc rate, for debugging
uint32_t systick_prescaller = KERNEL_SYSTICK_PRESCALLER;
#endif  // KERNEL_SYSTICK_PRESCALLER

uatomic_t SysTick_HandlerC(void)
{
    uint32_t now;
    uint32_t diff;

    now = SysTick->VAL;
    diff = last_val - now;
    last_val = now;
    if(diff > tick_period) diff += tick_period;
    hs_ticks += diff;

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



void kernel_enter_critical(atomic_t* state)
{
    // SVC_enter_critical();
    // critical section on ARM Cortex-M3 is implemented by disabling
    // SysTick and lower interrupts in basepri register
    *state = __get_BASEPRI();
    __set_BASEPRI(SYSTICK_PRIO << (8 - __NVIC_PRIO_BITS));
}

void kernel_exit_critical(atomic_t* state)
{
    // SCV_exit_critical();
    // critical section on ARM Cortex-M3 is implemented by disabling
    // SysTick and lower interrupts in basepri registers
    __set_BASEPRI(*state);
}

void kernel_exit_critical_cs(atomic_t* state)
{
    SVC_exit_critical_cs();
}


// this is not portable, on some arch where sizeof(word) < sizeof(int)
// interrupts should be disabled during reading of kernel_ticks
unsigned int kernel_get_ticks(void)
{
    return kernel_ticks;
}

// this function is just to place pointr to t in r0
static void get_hsticks(unsigned * t)
{
    SCV_get_hstics();
}

unsigned kernel_get_hstics(void)
{
    unsigned t = 0;

    // call internal function
    get_hsticks(&t);

    return t;
}


unsigned kernel_get_elapsed_hstics(unsigned start_time)
{
    return kernel_get_hstics() - start_time;
}


