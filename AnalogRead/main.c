#include <msp430g2553.h>
#include "uart.h"
#include <stdio.h>

#define A4  BIT4
#define dv  0.0032258

/*  Глобальные переменные  */
float volts=0.0;
unsigned int ipart,fpart;
char out[10];

/*  Обьявление функций  */
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
        ADC10CTL0 |= ADC10SC;   // начинаем новое преобразование
        while ((ADC10CTL1 & ADC10BUSY) == 0x01); // ждем, когда преобразование закончится
        volts=ADC10MEM*dv;  // конвертируем результат в напряжение и сохраняем
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
    fpart=(int)((volts-ipart)*1000);   // дробная часть, 3 цифры
    sprintf(out,"%d.%d\r",ipart,fpart);
    uart_puts(out);
	__delay_cycles(1000000);
}
