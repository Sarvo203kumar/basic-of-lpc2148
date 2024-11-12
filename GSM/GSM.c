
#include <lpc214x.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

__irq void UART0_Interrupt(void);

void GSM_Begin(void);							
void GSM_Calling(char *);
void GSM_HangCall(void);
void GSM_Response(void);
void GSM_Response_Display(void);
void GSM_Msg_Read(int);
bool GSM_Wait_for_Msg(void);
void GSM_Msg_Display(void);
void GSM_Msg_Delete(unsigned int);
void GSM_Send_Msg(char* , char*);
void GSM_Delete_All_Msg(void);

char buff[160];		/* buffer to store responses and messages */
bool status_flag = false;	/* for checking any new message */
volatile int buffer_pointer;
char Mobile_no[14];		/* store mobile no. of received message */
char message_received[60];		/* save received message */
int position = 0;	/* save location of current message */

void delay_ms(uint16_t j)
{
    uint16_t x,i;
	for(i=0;i<j;i++)
	{
    for(x=0; x<6000; x++);    /* loop to generate 1 milisecond delay with Cclk = 60MHz */
	}
}

void UART0_init(void)
{
	PINSEL0 = PINSEL0 | 0x00000005;	/* Enable UART0 Rx0 and Tx0 pins of UART0 */
	U0LCR = 0x83;	/* DLAB = 1, 1 stop bit, 8-bit character length */
	U0DLM = 0x00;	/* For baud rate of 9600 with Pclk = 15MHz */
	U0DLL = 0x61;	/* We get these values of U0DLL and U0DLM from formula */
	U0LCR = 0x03; /* DLAB = 0 */
	U0IER = 0x00000001; /* Enable RDA interrupts */
}

void UART0_TxChar(char ch) /* A function to send a byte on UART0 */
{
	U0IER = 0x00000000; /* Disable RDA interrupts */
	U0THR = ch;
	while( (U0LSR & 0x40) == 0 );	/* Wait till THRE bit becomes 1 which tells that transmission is completed */
	U0IER = 0x00000001; /* Enable RDA interrupts */
}

void UART0_SendString(char* str) /* A function to send string on UART0 */
{
	U0IER = 0x00000000; /* Disable RDA interrupts */
	uint8_t i = 0;
	while( str[i] != '\0' )
	{
		UART0_TxChar(str[i]);
		i++;
	}
	U0IER = 0x00000001; /* Enable RDA interrupts */
}

__irq void UART0_Interrupt(void)
{
	buff[buffer_pointer] = U0RBR;	/* copy UDR(received value) to buffer */
	buffer_pointer++;
	status_flag = true;						            /* flag for new message arrival */
	VICVectAddr = 0x00;
}

int main(void)
{
	buffer_pointer = 0;
	bool is_msg_arrived;
	memset(message_received, 0, 60);
	VICVectAddr0 = (unsigned) UART0_Interrupt;	/* UART0 ISR Address */
	VICVectCntl0 = 0x00000026;	/* Enable UART0 IRQ slot */
	VICIntEnable = 0x00000040;	/* Enable UART0 interrupt */
	VICIntSelect = 0x00000000;	/* UART0 configured as IRQ */
	UART0_init();
	UART0_SendString("GSM Initializing...");
	delay_ms(3000);
	GSM_Begin();	/* check GSM responses and initialize GSM */	
	
	while (1)
		{		
			/*check if any new message received */
			if(status_flag == true)
				{					
						is_msg_arrived = GSM_Wait_for_Msg();	/*check for message arrival*/
						if(is_msg_arrived == true)
						{
								//UART0_SendString("New message");	/* new message arrived */
								delay_ms(1000);
								GSM_Msg_Read(position);		/* read arrived message */		
								delay_ms(3000);
									
							/*check if received message is "call me" */
								if(strstr( message_received,"call me"))
									{									
											GSM_Calling(Mobile_no);		/* call sender of message */
											//UART0_SendString("Calling...");
											delay_ms(15000);
											GSM_HangCall();		/* hang call */
											//UART0_SendString("Hang Call");
											delay_ms(1000);
									}
					
								GSM_Msg_Delete(position);		/* to save SIM memory delete current message */
								//UART0_SendString("Clear msg");
								GSM_Response();
								delay_ms(1000);					
						}											
					is_msg_arrived = false;
					status_flag = false;
				}
			UART0_SendString("Waiting for msg");
			memset(Mobile_no, 0, 14);
			memset(message_received, 0, 60);			
		}
}

void GSM_Begin(void)
{
	while(1)
	{
		UART0_SendString("ATE0\r\n");		/* send ATE0 to check module is ready or not */
		delay_ms(500);
		if(strstr(buff,"OK"))
		{
			GSM_Response();		/* get Response */
			memset(buff,0,160);
			break;
		}
		else
		{
			//UART0_SendString("Error");
		}
	}
	delay_ms(1000);

	//UART0_SendString("Text Mode");
	UART0_SendString("AT+CMGF=1\r\n");	/* select message format as text */
	GSM_Response();
	delay_ms(1000);
}

void GSM_Msg_Delete(unsigned int position)
{
	buffer_pointer=0;
	char delete_cmd[20];
	sprintf(delete_cmd,"AT+CMGD=%d\r\n",position);	/* delete message at specified position */
	UART0_SendString(delete_cmd);
}

void GSM_Delete_All_Msg(void)
{
	UART0_SendString("AT+CMGDA=\"DEL ALL\"\r\n");	/* delete all messages of SIM */	
}

