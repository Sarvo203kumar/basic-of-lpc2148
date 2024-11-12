#include <lpc214x.h>
#include <stdint.h>

#define IER_RBR  0x01
#define BUFFER_SIZE 0x40

unsigned char UART0_BUFFER[BUFFER_SIZE];
unsigned int uart0_count;
unsigned char COMMAND[1];

unsigned char LEDON = '@', LEDOFF = '%';

void UART0_ISR() __irq {
    unsigned char IIRValue;

    IIRValue = U0IIR;
    IIRValue >>= 1;
    IIRValue &= 0x02;

    if (IIRValue == IIR_RDA) {
        UART0_BUFFER[uart0_count] = U0RBR;
        uart0_count++;
        if (uart0_count == BUFFER_SIZE) {
            uart0_count = 0;
        }
    }
    VICVectAddr = 0x0; // Clear the interrupt
}

void delay_ms(uint16_t j) {
    uint16_t x, i;
    for (i = 0; i < j; i++) {
        for (x = 0; x < 6000; x++) {
            // loop to generate 1 millisecond delay with Cclk = 60MHz
        }
    }
}

void UART0_initilize(void) {
    VPBDIV = 0x00; // Configure the APB clock
    PINSEL0 = PINSEL0 | 0x00000005; // Enable UART0 Rx0 and Tx0 pins
    U0LCR = 0x83; // DLAB = 1, 1 stop bit, 8-bit character length
    U0DLM = 0x00; // For baud rate of 9600 with Pclk = 15MHz
    U0DLL = 0x61; // We get these values of U0DLL and U0DLM from formula
    U0LCR &= 0x0F; // DLAB = 0

    VICIntEnable |= (1 << 6); // Enable UART0 interrupt
    VICVectCntl0 = (1 << 5) | 6; // Enable UART IRQ slot
    VICVectAddr0 = (unsigned) UART0_ISR; // UART ISR function address
    U0IER = IER_RBR; // Enable RBR interrupt
}

void UART0_TxChar(char ch) {
    U0THR = ch;
    while ((U0LSR & 0x40) == 0); // Wait till THRE bit becomes 1 (transmission complete)
}

void UART0_SendString(char* str) {
    uint8_t i = 0;
    while (str[i] != '\0') {
        UART0_TxChar(str[i]);
        i++;
    }
}

void wifi_sendATcommands(void) {
    delay_ms(5000);
    UART0_SendString("AT\r\n"); // Sends AT to ESP8266
    delay_ms(3000);
    UART0_SendString("AT+CWMODE=2\r\n"); // Set ESP8266 to AP mode
    delay_ms(3000);
    UART0_SendString("AT+CIFSR\r\n"); // Get the IP address of the ESP8266
    delay_ms(3000);
    UART0_SendString("AT+CIPMUX=1\r\n"); // Enable multiple connections
    delay_ms(3000);
    UART0_SendString("AT+CIPSERVER=1,80\r\n"); // Enable server on port 80
    delay_ms(3000);
}

int main(void) {
    IODIR1 = (1 << 20); // Configure P1.20 as an output pin for the LED
    UART0_initilize(); // Initialize UART0
    wifi_sendATcommands(); // Send AT commands to configure ESP8266

    while (1) {
        if (uart0_count != 0) {
            COMMAND[0] = UART0_BUFFER[uart0_count - 1];

            if (COMMAND[0] == LEDON) {
                IOSET1 = (1 << 20); // Turn on the LED
                delay_ms(100);
            }
            else if (COMMAND[0] == LEDOFF) {
                IOCLR1 = (1 << 20); // Turn off the LED
                delay_ms(100);
            }
        }
    }
}
