#include <lpc214x.h> // Includes LPC2148 register definitions

void timer0_delay(unsigned int ms); // Function prototype for timer-based delay
void pll_init();

int main(void) {
    pll_init();                // Initialize PLL for Fosc = 12MHz, CCLK = 60MHz, PCLK = 60MHz

    PINSEL0 = 0x00000000;      // Enable GPIO on all pins
    PINSEL1 = 0x00000000;
    PINSEL2 = 0x00000000;
    IO1DIR = 0x00000000;       // Set all pins on Port 1 as input
    IO0DIR = 0xFFFFFFFF;       // Set all pins on Port 0 as output

    while (1) {
        if ((IO1PIN & (1 << 16)) == 0) {
            IO0SET = (1 << 19); // Set P0.19
            timer0_delay(1000); // 1 second delay
        } else if ((IO1PIN & (1 << 17)) == 0) {
            IO0SET = (1 << 20); // Set P0.20
            timer0_delay(1000); // 1 second delay
        } else if ((IO1PIN & (1 << 18)) == 0) {
            IO0SET = (1 << 21); // Set P0.21
            timer0_delay(1000); // 1 second delay
        } else {
            IO0CLR = (1 << 19); // Clear P0.19
            IO0CLR = (1 << 20); // Clear P0.20
            IO0CLR = (1 << 21); // Clear P0.21
            timer0_delay(1000); // 1 second delay
        }
    }
}

void pll_init() {
    PLL0CON = 0x01;     // Enable the PLL
    PLL0CFG = 0x24;     // Set multiplier and divider values (M = 5, P = 2)
    PLL0FEED = 0xAA;    // Feed sequence
    PLL0FEED = 0x55;
    
    while (!(PLL0STAT & (1 << 10))); // Wait for PLL to lock
    
    PLL0CON = 0x03;     // Connect the PLL
    PLL0FEED = 0xAA;    // Feed sequence
    PLL0FEED = 0x55;
    
    VPBDIV = 0x01;      // Set PCLK = CCLK
}

void timer0_delay(unsigned int ms) {
    T0CTCR = 0x0;       // Select Timer Mode
    T0TCR = 0x02;       // Reset Timer
    T0PR = 59999;       // Prescaler value for 1ms (60000 cycles with 60MHz PCLK)
    T0TCR = 0x01;       // Start Timer

    while (ms > 0) {
        while (T0TC < 1000); // Wait until Timer Counter reaches 1000 (1 ms)
        T0TC = 0;           // Reset Timer Counter
        ms--;
    }

    T0TCR = 0x00;       // Stop Timer
}
