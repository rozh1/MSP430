#include <msp430g2452.h>
#include <stdint.h>
#include <stdio.h>

#include "uart.h"
#define A4  BIT4
#define dv  0.0035//2258
#define amp 17.5	//mA
#define VOLT_MUL 2.98

/*  Глобальные переменные  */
double seconds=0;
float volts=0.0, capacity=0, work_time=0;
unsigned int ipart,fpart;
char out[30];
unsigned int time=0;

/*  Обьявление функций  */
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
        ADC10CTL0 |= ADC10SC;   // начинаем новое преобразование
        while ((ADC10CTL1 & ADC10BUSY) == 0x01); // ждем, когда преобразование закончится
        volts=ADC10MEM*dv*VOLT_MUL;  // конвертируем результат в напряжение и сохраняем
        update_display();   // выводим в uart
    }

} // main

void ADC_init(void) {
            // Используем Vcc/Vss(аналоговая земля) для верхнего/нижнего ИОН,
            // 16 x ADC10CLKs (выборка за 16 тактов), включаем АЦП.
    ADC10CTL0 = SREF_0 + ADC10SHT_2 + ADC10ON;
            // Вход A1, делитель ADC10CLK на 1, одноканальный режим.
    ADC10CTL1 =  INCH_4 + SHS_0 + ADC10SSEL_0 + ADC10DIV_0 + CONSEQ_0;
    ADC10AE0 = A4;      // Разрешаем вход АЦП на порту P1.4

    ADC10CTL0 |= ENC;     // Разрешаем преобразования.
} // ADC_init

void update_display(void) {
    ipart=(int)volts;   // целая часть
    fpart=(int)((volts-ipart)*100);   // дробная часть, 2 цифры
    if (ipart==0)
    {
    	P1OUT = 0 | BIT0;
    	if (work_time==0) work_time=seconds/3600;
    	sprintf(out,"Capacity: %d mAh\n\r",(int)(work_time*amp));
    }
    else
    {
    	P1OUT = 0 | BIT6;
        sprintf(out,"%d.%2.d\n\r",ipart,fpart);
    }
    uart_puts(out);
	__delay_cycles(1000000);
	seconds++;
}

