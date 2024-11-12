#include <lpc214x.h> 
#define SwitchPinNumber 31
#define LedPinNumber 20 
int main()
{
unsigned int switchStatus;
PINSEL2 = 0x000000; 
IODIR1 = ((1<<LedPinNumber) | (0<<SwitchPinNumber)); 
while(1) 
{ 

switchStatus = (IOPIN1>>SwitchPinNumber) & 0x01 ; 
if(switchStatus == 1) 
{ 
IOPIN1 = (1<<LedPinNumber); 
} 
else
{ 
IOPIN1 = (0<<LedPinNumber);
} 
}
}