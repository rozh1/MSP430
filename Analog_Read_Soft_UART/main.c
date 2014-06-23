#include <msp430g2452.h>
#include <stdint.h>
#include <stdio.h>

#include "uart.h"
#define A4  BIT4
#define dv  0.0035//2258

/*  ���������� ����������  */
float volts=0.0;
unsigned int ipart,fpart;
char out[10];

/*  ���������� �������  */
void ADC_init(void);
void update_display(void);

void main(void) {
    WDTCTL = WDTPW + WDTHOLD;

	BCSCTL1 = CALBC1_1MHZ;      // Set DCO
  	DCOCTL  = CALDCO_1MHZ;

	P1DIR  |= BIT0 + BIT6;
	P1OUT  |= BIT0 + BIT6;

	uart_init();
    __enable_interrupt();
    ADC_init();

    for (;;) {
        ADC10CTL0 |= ADC10SC;   // �������� ����� ��������������
        while ((ADC10CTL1 & ADC10BUSY) == 0x01); // ����, ����� �������������� ����������
        volts=ADC10MEM*dv;  // ������������ ��������� � ���������� � ���������
        update_display();   // ������� � uart
    }

} // main

void ADC_init(void) {
            // ���������� Vcc/Vss(���������� �����) ��� ��������/������� ���,
            // 16 x ADC10CLKs (������� �� 16 ������), �������� ���.
    ADC10CTL0 = SREF_0 + ADC10SHT_2 + ADC10ON;
            // ���� A1, �������� ADC10CLK �� 1, ������������� �����.
    ADC10CTL1 =  INCH_4 + SHS_0 + ADC10SSEL_0 + ADC10DIV_0 + CONSEQ_0;
    ADC10AE0 = A4;      // ��������� ���� ��� �� ����� P1.4

    ADC10CTL0 |= ENC;     // ��������� ��������������.
} // ADC_init

void update_display(void) {
    ipart=(int)volts;   // ����� �����
    if (ipart==0)
    {
    	P1OUT = 0 | BIT0;
    	//P1OUT = BIT1;
    }
    else
    {
    	P1OUT = 0 | BIT6;
    	//P1OUT = BIT6;
    }
    fpart=(int)((volts-ipart)*1000);   // ������� �����, 3 �����
    sprintf(out,"%d.%d\r",ipart,fpart);
    uart_puts(out);
	__delay_cycles(1000000);
}
