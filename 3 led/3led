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
	IO1DIR = 0X0;
	IO0DIR =0XfffffFFF;
	while(1)
	{
	if((IO1PIN&(1<<16))==0)
		{
		IO1SET=(1<<19);
		Delay(100);
		}
	else if((IO1PIN&(1<<17))==0)
		{
		IO1SET=(1<<20);
		Delay(100);
		}	
	else if((IO1PIN&(1<<18))==0)
		{
		IO1SET=(1<<21);
		Delay(100);
		}	

	else
		{
		IO1CLR=(1<<19);
		IO1CLR=(1<<20);
		IO1CLR=(1<<21);
		Delay(100);
		}
	}

}