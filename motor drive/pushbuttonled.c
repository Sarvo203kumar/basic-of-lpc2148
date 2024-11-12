#include <lpc214x.h> 
#define SwitchPinNumber1 31
#define SwitchPinNumber2 30
#define SwitchPinNumber3 29
#define SwitchPinNumber4 28
#define LedPinNumber1 20 
#define LedPinNumber2 19 
#define LedPinNumber3 18
#define LedPinNumber4 17
int main()
{
unsigned int switchStatus1,SwitchStatus4;
unsigned int switchStatus2,SwitchStatus3;
PINSEL2 = 0x000000; 
IODIR1 = ((1<<LedPinNumber1) | (0<<SwitchPinNumber1) |(1<<LedPinNumber2) | (0<<SwitchPinNumber2) |(1<<LedPinNumber3) | (0<<SwitchStatus3)|(1<<LedPinNumber4)|(0<<SwitchPinNumber4));

while(1)
{ 

switchStatus1 = (IOPIN1>>SwitchPinNumber1) & 0x01 ; 
switchStatus2 = (IOPIN1>>SwitchPinNumber2) & 0x01 ; 
SwitchStatus3 = (IOPIN1>>SwitchPinNumber3) & 0x01 ;
SwitchStatus4 = (IOPIN1>>SwitchPinNumber4) & 0x01 ;
if(switchStatus1 == 1) 
{ 
IOPIN1 = (1<<LedPinNumber1)|(1<<LedPinNumber3);
//IOPIN1 =(1<<LedPinNumber3);
} 
else if (switchStatus2 == 1) 
{ 
IOPIN1 = (1<<LedPinNumber2)|(1<<LedPinNumber4);
}
else if (SwitchStatus3 ==1)
{
IOPIN1 = (1<<LedPinNumber3);
}
else if (SwitchStatus4 ==1)
{
IOPIN1 = (1<<LedPinNumber4);
}
else
{
IOPIN1 =(0<<LedPinNumber1);
IOPIN1 =(0<<LedPinNumber2);
IOPIN1 =(0<<LedPinNumber3);
IOPIN1 =(0<<LedPinNumber4);
}

}
}