bool GSM_Wait_for_Msg(void)
{
	char msg_location[4];
	int i;
	delay_ms(500);
	buffer_pointer=0;

	while(1)
	{
		if(buff[buffer_pointer]=='\r' || buff[buffer_pointer]== '\n')	/*eliminate "\r \n" which is start of string */
		{
			buffer_pointer++;
		}
		else
			break;
	}
		
	if(strstr(buff,"CMTI:"))		/* "CMTI:" to check if any new message received */
	{
		while(buff[buffer_pointer]!= ',')
		{
			buffer_pointer++;
		}
		buffer_pointer++;
		
		i=0;
		while(buff[buffer_pointer]!= '\r')
		{
			msg_location[i]=buff[buffer_pointer];		/* copy location of received message where it is stored */
			buffer_pointer++;
			i++;
		}

		/* convert string of position to integer value */
		position = atoi(msg_location);
		
		memset(buff,0,strlen(buff));
		buffer_pointer=0;

		return true;
	}
	else
	{
		return false;
	}
}

void GSM_Send_Msg(char *num,char *sms)
{
	char sms_buffer[35];
	buffer_pointer=0;
	sprintf(sms_buffer,"AT+CMGS=\"%s\"\r\n",num);
	UART0_SendString(sms_buffer);		/*send command AT+CMGS="Mobile No."\r */
	delay_ms(200);
	while(1)
	{
		if(buff[buffer_pointer]==0x3e)		/* wait for '>' character*/
		{
			buffer_pointer = 0;
			memset(buff,0,strlen(buff));
			UART0_SendString(sms);		/* send msg to given no. */
			UART0_TxChar(0x1a);		/* send Ctrl+Z then only message will transmit*/
			break;
		}
		buffer_pointer++;
	}
	delay_ms(300);
	buffer_pointer = 0;
	memset(buff,0,strlen(buff));
	memset(sms_buffer,0,strlen(sms_buffer));
}

void GSM_Calling(char *Mob_no)
{
	char call[20];
	sprintf(call,"ATD%s;\r\n",Mob_no);		
	UART0_SendString(call);		/* send command ATD<Mobile_No>; for calling*/
}

void GSM_HangCall(void)
{
	UART0_SendString("ATH\r\n");		/*send command ATH\r to hang call*/
	
}

void GSM_Response(void)
{
	unsigned int timeout=0;
	int CRLF_Found=0;
	char CRLF_buff[2];
	int Response_Length=0;
	while(1)
	{
		if(timeout>=60000)		/*if timeout occur then return */
		return;
		Response_Length = strlen(buff);
		if(Response_Length)
		{
			delay_ms(1);
			timeout++;
			if(Response_Length==strlen(buff))
			{
				for(int i=0;i<Response_Length;i++)
				{
					memmove(CRLF_buff,CRLF_buff+1,1);
					CRLF_buff[1]=buff[i];
					if(strncmp(CRLF_buff,"\r\n",2))
					{
						if(CRLF_Found++==2)		/* search for \r\n in string */
						{
							GSM_Response_Display();		/* display response */
							return;
						}
					}

				}
				CRLF_Found = 0;

			}
			
		}
		delay_ms(1);
		timeout++;
	}
	//status_flag = false;
}

void GSM_Response_Display(void)
{
	buffer_pointer = 0;
	while(1)
	{
		if(buff[buffer_pointer]== '\r' || buff[buffer_pointer]== '\n')		/* search for \r\n in string */
		{
			buffer_pointer++;
		}
		else
			break;
	}
	

	while(buff[buffer_pointer]!='\r')		/* display response till "\r" */
	{
		UART0_TxChar(buff[buffer_pointer]);								
		buffer_pointer++;
	}
	buffer_pointer=0;
	memset(buff,0,strlen(buff));
}

void GSM_Msg_Read(int position)
{
	char read_cmd[10];
	sprintf(read_cmd,"AT+CMGR=%d\r\n",position);
	UART0_SendString(read_cmd);		/* read message at specified location/position */
	GSM_Msg_Display();		/* display message */
}

void GSM_Msg_Display(void)
{
	delay_ms(500);
	if(!(strstr(buff,"+CMGR")))		/*check for +CMGR response */
	{
		UART0_SendString("No message");
	}
	else
	{
		buffer_pointer = 0;
		
		while(1)
		{
			if(buff[buffer_pointer]=='\r' || buff[buffer_pointer]== 'n')		/*wait till \r\n not over*/
			{
				buffer_pointer++;
			}
			else
			break;
		}
		
		/* search for 1st ',' to get mobile no.*/
		while(buff[buffer_pointer]!=',')
		{
			buffer_pointer++;
		}
		buffer_pointer = buffer_pointer+2;

		/* extract mobile no. of message sender */
		for(int i=0;i<=12;i++)
		{
			Mobile_no[i] = buff[buffer_pointer];
			buffer_pointer++;
		}
		
		do
		{
			buffer_pointer++;
		}while(buff[buffer_pointer-1]!= '\n');
		
		int i=0;

		/* display and save message */
		while(buff[buffer_pointer]!= '\r' && i<31)
		{
				UART0_TxChar(buff[buffer_pointer]);
				message_received[i]=buff[buffer_pointer];
				
				buffer_pointer++;
				i++;
		}
		
		buffer_pointer = 0;
		memset(buff,0,strlen(buff));
	}
	status_flag = false;
}

