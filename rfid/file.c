#include <lpc214x.h>
#include "stdutils.h"
#include "systeminit.h"
#include "lcd.h"

#define SBIT_WordLength    0x00u
#define SBIT_DLAB          0x07u
#define SBIT_FIFO          0x00u
#define SBIT_RxFIFO        0x01u
#define SBIT_TxFIFO        0x02u

#define SBIT_RDR           0x00u
#define SBIT_THRE          0x05u

#define TX0_PINSEL         0
#define RX0_PINSEL         2

unsigned int i = 0;
unsigned char lcd_address[] = {
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
    0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF};

/* Function to initialize the UART0 at specified baud rate */
void uart_init(uint32_t baudrate)
{
    uint32_t var_RegValue_u32;

    PINSEL0 |= (1<<RX0_PINSEL) | (1<<TX0_PINSEL);   // Configure P0.0/P0.1 as RX0 and TX0 

    U0FCR = (1<<SBIT_FIFO) | (1<<SBIT_RxFIFO) | (1<<SBIT_TxFIFO); // Enable FIFO and reset Rx/Tx FIFO buffers    
    U0LCR = (0x03<<SBIT_WordLength) | (1<<SBIT_DLAB);             // 8bit data, 1Stop bit, No parity

    var_RegValue_u32 = (pclk / (16 * baudrate)); 
    U0DLL = var_RegValue_u32 & 0xFF;
    U0DLM = (var_RegValue_u32 >> 0x08) & 0xFF;

    util_BitClear(U0LCR, SBIT_DLAB);  // Clear DLAB after setting DLL, DLM
}

/* Function to transmit a char */
void uart_TxChar(char ch)
{
    while (util_IsBitCleared(U0LSR, SBIT_THRE)); // Wait for previous transmission
    U0THR = ch;                                  // Load the data to be transmitted
}

/* Function to receive a char */
char uart_RxChar()
{
    while (util_IsBitCleared(U0LSR, SBIT_RDR));  // Wait till the data is received
    return U0RBR;                                // Read received data    
}

/* Function to receive a string via UART */
void uart_RxString(char *str, unsigned int maxLength)
{
    char ch;
    unsigned int i = 0;
    while (i < maxLength - 1) // Reserve space for the null terminator
    {
        ch = uart_RxChar();
        if (ch == '\n' || ch == '\r') // End of string
        {
            break;
        }
        str[i++] = ch;
    }
    str[i] = '\0'; // Null terminate the string
}

int main()
{
    char receivedString[32];

    SystemInit();
    uart_init(9600);  // Initialize the UART0 for 9600 baud rate
    IO1DIR = (1<<en) | (1<<rs) | (1<<d4) | (1<<d5) | (1<<d6) | (1<<d7);  

    init();
    disp(0x80, "RFID Reader");
    delay(50);
    command(0x01);

    while (1)
    {
        uart_RxString(receivedString, sizeof(receivedString)); // Receive a string via UART

        command(0x01); // Clear display
        i = 0;
        while (receivedString[i] != '\0' && i < sizeof(lcd_address))
        {
            command(lcd_address[i]);   // Move the LCD cursor to the correct position
            write(lcd_address[i],receivedString[i]);  // Display the received character on the LCD
            i++;
        }
        uart_TxChar('\n'); // Echo a newline character back through UART
        uart_TxChar('\r'); // Echo a carriage return character back through UART
    }       
}
