#include  <lpc214x.h>				//Includes LPC2148 register definitions


void  Delay(unsigned int j)	   //This Function is used to cause delay between LED ON and OFF events
{  
 unsigned int  i;
 for(;j>0;j--)
 {
  for(i=0; i<60000; i++);
 } 
}

int main(void)
{
	PINSEL0 = 0x00000000;		// Enable GPIO on all pins
  PINSEL1 = 0x00000000;
	PINSEL2 = 0x00000000;
	IO1DIR = (1<<16);	
	while(1)
	{
		IO1SET=(1<<16);
		 Delay(100);
		IO1CLR=(1<<16);
		 Delay(100);
	}

}