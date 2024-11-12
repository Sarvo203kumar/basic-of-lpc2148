
#include <lpc214x.h>
#include "stdutils.h"


/***************************************************************************************************
                        void SystemInit(void)
****************************************************************************************************
 * I/P Arguments: none.
 * Return value  : none

 * description  :This function is used to enable the PLL and set the operating freq to 60Mhz
***************************************************************************************************/
#define PLOCK 0x00000400

void SystemInit(void)
{

   PLL0CON = 0x01; // PPLE=1 & PPLC=0, only PLL enabled but not connected 
   PLL0CFG = 0x24; // set the multipler to 5 (i.e actually 4)  12x5 = 60 Mhz (M - 1 = 4)!!!
                   // Set P=2 since we want FCCO in range!!!
                   // So , Assign PSEL =01 in PLL0CFG as per the table.


   PLL0FEED = 0xAA; // Unlock the PLL registers by sending the sequence(0xAA-0x55)
   PLL0FEED = 0x55;


       // check whether PLL has locked on to the  desired freq by reading the lock bit
     // in the PPL0STAT register

   while( !( PLL0STAT & PLOCK ))
     {
         ;
     }

     // now enable(again) and connect
    PLL0CON = 0x03;


   PLL0FEED = 0xAA; // lock the PLL registers after setting the required PLL
   PLL0FEED = 0x55;

   VPBDIV = 0x01; // PCLK is same as CCLK i.e 60Mhz  
}
