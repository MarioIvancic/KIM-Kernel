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

	// pins for uart0
    PINSEL0 = (PINSEL0 & ~U0_PINMASK) | U0_PINSEL;

    uart0_init(UART_BAUD(38400), UART_8N2, UART_FIFO_14); // setup the UART
    init_printf(0, tfp_putc);

    set_eh_output(uart0_putchar);

    tfp_printf("\r\n\r\n       Kernel version %s STARTING\r\n", str_version);

	// 50Hz tick rate
    // kernel_init(CCLK / 50);
    kernel_init(LPC_TIMER_CLK / 50);


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
//		LED_ON();
        strled = 0;
	}
	else
	{
//		LED_OFF();
		strled = 1;
	}
}

void default_isr(void) { ; }



void __aeabi_unwind_cpp_pr0(void){}
void __aeabi_unwind_cpp_pr1(void){}
void __aeabi_unwind_cpp_pr2(void){}

