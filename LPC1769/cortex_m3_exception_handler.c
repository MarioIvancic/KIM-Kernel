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

/*
2.3.7. Exception entry and return
Descriptions of exception handling use the following terms:

Preemption
    When the processor is executing an exception handler, an exception can preempt the exception
    handler if its priority is higher than the priority of the exception being handled.
    See Interrupt priority grouping for more information about preemption by an interrupt.

    When one exception preempts another, the exceptions are called nested exceptions.
    See Exception entry more information.

Return
    This occurs when the exception handler is completed, and:
        there is no pending exception with sufficient priority to be serviced
        the completed exception handler was not handling a late-arriving exception.
    The processor pops the stack and restores the processor state to the state it had before
    the interrupt occurred. See Exception return for more information.

Tail-chaining
    This mechanism speeds up exception servicing. On completion of an exception handler,
    if there is a pending exception that meets the requirements for exception entry,
    the stack pop is skipped and control transfers to the new exception handler.

Late-arriving
    This mechanism speeds up preemption. If a higher priority exception occurs during state
    saving for a previous exception, the processor switches to handle the higher priority
    exception and initiates the vector fetch for that exception. State saving is not affected
    by late arrival because the state saved is the same for both exceptions.
    Therefore the state saving continues uninterrupted. The processor can accept a late
    arriving exception until the first instruction of the exception handler of the original
    exception enters the execute stage of the processor. On return from the exception handler
    of the late-arriving exception, the normal tail-chaining rules apply.

Exception entry
Exception entry occurs when there is a pending exception with sufficient priority and either:
    the processor is in Thread mode
    the new exception is of higher priority than the exception being handled,
    in which case the new exception preempts the exception being handled.

When one exception preempts another, the exceptions are nested.

Sufficient priority means the exception has has greater priority than any limits set by the
mask registers, see Exception mask registers. An exception with less priority than this is
pending but is not handled by the processor.

When the processor takes an exception, unless the exception is a tail-chained or a late-arriving
exception, the processor pushes information onto the current stack. This operation is referred to
as stacking and the structure of eight data words is referred as the stack frame. The stack frame
contains the following information:
            xxxx    <-- SP before exception
SP + 0x1c   xPSR
SP + 0x18   PC
SP + 0x14   LR
SP + 0x10   R12
SP + 0x0c   R3
SP + 0x08   R2
SP + 0x04   R1
SP + 0x00   R0      <-- SP in exception handler

Immediately after stacking, the stack pointer indicates the lowest address in the stack frame.
The alignment of the stack frame is controlled via the STKALIGN bit of the Configuration Control
Register (CCR).

The stack frame includes the return address. This is the address of the next instruction in the
interrupted program. This value is restored to the PC at exception return so that the interrupted
program resumes.

The processor performs a vector fetch that reads the exception handler start address from the
vector table. When stacking is complete, the processor starts executing the exception handler.
At the same time, the processor writes an EXC_RETURN value to the LR. This indicates which stack
pointer corresponds to the stack frame and what operation mode the processor was in before
the entry occurred.

If no higher priority exception occurs during exception entry, the processor starts executing
the exception handler and automatically changes the status of the corresponding pending interrupt
to active.

If another higher priority exception occurs during exception entry, the processor starts executing
the exception handler for this exception and does not change the pending status of the earlier
exception. This is the late arrival case.

Exception return
Exception return occurs when the processor is in Handler mode and executes one of the
following instructions attempts to set the PC to an EXC_RETURN value:
    an LDM or POP instruction that loads the PC
    an LDR instruction with PC as the destination
    a BX instruction using any register.

The processor saves an EXC_RETURN value to the LR on exception entry. The exception mechanism
relies on this value to detect when the processor has completed an exception handler.
Bits[31:4] of an EXC_RETURN value are 0xFFFFFFF. When the processor loads a value matching this
pattern to the PC it detects that the operation is a not a normal branch operation and, instead,
that the exception is complete. Therefore, it starts the exception return sequence.
Bits[3:0] of the EXC_RETURN value indicate the required return stack and processor mode,
as Table 2.17 shows.

Table 2.17. Exception return behavior
EXC_RETURN	Description
0xFFFFFFF1  Return to Handler mode.
            Exception return gets state from the main stack.
            Execution uses MSP after return.

0xFFFFFFF9  Return to Thread mode.
            Exception Return get state from the main stack.
            Execution uses MSP after return.

0xFFFFFFFD  Return to Thread mode.
            Exception return gets state from the process stack.
            Execution uses PSP after return.

All other values	Reserved.

#################################################################################################

From forum http://embdev.net/topic/170640#1636052

> so I believe that would be a Bus Fault.

Look at the fault status registers.

> I've found there is a BFAR register I could use, or by looking at the
> stack by way of a register, PSP or MSP, although I don't know which
> one would be valid, or what the disadvantages of each are.

What information are you looking for on the stack? The return address?
You will want to find out which stack pointer was used before entering
the exception (information in link register). Then look for the return
address at the appropriate offset.


Hi Jerry,
have the code converted to GNU (Codesourcery Toolchain).

The assembler code:

.type _hard_fault_handler_asm, %function

_hard_fault_handler_asm:
  TST LR, #4
  ITE EQ
  MRSEQ R0, MSP
  MRSNE R0, PSP
  B hard_fault_handler_c

The C code (modified a little):

// hard fault handler in C,
// with stack frame location as input parameter
void hard_fault_handler_c(unsigned int * hardfault_args)
{
unsigned int stacked_r0;
unsigned int stacked_r1;
unsigned int stacked_r2;
unsigned int stacked_r3;
unsigned int stacked_r12;
unsigned int stacked_lr;
unsigned int stacked_pc;
unsigned int stacked_psr;
unsigned int rBFAR;
unsigned int rCFSR;
unsigned int rHFSR;
unsigned int rDFSR;
unsigned int rAFSR;

stacked_r0 = ((unsigned long) hardfault_args[0]);
stacked_r1 = ((unsigned long) hardfault_args[1]);
stacked_r2 = ((unsigned long) hardfault_args[2]);
stacked_r3 = ((unsigned long) hardfault_args[3]);

stacked_r12 = ((unsigned long) hardfault_args[4]);
stacked_lr = ((unsigned long) hardfault_args[5]);
stacked_pc = ((unsigned long) hardfault_args[6]);
stacked_psr = ((unsigned long) hardfault_args[7]);

rBFAR = (*((volatile unsigned long *)(0xE000ED38)));
rCFSR = (*((volatile unsigned long *)(0xE000ED28)));
rHFSR = (*((volatile unsigned long *)(0xE000ED2C)));
rDFSR = (*((volatile unsigned long *)(0xE000ED30)));
rAFSR = (*((volatile unsigned long *)(0xE000ED3C)));

printf ("[Hard fault handler]\n");
printf ("R0 = %x\n", stacked_r0);
printf ("R1 = %x\n", stacked_r1);
printf ("R2 = %x\n", stacked_r2);
printf ("R3 = %x\n", stacked_r3);
printf ("R12 = %x\n", stacked_r12);
printf ("LR = %x\n", stacked_lr);
printf ("PC = %x\n", stacked_pc);
printf ("PSR = %x\n", stacked_psr);
printf ("BFAR = %x\n", rBFAR);
printf ("CFSR = %x\n", rCFSR);
printf ("HFSR = %x\n", rHFSR);
printf ("DFSR = %x\n", rDFSR);
printf ("AFSR = %x\n", rAFSR);
while(1);

return;
}
*/


