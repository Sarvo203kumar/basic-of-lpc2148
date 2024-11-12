#include <LPC214x.h> 
#include "lcd.h"

#define buzzer 31
#define sensor 29
 
int main()
{
	unsigned int switchStatus1;
IO1DIR = (1<<en)|(1<<rs)|(1<<d4)|(1<<d5)|(1<<d6)|(1<<d7)|(1<<buzzer) | (0<<sensor);
PINSEL0 =0X00000000;
PINSEL1 =0X00000000;
PINSEL2 = 0x00000000;
	
init();
	disp(0x80,"SECURITY SYSTEM");
	delay(100);
command(0x01); // Clear display
while(1)
{ 
switchStatus1 = (IOPIN1>>sensor) & 0x01 ;

if(switchStatus1 ==0) 
{ 
IOPIN1 =(1<<31);
//IOSET1=(1<<31);
	delay(1);
	disp(0x80,"HUMAN DETECTOR");
	command(0x01);
	delay(100);
}
else
{
//IOPIN1 =(1<<31);
IOCLR1=(1<<31);
	delay(1);
	disp(0x80,"HUMAN NOT");
	disp(0XC0," DETECTOR");
	command(0x01);
	delay(100);
}

}
}