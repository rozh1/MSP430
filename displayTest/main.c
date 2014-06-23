#include  "msp430g2452.h"
#include "stdlib.h"
#include "stdio.h"
#include "lcm.h"

#define btn1 BIT3
#define UltraTrigPin BIT5
#define UltraEcho BIT1

int mode = 0;
int need_update = 1;
unsigned int measure = 0;
unsigned int up = 0;

unsigned int measure_1 = 0;
unsigned int measure_2 = 0;

void main(void) {

	WDTCTL = WDTPW + WDTHOLD; // Stop watchdog timer

	P1OUT = 0;
	P1REN |= BIT3; //разрешаем подтяжку
	P1OUT |= BIT3; //подтяжка вывода P1.3 вверх

	P1IES |= btn1; // прерывание по переходу из 1 в 0
	P1IFG &= ~btn1; // Для предотвращения немедленного срабатывания прерывания,
					// обнуляем его флаг для P1.3 до разрешения прерываний
	P1IE |= btn1; // Разрешаем прерывания для P1.3

	//P1OUT &= ~UltraTrigPin;
	P1DIR = UltraTrigPin;
	P1SEL = UltraEcho;

	TA0CCTL0 |= CM_3 + SCS + CAP + CCIE+CCIS_0; //capture rising and falling edge, syncronize(always do this),capture mode, interrupt enabled
	TACTL = TASSEL_2 + ID_0 + MC_2; // SMCLK,/1, continuous up mode, clear

	InitializeLcm();

	_enable_interrupt();

	while (1) {

		if (need_update > 0)
			ClearLcmScreen();
		switch (mode) {
		case 2:

			up = 1;
			P1OUT |= UltraTrigPin;
			__delay_cycles(10000);
			P1OUT &= ~UltraTrigPin;

			if (need_update > 0) {
				PrintStr("Ultrasonic");
				LcmSetCursorPosition(2, 0);
				PrintStr("distance: ");
				need_update--;
			}
			char buf[10];
			sprintf(buf, "%d            ", measure);
			LcmSetCursorPosition(2, 10);
			PrintStr(buf);
			__delay_cycles(500000);
			break;
		case 1:
			if (need_update > 0) {
				PrintStr("Press button!");
				need_update--;
			}
			break;
		default:
			if (need_update > 0) {
				PrintStr("Hello World!");
				LcmSetCursorPosition(2, 0);
				PrintStr("MSP430g2452 LCD!");
				need_update--;
			}
			break;
		}

		__delay_cycles(1000);
	}
}

#pragma vector = PORT1_VECTOR
__interrupt void P1_ISR(void) {
	switch (P1IFG & btn1) {
	case btn1:
		mode++;
		if (mode > 2)
			mode = 0;
		need_update++;
		P1IFG &= ~btn1;
		return;
	default:
		P1IFG = 0;
		return;
	}
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void timerA0() {
	if (up) {
		measure_1 = TACCR0;
	} else {
		measure_2 = TACCR0 - measure_1;
		measure = measure_2 / 58;
	}
	up = !up; //if this was the rising edge, the next one will be a falling edge, and vice-versa
	TA0CTL &= ~TAIFG; //clear timer A interrupt flag, so the chip knows we handled the interrupt
	TACCTL0 &= ~CCIFG; //clears capture interrupt flag, should already be done automatically though TODO
}
