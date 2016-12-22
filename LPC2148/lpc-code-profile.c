// lpc-code-profile.c
// statistical code profiling
// based on code in "Listing A" by David Brenan July 11, 2002, 7:00am PDT
// based on code from html page titled "Profiling code on an embedded platform" 
// by Edwin Olson (eolson@mit.edu), April 2007 
// http://www.blisstonia.com/eolson/notes/profile_arm.php
/*
David Brenan
...
A simple code profiler divides the execution address space into small, equal regions and 
maintains a counter for each one. A regular timer interrupt then periodically samples the 
CPU instruction pointer to determine the address of the executing code on each timer tick 
and increments the corresponding counter. Over time, this builds a statistical profile of 
where the CPU is spending its time.

Implementation details
Profile counter table—You’ll need a block of RAM to store the table of profiling counters. 
Each counter corresponds to a small region of the executable code in memory, say 32 bytes. 
Ideally, you should use 32-bit counters to avoid counter overflow issues. For example, a 
system with 64 KB of executable code divided into 2,048 regions of 32 bytes each would 
therefore require 2,048 regions of 4 bytes each, which equals 8 KB of spare RAM.
...
*/

#include <stdio.h>

/* Define the extent of the executable code and the profiling regions */
#define CODE_BASE_ADDRESS   0L           
#define CODE_SIZE          (64L*1024L)  
#define REGION_SIZE  32
#define NREGIONS           (CODE_SIZE/REGION_SIZE)

/* Array of profile counters */
unsigned long ProfileCounters[NREGIONS];   

/* Flag to control the capture of profiling information */
volatile unsigned ProfileEnabled = 1;

/* Psuedo-code for timer interrupt (processor-dependent, typically assembler)
asm
{
    Interrupt entry code
    Get saved instruction pointer (ip)

    if (ProfileEnabled)
    {
        ++ProfileCounters[(ip - CODE_BASE_ADDRESS)/REGION_SIZE];
    }
    Other duties here
    Interrupt exit code
}
 */

/* Dump results to debug terminal. You will probably want to filter the results to 
exclude regions with only a few hits and to sort the table into number-of-hits order. */
void ProfileDump(void)
{
    unsigned long i, addr;
    ProfileEnabled = 0;

    for (i = 0, addr = CODE_BASE_ADDRESS; i < NREGIONS; i++, addr += REGION_SIZE)
    {
        printf("Code region %lX..%lX had %lu hits ", addr, addr + (REGION_SIZE-1), ProfileCounters[i]);
    }
}

//--------------------------------------------------------------------------------------------------

/*
Edwin Olson
...
Approach

-   Write a FIQ (fast interrupt) that will periodically sample the PC (program counter), 
    and write the sampled address to a buffer.
-   Write a user task that periodically polls the sample buffer and sends it to a host. 
    (I send the data via UDP packets, but serial would have worked too.)
-   Process the samples on the local host to generate useful profiling data. 

The implementation of #1 and #2 are strongly platform and embedded-O/S dependent, but they're 
also fairly easy to do! Below is my FIQ, for example, which uses the LPC timer 0 to generate an 
interrupt every 100k cycles (or there-abouts). Not shown, of course, is the modification to 
the reset vectors to point to on_profile_irq, and the normal O/S task which polls profile_out 
and sends the data to the host.
...
*/


#define PROFILE_SAMPLES 256
uint32_t profile_buffer[PROFILE_SAMPLES];
volatile uint32_t profile_out;

void profile_init()
{
    PCONP |= (1<<1); // PCTIM0
    
    T0PR   = 1;               // prescale divider
    T0TCR  = (1<<0);          // enable the counter
    T0MR0  = 9000;            // match register
    T0MCR  = (1<<0) | (1<<1); // when we hit MR0, throw an IRQ and reset.
    T0CTCR = 0;               // counter mode, por favor

    VICIntSelect |= (1<<4);   // make MR0 match a FIQ
    VICIntEnable |= (1<<4);
}

void on_profile_irq() __attribute__ ((interrupt ("FIQ")));
void on_profile_irq()
{
    uint32_t addr;
    asm volatile("mov %r0, lr \r\n"
                 : "=r"(addr) );

    if (profile_out < PROFILE_SAMPLES) profile_buffer[profile_out++] = addr;
    T0IR |= (1<<0);         // ack the interrupt
}
 
