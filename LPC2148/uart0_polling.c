// uart0_polling.c
// uart0 polling functions for LPC13xx LPC17xx LPC21xx

#include "lpcmcu.h"
#include "uart0.h"

// get char or -1 if nothing available
int uart0_getchar(void)
{
	if(U0LSR & ULSR_RDR) return U0RBR;    // check if character is available
	return -1;
}

// send char ch (blocking function)
int uart0_putchar(int ch)
{
	while(!(U0LSR & ULSR_THRE));          // wait for TX buffer to empty
	U0THR = (unsigned char)ch;
	return ch;
}

void uart0_tx_flush(void)
{
	U0FCR |= UFCR_TX_FIFO_RESET;          // clear the TX fifo
}


// init UART0 in polling mode
void uart0_init(int baud, int mode, int fmode)
{
	// set port pins for UART0
	// PINSEL0 = (PINSEL0 & ~U0_PINMASK) | U0_PINSEL;

#if defined(LPC213x) || defined(LPC214x) || defined(LPC17xx)
	PCONP = (PCONP & PCONP_MASK) | PCONP_UART0;
#elif defined(LPC13xx)
    LPC_SYSCON->SYSAHBCLKCTRL = (LPC_SYSCON->SYSAHBCLKCTRL & SYSAHBCLK_MASK) | SYSAHBCLK_UART;
#else
    #error POWER CONTROL not implemented!!
#endif

	U0IER = 0x00;                         // disable all interrupts
	U0IIR;                                // clear interrupt ID
	U0RBR;                                // clear receive register
	U0LSR;                                // clear line status register

	// set the baudrate
	U0LCR = ULCR_DLAB_ENABLE;             // select divisor latches
	U0DLL = (unsigned char)baud;          // set for baud low byte
	U0DLM = (unsigned char)(baud >> 8);   // set for baud high byte

	// set the number of characters and other
	// user specified operating parameters
	U0LCR = (mode & ~ULCR_DLAB_ENABLE);
	U0FCR = fmode;
}
