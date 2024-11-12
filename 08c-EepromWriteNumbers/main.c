

#include "uart.h"
#include "systemInit.h"
#include "eeprom.h"

uint16_t writeNum_16 = 12345;
uint32_t writeNum_32 = 12345678;

uint16_t readNum_16;
uint32_t readNum_32;

#define num_16_address 0x00
#define num_32_address 0x02 // As num_16 takes two bytes, new address will start from +2 location

/* start the main program */
int main() 
{
	SystemInit();
    UART0_Init(9600);	 	          
    EEPROM_Init(AT24C16,P0_3,P0_2);		  /* Eeprom IC:AT24C16   SDA:P0_3    SCL:P0_2 */ 

	EEPROM_WriteNBytes(num_16_address,(uint8_t *)&writeNum_16,2); //write 2-bytes of data(writeNum_16) at 0x00.
	EEPROM_WriteNBytes(num_32_address,(uint8_t *)&writeNum_32,4); //write 4-bytes of data(writeNum_32) at 0x02.
	
	EEPROM_ReadNBytes(num_16_address,(uint8_t *)&readNum_16,2); //Read 2-bytes of data from 0x00 into readNum_16
	EEPROM_ReadNBytes(num_32_address,(uint8_t *)&readNum_32,4); //Read 4-bytes of data from 0x02 into readNum_32
	
	UART0_Printf("num_16 = %5u    num_32=%8U",readNum_16,readNum_32);

    while (1);
}
