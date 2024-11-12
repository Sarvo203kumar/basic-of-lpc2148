#include <lpc214x.h>
#include <stdint.h>

void delay_ms(uint16_t j)
{
    uint16_t x, i;
    for (i = 0; i < j; i++)
    {
        for (x = 0; x < 6000; x++);    /* loop to generate 1 millisecond delay with Cclk = 60MHz */
    }
}

int main(void)
{
    uint32_t value;

    PINSEL1 = 0x00080000;   /* P0.25 as DAC output */
    IO0DIR = (IO0DIR & 0xFFFFF0FF); /* Input pins for switch. P0.8 sine, P0.9 triangular, P0.10 sawtooth, P0.11 square */
    PINSEL1 |= 0x01000000; // handle ADC 0.1
    AD0CR = 0x00200402; // SEL = 0x02 (ADC channel 1), CLKDIV = 0x04 (12/4=3+1 = 4MHz), BURST = 0, PDN = 1, START = 0
    AD0CR |= (1 << 21); // PDN = 1, enable ADC
    AD0CR |= (1 << 24); // start conversion

    while (1)
    {
        if (!(IO0PIN & (1 << 8))) /* If switch for sine wave is pressed */
        {
            while (!(IO0PIN & (1 << 8))) // Stay in this loop as long as the button is pressed
            {
                AD0CR |= (1 << 24); // Start conversion
                while (!(AD0DR1 & (1 << 31))); // Wait for conversion to complete
                value = (AD0DR1 >> 6) & 0x000003FF; // Read ADC value
                DACR = (1 << 16) | (value << 6); // Update DAC
                delay_ms(1);
            }
        }
    }
}
