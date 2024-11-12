#include <LPC214x.h>
#include "lcd.h"

unsigned int a;

int main() {
    // Set direction of pins to output for LCD control
    IO1DIR = (1 << en) | (1 << rs) | (1 << d4) | (1 << d5) | (1 << d6) | (1 << d7);

    // Initialize LCD
    init();

    // Configure ADC pin
    PINSEL1 |= (0x01 << 27); // Select ADC function for pin AD0.1

    // Configure ADC: SEL=2 (AD0.1), CLKDIV=4, BURST=0, CLKS=0, PDN=1, START=0, EDGE=0
    AD0CR = 0x00200402;
    
    while (1) {
        // Display "ADC:" on the LCD
        disp(0x80, "ADC:");
        
        // Start ADC conversion
        AD0CR |= 0x01000000;
        
        // Wait for conversion to complete
        while (!(AD0GDR & 0x80000000));
        
        // Read ADC result (10-bit value)
        a = (AD0GDR >> 6) & 0x3FF;
        
        // Display the ADC result on the LCD
        write(0x84, ((a / 100) + 0x30));     // Hundreds digit
        write(0x85, (((a % 100) / 10) + 0x30)); // Tens digit
        write(0x86, ((a % 10) + 0x30));     // Units digit
        
        delay(1); // Delay to update the display periodically
    }
}
