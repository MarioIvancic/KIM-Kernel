// uart0.h

#ifndef __UART0_H__
#define __UART0_H__

#include "lpc_uart_bit_defines.h"


void uart0_init(int baud, int mode, int fmode);

// get char or -1 if nothing available
int uart0_getchar(void);

// send char ch (blocking function)
int uart0_putchar(int ch);

// clear the TX fifo
void uart0_tx_flush(void);


#endif // __UART0_H__
