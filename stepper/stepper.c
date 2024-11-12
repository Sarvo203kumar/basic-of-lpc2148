#include  <lpc214x.h>	 



const unsigned int Step_Sequence[4]={0x01,0x02,0x04,0x08};		//1Phase Full step sequence

void  Delay(unsigned char j)	
{  
 unsigned int  i;
 for(;j>0;j--)
 {
  for(i=0; i<60000; i++);
 } 
}
                               

int  main(void)
{ 
 unsigned char Index=0; 
 PINSEL0 = 0x00000000;		
 PINSEL1 = 0x00000000;
 PINSEL2=  0x00000000;
 IO1DIR = (1<<20) | (1<<21) | (1<<22) | (1<<23);		// Set P1.20, P1.21, P1.22, P1.23 as Output

 
 while(1)	
 { 
  for(Index=0;Index<4;Index++)
  {
   IO1CLR=0x00F00000;
   IO1PIN|=Step_Sequence[Index] << 20;
   Delay(10);
  }
 }
 return(0);
}
