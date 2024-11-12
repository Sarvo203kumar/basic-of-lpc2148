#include <lpc214x.h>
#include <stdint.h>
#include <stdio.h>
#define en 16 // Define the pin connected to EN of the LCD
#define rs 17 // Define the pin connected to RS of the LCD
#define d4 18 // Define the pin connected to D4 of the LCD
#define d5 19 // Define the pin connected to D5 of the LCD
#define d6 20 // Define the pin connected to D6 of the LCD
#define d7 21 // Define the pin connected to D7 of the LCD
unsigned int fire ;
// Function to generate delay in milliseconds
void delay_ms(uint16_t j)
{
    uint16_t x, i;
    for (i = 0; i < j; i++)
    {
        for (x = 0; x < 6000; x++); // Loop to generate 1 millisecond delay with Cclk = 60MHz
    }
}

// Function to initialize UART0
void UART0_Init(uint32_t baudrate)
{
    PINSEL0 |= 0x00000005; // Enable RxD0 and TxD0
    U0LCR = 0x83;          // 8 bits, no Parity, 1 Stop bit
    U0DLM = 0x00;
    U0DLL = 0x61;          // For baud rate of 9600 with Pclk = 15MHz
    U0LCR = 0x03;          // Disable DLAB
}

// Function to send a character via UART0
void UART0_TxChar(char ch)
{
    U0THR = ch;
    while (!(U0LSR & 0x40)); // Wait till THR is empty
}

// Function to send a string via UART0
void UART0_SendString(const char *str)
{
    while (*str)
    {
        UART0_TxChar(*str++);
    }
}

// Function to receive a string via UART0
int UART0_RxString(char *str)
{
    int i = 0;
		char ch;
    while (1)
    {
        while (!(U0LSR & 0x01)); // Wait till the data is received
         ch = U0RBR;
        if (ch == '\r') // Check for end of string
        {
            str[i] = '\0';
            break;
        }
        else
        {
            str[i] = ch;
            i++;
        }
    }
    return i;
}

// Function to send a command to the LCD
void command(uint8_t cmd)
{
    IO1CLR = (1 << rs);          // RS = 0 for command
    IO1SET = (cmd & 0xF0) << d4; // Send higher nibble
    IO1SET = (1 << en);          // EN = 1
    delay_ms(1);
    IO1CLR = (1 << en); // EN = 0
    delay_ms(1);
    IO1CLR = 0xF0000;          // Clear higher nibble
    IO1SET = (cmd & 0x0F) << d4; // Send lower nibble
    IO1SET = (1 << en);          // EN = 1
    delay_ms(1);
    IO1CLR = (1 << en); // EN = 0
    delay_ms(1);
}

// Function to write a character to the LCD
void write(char data)
{
    IO1SET = (1 << rs);          // RS = 1 for data
    IO1SET = (data & 0xF0) << d4; // Send higher nibble
    IO1SET = (1 << en);          // EN = 1
    delay_ms(1);
    IO1CLR = (1 << en); // EN = 0
    delay_ms(1);
    IO1CLR = 0xF0000;          // Clear higher nibble
    IO1SET = (data & 0x0F) << d4; // Send lower nibble
    IO1SET = (1 << en);          // EN = 1
    delay_ms(1);
    IO1CLR = (1 << en); // EN = 0
    delay_ms(1);
}

// Function to display a string on the LCD
void disp(uint8_t pos, const char *str)
{
    command(pos);
    while (*str)
    {
        write(*str++);
    }
}

// Function to initialize the LCD
void init()
{
    IO1DIR |= (1 << en) | (1 << rs) | (0xF << d4); // Set LCD pins as output
    delay_ms(20);                                 // Wait for LCD to power up
    command(0x02);                                // Initialize LCD in 4-bit mode
    command(0x28);                                // 2 lines, 5x7 matrix
    command(0x0C);                                // Display on, cursor off
    command(0x06);                                // Increment cursor
    command(0x01);                                // Clear display
    delay_ms(5);
}

int main(void)
{
	
    char str[50];
    

    IO1DIR |= (1 << en) | (1 << rs) | (1 << d4) | (1 << d5) | (1 << d6) | (1 << d7); // Set LCD pins as output
    init();
    disp(0x80, "SERIAL TEST:");
    delay_ms(1000);
    command(0x01); // Clear display
    UART0_Init(9600);

    while (1)
    {
        fire = UART0_RxString(str);
        UART0_SendString("Received String: ");
        UART0_SendString(str);
        UART0_SendString("\n\r");
        command(0x01); // Clear display
        disp(0x80, str); // Display the received string at the beginning of the first line
        delay_ms(1000); // Delay before clearing the display
    }
}
