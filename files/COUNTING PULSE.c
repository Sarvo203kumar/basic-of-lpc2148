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

#define key (IO1PIN & (1<<22))

unsigned char z,a;

int main()
{
	IO1DIR = (1<<en)|(1<<rs)|(1<<d4)|(1<<d5)|(1<<d6)|(1<<d7);
	init();
	disp(0x80,"COUNT");
	while(1)
	{
	disp(0x80,"COUNT:");
	write(0x87,a+0x30);
	if(!key){while(!key);if(a<9){a++;} else a=0;}
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
	for(q=0; y[q]!='\0' ; q++)
	{
		write(x+q,y[q]);
	}
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