#ifndef BIT
    #define BIT(n) (1UL << (n))
#endif

//extern int uart0_putchar(int);
//#define abt_putchar(a) uart0_putchar((a))

// pointer to print char function
static int (*abt_putchar)(int);
typedef int (*int_int_fcn)(int);


// enable div-by-0 and unaligned fault
void enable_div0_and_unaligned_fault(void)
{
    // SCB->CCR |= 0x18;
    *((volatile unsigned long *)(0xE000ED14)) |= 0x18;
}

void set_eh_output(void* fcn)
{
    abt_putchar = (int_int_fcn) fcn;
}


static void abt_puts(const char* s)
{
    if(!abt_putchar) return;
	while(*s)
	{
		abt_putchar(*s);
		s++;
	}
}


static void abt_hex(unsigned long x)
{
	static const char hc[] = {"0123456789ABCDEF"};
	static char hb[12];
	int i;
	hb[0] = '0';
	hb[1] = 'x';
	for(i = 9; i > 1; i--, x >>= 4)
	{
		hb[i] = hc[x & 0x0f];
	}
	hb[10] = 0;
	abt_puts(hb);
}


static inline unsigned int get_basepri(void)
{
    unsigned int t;
    asm volatile
    (
        "\n\t"
        "MRS %0, basepri\n\t"
        : "=r" (t)
    );
    return t;
}



