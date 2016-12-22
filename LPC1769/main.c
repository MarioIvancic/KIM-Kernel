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

// main.c
// kernel test app

#include <string.h>
#include "appconfig.h"
#include "lpc_uart_magic.h"
#include "sys_init.h"
#include "uart0.h"
#include "printf.h"


#include "kernel.h"


// 500ms for tick rate of 50Hz
#define TOGGLE_LED_TIME 10


void toggle_led(void);

// macros for stringification
#define xstr(s) str(s)
#define str(s) #s

// version and build timestamp
const char str_version[32] = "v0.1.0";
const char str_build_timestamp[20] = xstr(BUILD_TIMESTAMP);
// "2014-01-01_00:00:00" --- 19 + 1 chars

#undef xstr
#undef str


// exception handling
extern void set_eh_output(void* fcn);
extern void enable_div0_and_unaligned_fault(void);


void * task1(void* p)
{
    uatomic_t led_time = kernel_get_ticks();

    while(1)
    {
        uatomic_t now;
        while(((now = kernel_get_ticks()) - led_time) <= TOGGLE_LED_TIME);
        led_time = now;
        toggle_led();
        while(((now = kernel_get_ticks()) - led_time) <= TOGGLE_LED_TIME);
        led_time = now;
        toggle_led();
        while(((now = kernel_get_ticks()) - led_time) <= TOGGLE_LED_TIME);
        led_time = now;
        toggle_led();
        while(((now = kernel_get_ticks()) - led_time) <= TOGGLE_LED_TIME);
        led_time = now;
        toggle_led();

		while(((now = kernel_get_ticks()) - led_time) <= 4 * TOGGLE_LED_TIME);
        led_time = now;
        toggle_led();
        while(((now = kernel_get_ticks()) - led_time) <= 4 * TOGGLE_LED_TIME);
        led_time = now;
        toggle_led();
    }
    return 0;
}

void * task2(void* p)
{
    (void)p;
    tfp_printf("Calling task_usleep() in task2\r\n");
    task_usleep(4);
    tfp_printf("Calling task_exit() in task2\r\n");
    return 0;   // implicitno se poziva task_exit
}


void * task3(void* p)
{
    (void)p;
    while(1)
    {
        tfp_printf("task_yield() in task3\r\n");
        task_yield();
    }
    return 0;
}

void * task4(void* p)
{
    (void)p;
    while(1)
    {
        tfp_printf("task_yield() in task4\r\n");
        task_yield();
    }
    return 0;
}

TCB_t task1_tcb;
TCB_t task2_tcb;
TCB_t task3_tcb;
TCB_t task4_tcb;
#define TASK_1_SS 1024
#define TASK_2_SS 2 * 1024
#define TASK_3_SS 2 * 1024
#define TASK_4_SS 1024
char task1_stack[TASK_1_SS];
char task2_stack[TASK_2_SS];
char task3_stack[TASK_3_SS];
char task4_stack[TASK_4_SS];


// tfp_printf character output function
void tfp_putc ( void* p, char c)
{
    uart0_putchar(c);
}

int main(void)
{
    sys_init();

    //  wdtInit();			// initialize the watchdog timer

#if defined(LPC2148)
    vic_init(default_isr);
#endif


#if defined(LPC2148)
    PINSEL0 = (PINSEL0 & ~U0_PINMASK) | U0_PINSEL;
#elif defined(LPC1768) || defined(LPC1769)
    // UART0, TX=P0.2, RX=P0.3

    #define UART0_PINSEL_MASK ((3 << 4) | (3 << 6))
    #define UART0_PINSEL_VAL  ((1 << 4) | (1 << 6))

    PINSEL0 = (PINSEL0 & ~(UART0_PINSEL_MASK)) | UART0_PINSEL_VAL;
#endif


    uart0_init(UART_BAUD(38400), UART_8N2, UART_FIFO_14); // setup the UART
    init_printf(0, tfp_putc);

#if defined(LPC1768) || defined(LPC1769)
    set_eh_output(uart0_putchar);
    enable_div0_and_unaligned_fault();
#endif

    tfp_printf("\r\n\r\n       Kernel version %s STARTING\r\n", str_version);

    // kernel_init(CCLK / 1000);       // 1000Hz tick rate
    kernel_init(CCLK / 50);         // 50Hz tick rate


    task_create(&task1_tcb, task1, &task1_tcb, task1_stack, TASK_1_SS, 0, 0);
    task_create(&task2_tcb, task2, &task2_tcb, task2_stack, TASK_2_SS, 1, 0);
    //task_create(&task3_tcb, task3, &task3_tcb, task3_stack, TASK_3_SS, 1, 0);
    //task_create(&task4_tcb, task4, &task4_tcb, task4_stack, TASK_4_SS, 1, 0);


    tfp_printf("Calling kernel_start()\r\n");
    kernel_start();
    
    // we must never break this loop
    while(1)
    {
    	task_yield();
    }

	return 0;
}



void toggle_led(void)
{
	// LEDSTROBE
	static int strled;

	if(strled)
	{
		LED_ON();
        strled = 0;
	}
	else
	{
		LED_OFF();
		strled = 1;
	}
}

void default_isr(void) { ; }



void __aeabi_unwind_cpp_pr0(void){}
void __aeabi_unwind_cpp_pr1(void){}
void __aeabi_unwind_cpp_pr2(void){}

