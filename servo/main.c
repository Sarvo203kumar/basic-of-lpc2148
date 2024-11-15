#include <lpc214x.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// Function Prototypes
void initializePLL(void);
void delay_ms(uint16_t z);
__irq void PWM_ISR(void);
void LCD_SEND(char command);
void LCD_INITIALIZE(void);
void LCD_DISPLAY(char* msg);

void initializePLL(void) {      // Function to use PLL for clock generation
    PLL0CON = 0x01;
    PLL0CFG = 0x24;
    PLL0FEED = 0xAA;
    PLL0FEED = 0x55;
    while (!(PLL0STAT & 0x00000400));
    PLL0CON = 0x03;
    PLL0FEED = 0xAA;
    PLL0FEED = 0x55;
    VPBDIV = 0x01;
}

void delay_ms(uint16_t z) {     // Function to generate 1 millisecond delay with Cclk (60MHz)
    uint16_t x, i;
    for (i = 0; i < z; i++) {
        for (x = 0; x < 6000; x++);
    }
}

__irq void PWM_ISR(void) {
    if (PWMIR & 0x0001) {
        PWMIR = 0x0001; // Clear interrupt for MR0
    }
    if (PWMIR & 0x0008) {
        PWMIR = 0x0008; // Clear interrupt for MR3
    }
    VICVectAddr = 0x00000000; // Clear the interrupt in the VIC
}

void LCD_SEND(char command) { // Function to send hex commands
    IO0PIN = ((IO0PIN & 0xFFFF00FF) | ((command & 0xF0) << 8)); // Send upper nibble of command
    IO0SET = 0x00000040;  // Make Enable HIGH
    IO0CLR = 0x00000030;  // Make RS & RW LOW
    delay_ms(5);
    IO0CLR = 0x00000040;  // Make Enable LOW
    delay_ms(5);
    IO0PIN = ((IO0PIN & 0xFFFF00FF) | ((command & 0x0F) << 12)); // Send Lower nibble of command
    IO0SET = 0x00000040;  // ENABLE HIGH
    IO0CLR = 0x00000030;  // RS & RW LOW
    delay_ms(5);
    IO0CLR = 0x00000040;  // ENABLE LOW
    delay_ms(5);
}

void LCD_INITIALIZE(void) { // Function to get ready the LCD
    IO0DIR = 0x0000FFF0; // Sets pin P0.12, P0.13, P0.14, P0.15, P0.4, P0.6 as OUTPUT
    delay_ms(20);
    LCD_SEND(0x02); // Initialize LCD in 4-bit mode of operation
    LCD_SEND(0x28); // 2 lines (16X2)
    LCD_SEND(0x0C); // Display on, cursor off
    LCD_SEND(0x06); // Auto increment cursor
    LCD_SEND(0x01); // Display clear
    LCD_SEND(0x80); // First line, first position
}

void LCD_DISPLAY(char* msg) { // Function to print characters sent one by one
    uint8_t i = 0;
    while (msg[i] != 0) {
        IO0PIN = ((IO0PIN & 0xFFFF00FF) | ((msg[i] & 0xF0) << 8)); // Send Upper nibble
        IO0SET = 0x00000050; // RS HIGH & ENABLE HIGH to print data
        IO0CLR = 0x00000020; // RW LOW (Write mode)
        delay_ms(2);
        IO0CLR = 0x00000040; // EN = 0
        delay_ms(5);
        IO0PIN = ((IO0PIN & 0xFFFF00FF) | ((msg[i] & 0x0F) << 12)); // Send Lower nibble
        IO0SET = 0x00000050; // RS & EN HIGH
        IO0CLR = 0x00000020;
        delay_ms(2);
        IO0CLR = 0x00000040;
        delay_ms(5);
        i++;
    }
}

int main() {    
    // Variable declarations at the beginning
    char displayadc[18];
    float adc;
    float angle;
    char anglevalue[18];
    float dutycycle;
    unsigned short int adcvalue;

    // Initialize LCD
    LCD_INITILIZE();  // Calls function to get ready the LCD to display
    
    LCD_DISPLAY("CIRCUIT DIGEST");
    delay_ms(900); 

    LCD_SEND(0xC0);
    LCD_DISPLAY("SERVO LPC2148");
    delay_ms(900); 

    // Setup PWM
    PINSEL0 |= 0x00000008;  // Setting pin P0.1 of LPC2148 as PWM3
    VICVectAddr0 = (unsigned)PWM_ISR;  // PWM ISR Address 
    VICVectCntl0 = (0x00000020 | 8);  // Enable PWM IRQ slot 
    VICIntEnable |= 0x00000100;  // Enable PWM interrupt 
    VICIntSelect |= 0x00000000;  // PWM configured as IRQ 
    PWMTCR = 0x02;  // Reset and disable counter for PWM 
    PWMPR = 0x1D;  // Prescale Register value 
    PWMMR0 = 20000;  // Time period of PWM wave, 20ms
    PWMMR3 = 650;  // Ton of PWM wave 0.65ms
    PWMMCR = 0x00000203;  // Reset and interrupt on MR0 match, interrupt on MR3 match
    PWMLER = 0x09;  // Latch enable for PWM3 and PWM0 
    PWMPCR = 0x0800;  // Enable PWM3 and PWM 0, single edge controlled PWM
    PWMTCR = 0x09;  // Enable PWM and counter
    
    // ADC and PWM setup for the servo motor
    PINSEL1 = 0x01000000;  // Setting P0.28 as ADC INPUT (from potentiometer)
    AD0CR = ((14 << 8) | (1 << 21));  // Setting clock and PDN for A/D Conversion
    PWMPCR |= (1 << 9);  // Enable PWM3 output at pin P0.1 for servo

    // Main loop
    while (1) {
        AD0CR |= (1 << 1);  // Select AD0.1 channel in ADC register               
        AD0CR |= (1 << 24);  // Start the A/D conversion
        
        // Wait for the conversion to complete
        while ((AD0DR1 & (1 << 31)) == 0);  // Check the DONE bit in ADC Data register
        
        adcvalue = (AD0DR1 >> 6) & 0x3FF;  // Get the RESULT from ADC data register
        dutycycle = adcvalue / 4;  // Formula to get dutycycle values (0 to 255)
        PWMMR3 = dutycycle;  // Set dutycycle value to PWM match register 
        PWMLER = 0x08;  // Enable PWM output with dutycycle value
        delay_ms(50);

        // Display ADC value and angle on LCD
        LCD_SEND(0x80);
        sprintf(displayadc, "adcvalue=%f", dutycycle); 
        LCD_DISPLAY(displayadc);  // Display ADC value (0 to 1023)
        
        angle = adcvalue / 5.7;  // Convert ADC value to angle (0 to 180 degrees)
        LCD_SEND(0xC0); 
        sprintf(anglevalue, "ANGLE=%.2f deg  ", angle);  
        LCD_DISPLAY(anglevalue);  // Display angle value
    }
}