// hard fault handler in C,
// with stack frame location as input parameter
void fault_exception_handler_c(unsigned int * hardfault_args)
{
    unsigned int stacked_r0;
    unsigned int stacked_r1;
    unsigned int stacked_r2;
    unsigned int stacked_r3;
    unsigned int stacked_r4;
    unsigned int stacked_r5;
    unsigned int stacked_r6;
    unsigned int stacked_r7;
    unsigned int stacked_r8;
    unsigned int stacked_r9;
    unsigned int stacked_r10;
    unsigned int stacked_r11;
    unsigned int stacked_r12;
    unsigned int stacked_lr;
    unsigned int stacked_pc;
    unsigned int stacked_psr;
    unsigned int rBFAR;
    unsigned int rMMFAR;
    unsigned int rCFSR;
    unsigned int rHFSR;
    unsigned int rDFSR;
    unsigned int rAFSR;
    unsigned int rCCR;
    unsigned int rSHP;
    unsigned int rSHCSR;
    unsigned int UsageFault_prio;
    unsigned int BusFault_prio;
    unsigned int MemMenageFault_prio;
    unsigned int SVCall_prio;
    unsigned int SysTick_prio;
    unsigned int PendSV_prio;
    unsigned int basepri;
    unsigned int i, sp, *psp;

    sp = ((unsigned int) hardfault_args) - 8;
    psp = (unsigned int*) sp;

    stacked_r0 = ((unsigned long) hardfault_args[0]);
    stacked_r1 = ((unsigned long) hardfault_args[1]);
    stacked_r2 = ((unsigned long) hardfault_args[2]);
    stacked_r3 = ((unsigned long) hardfault_args[3]);

    stacked_r12 = ((unsigned long) hardfault_args[4]);
    stacked_lr = ((unsigned long) hardfault_args[5]);
    stacked_pc = ((unsigned long) hardfault_args[6]);
    stacked_psr = ((unsigned long) hardfault_args[7]);

    stacked_r11 = ((unsigned long) hardfault_args[-1]);
    stacked_r10 = ((unsigned long) hardfault_args[-2]);
    stacked_r9 = ((unsigned long) hardfault_args[-3]);
    stacked_r8 = ((unsigned long) hardfault_args[-4]);
    stacked_r7 = ((unsigned long) hardfault_args[-5]);
    stacked_r6 = ((unsigned long) hardfault_args[-6]);
    stacked_r5 = ((unsigned long) hardfault_args[-7]);
    stacked_r4 = ((unsigned long) hardfault_args[-8]);

    rBFAR = (*((volatile unsigned long *)(0xE000ED38)));
    rCFSR = (*((volatile unsigned long *)(0xE000ED28)));
    rHFSR = (*((volatile unsigned long *)(0xE000ED2C)));
    rDFSR = (*((volatile unsigned long *)(0xE000ED30)));
    rAFSR = (*((volatile unsigned long *)(0xE000ED3C)));
    rCCR  = (*((volatile unsigned long *)(0xE000ED14)));
    rSHP  = (*((volatile unsigned long *)(0xE000ED18)));
    rSHCSR  = (*((volatile unsigned long *)(0xE000ED24)));
    rMMFAR  = (*((volatile unsigned long *)(0xE000ED34)));

    UsageFault_prio  = ((*((volatile unsigned long *)(0xE000ED18))) >> 16) & 255;
    BusFault_prio  = ((*((volatile unsigned long *)(0xE000ED18))) >> 8) & 255;
    MemMenageFault_prio  = ((*((volatile unsigned long *)(0xE000ED18))) >> 0) & 255;
    SVCall_prio  = ((*((volatile unsigned long *)(0xE000ED1c))) >> 24) & 255;
    SysTick_prio  = ((*((volatile unsigned long *)(0xE000ED20))) >> 24) & 255;
    PendSV_prio  = ((*((volatile unsigned long *)(0xE000ED20))) >> 16) & 255;
    basepri = get_basepri();

    abt_puts("\r\n");
    if(rSHCSR & BIT(0))  abt_puts("Memory Manager Exception\r\n");
    if(rSHCSR & BIT(1))  abt_puts("Bus Fault Exception\r\n");
    if(rSHCSR & BIT(3))  abt_puts("Usage Fault Exception\r\n");
    if(rSHCSR & BIT(7))  abt_puts("SVC Call Exception\r\n");
    if(rSHCSR & BIT(8))  abt_puts("Monitor Exception\r\n");
    if(rSHCSR & BIT(10)) abt_puts("PendSvcAct Exception\r\n");
    if(rSHCSR & BIT(11)) abt_puts("SysTick Exception\r\n");
    if(rSHCSR & BIT(12)) abt_puts("Usage Fault Exception Pending\r\n");
    if(rSHCSR & BIT(13)) abt_puts("Memory Fault Exception Pending\r\n");
    if(rSHCSR & BIT(14)) abt_puts("Bus Fault Exception Pending\r\n");
    if(rSHCSR & BIT(15)) abt_puts("SVC Call Exception Pending\r\n");

    if(rHFSR & BIT(1))   abt_puts("Hard Fault Exception on vector table read.\r\n");
    if(rHFSR & BIT(30))  abt_puts("Forced Exception\r\n");

    // MMFSR bits in CFSR
    if(rCFSR & BIT(0))
    {
        abt_puts("Instruction fetch from "); abt_hex(stacked_pc); abt_puts("\r\n");
    }
    if(rCFSR & BIT(1))
    {
        abt_puts("Data Load/Store from/to "); abt_hex(rMMFAR); abt_puts("\r\n");
        abt_puts("Caused by instruction @ "); abt_hex(stacked_pc); abt_puts("\r\n");
    }
    if(rCFSR & BIT(3))
    {
        abt_puts("Memory Fault on unstacking for a return from exception\r\n");
        abt_puts("Caused by corrupted SP or stack changed during exception\r\n");
    }
    if(rCFSR & BIT(4))
    {
        abt_puts("Memory Fault on stacking for exception entry\r\n");
        abt_puts("Caused by corrupted or not initialized SP, or\r\n");
        abt_puts("Stack is reaching a region not defined by the MPU as r/w memory.\r\n");
    }

    // BFSR bits in CFSR
    if(rCFSR & BIT(8))
    {
        abt_puts("Instruction bus error caused by:\r\n");
        abt_puts(" Branch/Call to invalid memory regions\r\n");
        abt_puts(" Invalid return due to corrupted SP or stack content\r\n");
        abt_puts(" Incorrect entry in the exception vector table.\r\n");
    }
    if(rCFSR & BIT(9))
    {
        abt_puts("Data Bus Error @ "); abt_hex(rBFAR); abt_puts("\r\n");
        abt_puts("Caused by instruction @ "); abt_hex(stacked_pc); abt_puts("\r\n");
    }
    if(rCFSR == BIT(10))
    {
        abt_puts("Imprecise data bus error\r\n");
    }
    if(rCFSR & BIT(11))
    {
        abt_puts("Bus Fault on unstacking for a return from exception\r\n");
    }
    if(rCFSR & BIT(12))
    {
        abt_puts("Bus Fault on stacking for exception entry caused by:\r\n");
        abt_puts(" Stack pointer is corrupted or not initialized\r\n");
        abt_puts(" Stack is reaching an undefined memory region.\r\n");
    }

    // UFSR bits in CFSR
    if(rCFSR & BIT(16))
    {
        abt_puts("Undefined instruction @ "); abt_hex(stacked_pc); abt_puts("\r\n");
    }
    if(rCFSR & BIT(17))
    {
        abt_puts("Invalid CPU state @ "); abt_hex(stacked_pc); abt_puts("\r\n");
        abt_puts("Potential reasons:\r\n");
        abt_puts("Loading a branch target address to PC with LSB=0.\r\n");
        abt_puts("Stacked PSR corrupted during exception or interrupt handling.\r\n");
        abt_puts("Vector table contains a vector address with LSB=0.\r\n");
    }
    if(rCFSR & BIT(18))
    {
        abt_puts("Invalid PC load @ "); abt_hex(stacked_pc); abt_puts("\r\n");
        abt_puts("Load of an illegal EXC_RETURN value to the PC. Potential reasons:\r\n");
        abt_puts("Invalid return due to corrupted SP, LR, or stack content.\r\n");
        abt_puts("ICI/IT bit in PSR invalid for an instruction.\r\n");
    }
    if(rCFSR & BIT(19))
    {
        abt_puts("No coprocessor Usage Fault.\r\n");
    }
    if(rCFSR & BIT(24))
    {
        abt_puts("Unaligned Memory Access Usage Fault.\r\n");
    }
    if(rCFSR & BIT(25))
    {
        abt_puts("DIV by 0 Usage Fault.\r\n");
    }



	//if(ctx.dabort) { abt_puts("  [pc]="); abt_hex(ctx.inst); } abt_puts("\r\n");

	abt_puts("Dumping Registers [r12=ip; r13=sp; r14=lr; r15=pc;]\r\n");

	abt_puts("r0 ="); abt_hex(stacked_r0); abt_puts("  r1 ="); abt_hex(stacked_r1);
	abt_puts("  r2 ="); abt_hex(stacked_r2); abt_puts("  r3 ="); abt_hex(stacked_r3); abt_puts("\r\n");

	abt_puts("r4 ="); abt_hex(stacked_r4); abt_puts("  r5 ="); abt_hex(stacked_r5);
	abt_puts("  r6 ="); abt_hex(stacked_r6); abt_puts("  r7 ="); abt_hex(stacked_r7); abt_puts("\r\n");

	abt_puts("r8 ="); abt_hex(stacked_r8); abt_puts("  r9 ="); abt_hex(stacked_r9);
	abt_puts("  r10="); abt_hex(stacked_r10); abt_puts("  r11="); abt_hex(stacked_r11); abt_puts("\r\n");

	abt_puts("r12="); abt_hex(stacked_r12); abt_puts("  lr ="); abt_hex(stacked_lr);
	abt_puts("  pc ="); abt_hex(stacked_pc); abt_puts("  sp ="); abt_hex(sp); abt_puts("\r\n");

	abt_puts("psr="); abt_hex(stacked_psr); abt_puts("\r\n");

	if(rCFSR & BIT(15)) { abt_puts("BFAR ="); abt_hex(rBFAR); abt_puts("\r\n"); }
	if(rCFSR & BIT(7))  { abt_puts("MMFAR="); abt_hex(rMMFAR); abt_puts("\r\n"); }

	abt_puts("CFSR="); abt_hex(rCFSR);	abt_puts("  HFSR="); abt_hex(rHFSR); //  abt_puts("\r\n");
	abt_puts("  DFSR="); abt_hex(rDFSR);	abt_puts("  AFSR="); abt_hex(rAFSR);  abt_puts("\r\n");
	abt_puts("SHCSR="); abt_hex(rSHCSR);   abt_puts("\r\n");
	abt_puts("UsageFault_prio="); abt_hex(UsageFault_prio);
	abt_puts("  BusFault_prio="); abt_hex(BusFault_prio);
	abt_puts("  MemMenageFault_prio="); abt_hex(MemMenageFault_prio);
	abt_puts("\r\n");
	abt_puts("SVCall_prio="); abt_hex(SVCall_prio);
	abt_puts("  SysTick_prio="); abt_hex(SysTick_prio);
	abt_puts("  PendSV_prio="); abt_hex(PendSV_prio);
	abt_puts("\r\n");
	abt_puts("BASEPRI="); abt_hex(basepri);
	abt_puts("\r\n");

	//abt_puts("Legend: r0  r1  r2  r3  r4  r5  r6  r7  r8  r9  r10 r11 r12 r13 r14 r15\r\n");
	//abt_puts("                                            sb  sl  fp  ip  sp  lr  pc\r\n");
	//abt_puts("        a1  a2  a3  a4  v1  v2  v3  v4  v5  v6  v7  v8\r\n");


	abt_puts("Dumping stack:\r\n");

#define PRINT_STACK_LINE() abt_hex((unsigned int)psp); abt_puts(":");	i = 0; \
    do{ abt_puts(" "); abt_hex(*psp); i++; psp++; } \
    while(i < 4); abt_puts("\r\n")

	PRINT_STACK_LINE();
	PRINT_STACK_LINE();
	PRINT_STACK_LINE();
	PRINT_STACK_LINE();
	PRINT_STACK_LINE();
	PRINT_STACK_LINE();
	PRINT_STACK_LINE();
	PRINT_STACK_LINE();

#undef PRINT_STACK_LINE

	abt_puts("\r\n");
	abt_puts("Program halted\r\n");

    while(1);

    return;
}
