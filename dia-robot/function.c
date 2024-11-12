#include <lpc214x.h>
#include <stdint.h>

#define SwitchPinNumber1 17
#define SwitchPinNumber2 18
#define SwitchPinNumber3 19
#define SwitchPinNumber4 20

#define LedPinNumber1 28
#define LedPinNumber2 29
#define LedPinNumber3 30
#define LedPinNumber4 31

#define AD04 (1 << 18)   // Select AD0.4 function for P0.25
#define AD01 (1 << 24)   // Select AD0.1 function for P0.28
#define AD02 (1 << 26)   // Select AD0.2 function for P0.29
#define AD03 (1 << 28)   // Select AD0.3 function for P0.30

#define SEL_AD04 (1 << 4)  // Select ADC channel 4
#define SEL_AD01 (1 << 1)  // Select ADC channel 1
#define SEL_AD02 (1 << 2)  // Select ADC channel 2
#define SEL_AD03 (1 << 3)  // Select ADC channel 3

#define CLKDIV (15 - 1)            // 4MHz ADC clock, PCLK = 60MHz
#define BURST_MODE_OFF (0 << 16)   // 0 for burst mode off
#define PowerUP (1 << 21)
#define START_NOW ((0 << 26) | (0 << 25) | (1 << 24))  // Start conversion immediately
#define ADC_DONE (1UL << 31)
#define VREF 3.3

volatile int result1 = 0;
volatile int result2 = 0;
volatile int result3 = 0;
volatile int result4 = 0;

volatile int convert1 = 0;
volatile int convert2 = 0;
volatile int convert3 = 0;
volatile int convert4 = 0;

volatile unsigned int switchStatus1;
volatile unsigned int switchStatus2;
volatile unsigned int switchStatus3;
volatile unsigned int switchStatus4;

long map(long x, long in_min, long in_max, long out_min, long out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void delay_ms(uint16_t j) {
    uint16_t x, i;
    for (i = 0; i < j; i++) {
        for (x = 0; x < 6000; x++);
    }
}

int main() {
    PINSEL0 = 0x000A800A;  // Configure P0.1, P0.7, P0.8 as PWM pins
    PINSEL2 = 0x00000000;  // Configure PORT1 pins as GPIO
    PINSEL1 |= AD04 | AD01 | AD02 | AD03 | (1 << 10);  // Configure ADC and PWM pins

    IODIR1 = ((0 << 17) | (0 << 18) | (0 << 19) | (0 << 20) |
              (1 << 28) | (1 << 29) | (1 << 30) | (1 << 31));  // LED as output, switches as input

    unsigned long AD0CR_setup = (CLKDIV << 8) | BURST_MODE_OFF | PowerUP;

    // PWM setup
    PWMTCR = 0x02;          // Reset and disable counter for PWM
    PWMPR = 0x1D;           // Prescale register
    PWMMR0 = 20000;         // 20 ms PWM period
    PWMMCR = 0x00000002;    // Reset on MR0 match
    PWMLER = 0x7C;          // Latch enable for PWM channels
    PWMPCR = 0x7C00;        // Enable PWM channels
    PWMTCR = 0x09;          // Enable PWM and counter

    while (1) {
        // Check switch status
        switchStatus1 = (IOPIN1 >> SwitchPinNumber1) & 0x01;
        switchStatus2 = (IOPIN1 >> SwitchPinNumber2) & 0x01;
        switchStatus3 = (IOPIN1 >> SwitchPinNumber3) & 0x01;
        switchStatus4 = (IOPIN1 >> SwitchPinNumber4) & 0x01;

        if (switchStatus1 == 1) {
            IOSET1 = (1 << LedPinNumber1);  // Turn on LED1
            AD0CR = AD0CR_setup | SEL_AD01;
            AD0CR |= START_NOW;

            while ((AD0DR1 & ADC_DONE) == 0);
            convert1 = (AD0DR1 >> 6) & 0x3FF;
            result1 = map(convert1, 0, 1023, 0, 2450);
            PWMMR5 = result1;
            PWMLER = 0x20;
            delay_ms(2);
        } else {
            IOCLR1 = (1 << LedPinNumber1);  // Turn off LED1
        }

        // Repeat for other switches and channels...

        // (Additional switch and LED checks go here for switchStatus2, switchStatus3, and switchStatus4)
    }
}
