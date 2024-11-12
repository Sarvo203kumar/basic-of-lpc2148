

#include "uart.h"
#include "systemInit.h"
#include "eeprom.h"

/* start the main program */
int main() 
{
    char write_str[] = "Welcome to ARM programming ";
    char read_str[50];
	uint16_t eeprom_address = 0x00;

	SystemInit();
    UART0_Init(9600);	 	          
    EEPROM_Init(AT24C16,P0_3,P0_2);		  /* Eeprom IC:AT24C16   SDA:P0_3    SCL:P0_2 */ 

    UART0_Printf("\n\rEeprom Write String: %s", write_str); //Print the message on UART
    EEPROM_WriteString(eeprom_address, write_str); // Write the string at memoryLocation    0x00


    EEPROM_ReadString(eeprom_address, read_str); // Read the string from memoryLocation 0x00
    UART0_Printf("\n\rEeprom Read String: %s", read_str); //Print the message on UART

    while (1);
}
