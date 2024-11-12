#include <LPC213x.h>

void init(void);
void command(unsigned char);
void write(unsigned char,unsigned char);
void disp(unsigned char,const unsigned char*);
void assign(unsigned char);
void delay(unsigned int x);

#define rs 16
#define en 17

#define d4 18
#define d5 19
#define d6 20
#define d7 21
#define led 21
unsigned char z;
unsigned int a;

int main()
{
	IO1DIR = (1<<en)|(1<<rs)| (1<<d4)|(1<<d5)|(1<<d6)|(1<<d7);\
	IO0DIR=(1<<21);
	init();
	while(1)
	{
		disp(0x80,"ADC:");
		write(0x84,((a/1000)+0x30));
		write(0x85,(((a%1000)/100)+0x30));
		write(0x86,((((a%1000)/10)%10)+0x30));
		write(0x87,(((a%1000)%10)+0x30));
		PINSEL1 =0x01000000; // handle ADC 0.1
		AD0CR = 0x00200402; //SEL = 0x02 (1st adc), CLKDIV = 0x04 (12/4=3+1 = 4mhz), CLK = 010 (8bit conversion), BURST and PDN =1, CLK = 011 (7 bit conversion)
		AD0CR =AD0CR | (1<<24); //start conversion 
		while(!(AD0DR1 & 0x80000000));	//wait for done bit set
		a = ((AD0DR1 >> 6)&0x000003ff);	//6 to 15 is result shift to 0 to x and mask with 8bit value (0xff) 10bit (0x3ff) 7bit(0x7f);
		if(a>200)
		{
			IO0SET=(1<<21);
			delay(10);
		}
		else 
		{
			IO0CLR=(1<<21);
			delay(10);
		}
	}
}

void init(void) {
    command(0x33); // Initialization sequence for 4-bit mode
    command(0x32);
    command(0x28); // 4-bit mode, 2 lines, 5x7 dots
    command(0x0C); // Display on, cursor off
    command(0x06); // Entry mode set, increment cursor
    command(0x01); // Clear display
    //delay(2000); // Allow time for clear display command to execute
}

void command(unsigned char x) {
    z = (x & 0xF0);
    assign(z);
    IO1CLR = (1 << rs); // RS = 0 for command
    IO1SET = (1 << en);
    delay(1);
    IO1CLR = (1 << en);
    delay(1);
    z = ((x << 4) & 0xF0);
    assign(z);
    IO1SET = (1 << en);
    delay(1);
    IO1CLR = (1 << en);
    delay(1);
}

void write(unsigned char x, unsigned char y) {
    command(x);
    z = (y & 0xF0);
    assign(z);
    IO1SET = (1 << rs); // RS = 1 for data
    IO1SET = (1 << en);
    delay(1);
    IO1CLR = (1 << en);
    delay(1);
    z = ((y << 4) & 0xF0);
    assign(z);
    IO1SET = (1 << rs); // RS = 1 for data
    IO1SET = (1 << en);
    delay(1);
    IO1CLR = (1 << en);
    delay(1);
}

void disp(unsigned char x, const unsigned char *y) {
    unsigned char q;
    for (q = 0; y[q] != '\0'; q++) {
        write(x + q, y[q]);
    }
}

void assign(unsigned char x) {
    if (x & 0x10) IO1SET = (1 << d4); else IO1CLR = (1 << d4);
    if (x & 0x20) IO1SET = (1 << d5); else IO1CLR = (1 << d5);
    if (x & 0x40) IO1SET = (1 << d6); else IO1CLR = (1 << d6);
    if (x & 0x80) IO1SET = (1 << d7); else IO1CLR = (1 << d7);
}

void delay(unsigned int x) {
    volatile unsigned int i, j;
    for (i = 0; i < x; i++) {
        for (j = 0; j < 3000; j++);
    }
}



