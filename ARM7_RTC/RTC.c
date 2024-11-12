

#include <lpc214x.h>
#include <stdint.h>
#include <stdio.h>
#include "UART0.h"
#include "lcd.h"
uint8_t alarm, flag;

#define rs 16
#define en 17

#define d4 18
#define d5 19
#define d6 20
#define d7 21

__irq void RTC_ISR(void)
{
	if (ILR & 0x01)
	{
		flag = 1;
		ILR = ILR | 0x01;
	}
	if (ILR & 0x02)
	{
		alarm = 1;
		ILR = ILR | 0x02;
	}
	VICVectAddr = 0;
}

typedef struct
{
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	uint8_t day_of_month;
	uint8_t day_of_week;
	uint16_t day_of_year;
	uint8_t month;
	uint16_t year;
}RTC_Time;

void RTC_Set_Time( RTC_Time Set_Time)
{
	SEC = Set_Time.seconds;
	MIN = Set_Time.minutes;
	HOUR = Set_Time.hours;
	DOM = Set_Time.day_of_month;
	DOW = Set_Time.day_of_week;
	DOY = Set_Time.day_of_year;
	MONTH = Set_Time.month;
	YEAR = Set_Time.year;
}

void RTC_Set_Alarm_Time( RTC_Time Alarm_Time)
{
	ALSEC = Alarm_Time.seconds;
	ALMIN = Alarm_Time.minutes;
	ALHOUR = Alarm_Time.hours;
	ALDOM = Alarm_Time.day_of_month;
	ALDOW = Alarm_Time.day_of_week;
	ALDOY = Alarm_Time.day_of_year;
	ALMON = Alarm_Time.month;
	ALYEAR = Alarm_Time.year;
}

RTC_Time RTC_Get_Time(void)
{
	RTC_Time time;
	time.seconds = SEC;
	time.minutes = MIN;
	time.hours = HOUR;
	time.day_of_month = DOM;
	time.day_of_week = DOW;
	time.day_of_year = DOY;
	time.month = MONTH;
	time.year = YEAR;
	return time;
}

int main(void)
{
	IO1DIR = (1 << en) | (1 << rs) | (1 << d4) | (1 << d5) | (1 << d6) | (1 << d7); //lcd set
    	/* Setting Time + Alarm */
	RTC_Time set_time, alarm_time, current_time;
	char timestr[30], datestr[30];
	alarm = 0;
	flag = 0;
	init();
	IO0DIR = 0x00000010; /* P0.4 as output pin for LED */
	UART0_init();
	PCONP = (PCONP | (1<<9)); /* PCRTC = 1 */
	/* The RTC registers cannot be written to unless we make PCRTC = 1 */
	ILR = 0x0; /* No RTC interrupts */
	CCR = 0x12; /* 32.768kHz clock and Reset Clock Tick Counter */
	CCR = 0x10; 
	CIIR = 0x00; /* No interrupts */
	AMR = 0x00; /* Alarm registers not masked */
	VICVectAddr0 = (unsigned) RTC_ISR;
	VICVectCntl0 = 0x0000002D;
	VICIntEnable = 0x00002000;
	VICIntSelect = 0x00000000;
	set_time.seconds = 00;
	set_time.minutes = 25;
	set_time.hours = 11;
	set_time.day_of_month = 6;
	set_time.day_of_week = 5;
	set_time.day_of_year = 279;
	set_time.month = 10;
	set_time.year = 2017;
	RTC_Set_Time(set_time);
	CIIR = 0x01; /* Seconds value increment interrupt */
	alarm_time.seconds = 15;
	alarm_time.minutes = 25;
	alarm_time.hours = 11;
	alarm_time.day_of_month = 6;
	alarm_time.day_of_week = 5;
	alarm_time.day_of_year = 279;
	alarm_time.month = 10;
	alarm_time.year = 2017;
	RTC_Set_Alarm_Time(alarm_time);	
	CCR = 0x11; /* 32.768kHz clock and clock Enable */
	ILR = 0x03; /* RTC interrupts enabled */
	IO0CLR = 0x00000010;	

	while(1)
	{
		if(alarm == 1)
		{
			current_time = RTC_Get_Time();
			sprintf(timestr,"Alarm!!!: %d:%d:%d \r\n",current_time.hours,current_time.minutes,current_time.seconds);
			UART0_SendString(timestr);
			disp(0x80, (unsigned char*)timestr);
			uint8_t i;
			for(i=0;i<10;i++)
			{
				IO0SET = 0x00000010;
				delay_ms(600);
				IO0CLR = 0x00000010;
				delay_ms(600);
			}
			alarm = 0;
		}
		if (flag == 1)
		{
			current_time = RTC_Get_Time();
			sprintf(timestr,"Time: %d:%d:%d ",current_time.hours,current_time.minutes,current_time.seconds);
			sprintf(datestr,"Date: %d/%d/%d \r\n",current_time.day_of_month,current_time.month,current_time.year);
			UART0_SendString(timestr);
			UART0_SendString(datestr);
			disp(0x80, (unsigned char*)timestr); 
      disp(0xC0, (unsigned char*)datestr); 
			flag = 0;
		}
	}
}


