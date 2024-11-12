

#include <lpc214x.h>
#include <stdint.h>
#include <stdio.h>
#include "lcd.h"
void delay_ms(uint16_t j)
{
    uint16_t x,i;
	for(i=0;i<j;i++)
	{
    for(x=0; x<6000; x++);    /* loop to generate 1 milisecond delay with Cclk = 60MHz */
	}
}

/*void UART0_init(void)
{
	PINSEL0 = PINSEL0 | 0x00000005;	 Enable UART0 Rx0 and Tx0 pins of UART0 */
	//U0LCR = 0x83;	/* DLAB = 1, 1 stop bit, 8-bit character length */
	//U0DLM = 0x00;	/* For baud rate of 9600 with Pclk = 15MHz */
	//U0DLL = 0x61;	/* We get these values of U0DLL and U0DLM from formula */
	//U0LCR = 0x03; /* DLAB = 0 */
//}

//void UART0_TxChar(char ch) /* A function to send a byte on UART0 */
//{
	//U0THR = ch;
//	while( (U0LSR & 0x40) == 0 );	/* Wait till THRE bit becomes 1 which tells that transmission is completed */
//}

//void UART0_SendString(char* str) /* A function to send string on UART0 */
//{
	//uint8_t i = 0;
	//while( str[i] != '\0' )
	//{
		//UART0_TxChar(str[i]);
		//i++;
	//}
//}

uint32_t ADC_READ1(void)
{
	uint32_t x_out;
	PINSEL1 = (PINSEL1 | 0x01000000); /* P0.28 as AD0.1 */
	AD0CR = 0x00200402; /* ADC1 operational, 10-bits, 11 clocks for conversion */
	AD0CR = AD0CR | (1<<24); /* Start Conversion */
	while ( !(AD0DR1 & 0x80000000) );
	x_out = AD0DR1;
	x_out = (x_out>>6);
	x_out = (x_out & 0x000003FF);	
	return x_out;
}

uint32_t ADC_READ2(void)
{
	uint32_t y_out;
	PINSEL1 = (PINSEL1 | 0x04000000); /* P0.29 as AD0.2 */
	AD0CR = 0x00200404; /* ADC2 operational, 10-bits, 11 clocks for conversion */
	AD0CR = AD0CR | (1<<24); /* Start Conversion */
	while ( !(AD0DR2 & 0x80000000) );
	y_out = AD0DR2;
	y_out = (y_out>>6);
	y_out = (y_out & 0x000003FF);	
	return y_out;
}

uint32_t ADC_READ3(void)
{
	uint32_t z_out;
	PINSEL1 = (PINSEL1 | 0x10000000); /* P0.30 as AD0.3 */
	AD0CR = 0x00200408; /* ADC3 operational, 10-bits, 11 clocks for conversion */
	AD0CR = AD0CR | (1<<24); /* Start Conversion */
	while ( !(AD0DR3 & 0x80000000) );
	z_out = AD0DR3;
	z_out = (z_out>>6);
	z_out = (z_out & 0x000003FF);	
	return z_out;
}

int main(void)
{
	char result[70];
	uint32_t x_out1, y_out1, z_out1;
	long x_val, y_val, z_val;
	IO1DIR = (1 << en) | (1 << rs) | (1 << d4) | (1 << d5) | (1 << d6) | (1 << d7); //lcd set
	//UART0_init();
	init();
	command(0x01);
	command(0x0C); // Display on, cursor off
	while(1)
	{
		x_out1 = ADC_READ1();
		delay_ms(10);
		y_out1 = ADC_READ2();
		delay_ms(10);
		z_out1 = ADC_READ3();
		delay_ms(10);
	/*	x_val = ( ( ( (x_out1 * 3.3) / 1024 ) - 1.6 ) / 0.33 );
		y_val = ( ( ( (y_out1 * 3.3) / 1024 ) - 1.6 ) / 0.33 );
		z_val = ( ( ( (z_out1 * 3.3) / 1024 ) - 1.6 ) / 0.33 );*/
		delay_ms(10);
		//sprintf(result,"x=%lfg, y=%lfg, z=%lfg",x_val,y_val,z_val);
		disp(0x80,"A:");
		delay_ms(10);
		write(0x82,((x_out1/1000)+0x30));
		delay_ms(10);
		write(0x83,(((x_out1%1000)/100)+0x30));
		delay_ms(10);
		write(0x84,((((x_out1%1000)/10)%10)+0x30));
		delay_ms(10);
		write(0x85,(((x_out1%1000)%10)+0x30));
		delay_ms(100);
		disp(0x87,"B:");
		delay_ms(10);
		write(0x89,((y_val/1000)+0x30));
		delay_ms(10);
		write(0x8A,(((y_val%1000)/100)+0x30));
		delay_ms(10);
		write(0x8B,((((y_val%1000)/10)%10)+0x30));
		delay_ms(10);
		write(0x8C,(((y_val%1000)%10)+0x30));
		delay_ms(100);
		disp(0XC0,"C:");
		delay_ms(10);
		write(0xc2,((z_val/1000)+0x30));
		delay_ms(10);
		write(0xc3,(((z_val%1000)/100)+0x30));
		delay_ms(10);
		write(0xc4,((((z_val%1000)/10)%10)+0x30));
		delay_ms(10);
		write(0xc5,(((z_val%1000)%10)+0x30));
		delay_ms(100);
		//UART0_SendString(result);
		//UART0_SendString("\r\n");
		delay_ms(1000);
	}
}

