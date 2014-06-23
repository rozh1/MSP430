#include <msp430g2553.h>
#include "uart.h"
#include <stdio.h>

#define A4  BIT4
#define dv  0.0032258

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

	P1DIR  = BIT0 + BIT6; 		// P1.0 and P1.6 are the red+green LEDs
	P1OUT  = BIT0 + BIT6; 		// All LEDs off
	uart_init();
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
    fpart=(int)((volts-ipart)*1000);   // ������� �����, 3 �����
    sprintf(out,"%d.%d\r",ipart,fpart);
    uart_puts(out);
	__delay_cycles(1000000);
}
