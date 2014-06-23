#include "msp430g2553.h"
#include "uart.h"

void uart_rx_isr(unsigned char c) {
	uart_putc(c);
	P1OUT ^= BIT0;		// toggle P1.0 (red led)
}

/**
 * Main routine
 */
int main(void)
{
    WDTCTL  = WDTPW + WDTHOLD; 	// Stop WDT
	BCSCTL1 = CALBC1_1MHZ;      // Set DCO
  	DCOCTL  = CALDCO_1MHZ;

	P1DIR  = BIT0 + BIT6; 		// P1.0 and P1.6 are the red+green LEDs
	P1OUT  = BIT0 + BIT6; 		// All LEDs off

    uart_init();

	// register ISR called when data was received
    uart_set_rx_isr_ptr(uart_rx_isr);

    __bis_SR_register(GIE);

    uart_puts((char *)"\n\r***************\n\r");
    uart_puts((char *)"MSP430 harduart\n\r");
    uart_puts((char *)"***************\n\r\n\r");

	uart_puts((char *)"PRESS any key to start echo example ... ");

	unsigned char c = uart_getc();

	uart_putc(c);
	uart_puts((char *)"\n\rOK\n\r");

    volatile unsigned long i;

    while(1) {

	   P1OUT ^= BIT6; 			// Toggle P1.6 output (green LED) using exclusive-OR
	   i = 50000;             	// Delay

	   do (i--);				// busy waiting (bad)
	   while (i != 0);
    }
}

