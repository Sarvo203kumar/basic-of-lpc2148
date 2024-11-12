#include <LPC213x.h>

void init(void);
void command(unsigned char);
void write(unsigned char,unsigned char);
void disp(unsigned char,const unsigned char*);
void assign(unsigned char);
void delay(long int);

#define rs 16
#define en 17

#define d4 18
#define d5 19
#define d6 20
#define d7 21

unsigned char z;
unsigned int a;

int main()
{
	IO1DIR = (1<<en)|(1<<rs)| (1<<d4)|(1<<d5)|(1<<d6)|(1<<d7);
	init();
	while(1)
	{
		disp(0x80,"ADC:");
		write(0x84,((a/100)+0x30));
		write(0x85,(((a%100)/10)+0x30));
		write(0x86,(((a%100)%10)+0x30));
		PINSEL1 |= (0x01 << 24); // handle ADC 0.1
		AD0CR = 0x00250402; //SEL = 0x02 (1st adc), CLKDIV = 0x04 (12/4=3+1 = 4mhz), CLK = 010 (8bit conversion), BURST and PDN =1, CLK = 111 (7 bit conversion)
		AD0CR |= 0x01000000; //start conversion 
		while(!(AD0GDR & 0x80000000));	//wait for done bit set
		a = ((AD0GDR >> 6)&0xff);	//6 to 15 is result shift to 0 to x and mask with 8bit value (0xff) 10bit (0x3ff) 7bit(0x7f);
	}
}

void init(void)
{
	command(0x02);
	command(0x28);
	command(0x06);
	command(0x0c);
	command(0x01);
}

void command(unsigned char x)
{
	z = (x & 0xf0);
	assign(z);
	IO1SET = (1<<en);
	delay(10);
	IO1CLR = (1<<en);
	z = ((x<<4)&0xf0);
	assign(z);
	IO1SET = (1<<en);
	delay(10);
	IO1CLR = (1<<en);
}

void write(unsigned char x,unsigned char y)
{
	command(x);
	z = (y & 0xf0);
	assign(z);
	IO1SET = (1<<rs);
	IO1SET = (1<<en);
	delay(10);
	IO1CLR = (1<<en);
	IO1CLR = (1<<rs);
	z = ((y<<4)&0xf0);
	assign(z);
	IO1SET = (1<<rs);
	IO1SET = (1<<en);
	delay(10);
	IO1CLR = (1<<en);
	IO1CLR = (1<<rs);
}

void disp(unsigned char x, const unsigned char *y)
{
	unsigned char q;
	while(*y) write(x++,*y++);
}

void assign(unsigned char x)
{
	if(x & 0x10) IO1SET = (1<<d4); else IO1CLR = (1<<d4);
	if(x & 0x20) IO1SET = (1<<d5); else IO1CLR = (1<<d5);
	if(x & 0x40) IO1SET = (1<<d6); else IO1CLR = (1<<d6);
	if(x & 0x80) IO1SET = (1<<d7); else IO1CLR = (1<<d7);
}

void delay(long int x)
{
	while(x--);
}

