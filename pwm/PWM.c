#include <lpc214x.h>
#include <stdint.h>

void delay_ms(uint16_t j)
{
    uint16_t x,i;
	for(i=0;i<j;i++)
	{
    for(x=0; x<6000; x++);    /* loop to generate 1 milisecond delay with Cclk = 60MHz */
	}
}

__irq void PWM_ISR (void)
{
	if ( PWMIR & 0x0001 )	/* If interrupt due to PWM0 */
	{
		PWMIR = 0x0001;	/* Clear PWM0 interrupt */
	}
	
	if ( PWMIR & 0x0002 )	/* If interrupt due to PWM1 */
	{
		PWMIR = 0x0002;	/* Clear PWM1 interrupt */
	}
	
	if ( PWMIR & 0x0004 )	/* If interrupt due to PWM2 */
	{
		PWMIR = 0x0004;	/* Clear PWM2 interrupt */
	}
	
	if ( PWMIR & 0x0008 )	/* If interrupt due to PWM3 */
	{
		PWMIR = 0x0008;	/* Clear PWM3 interrupt */
	}	
	VICVectAddr = 0x00000000;
}


int main (void)
{
	uint32_t value;
	value = 1;
	VPBDIV		= 0x00000002;
	PINSEL0 = PINSEL0 | 0x00000008; /* Configure P0.1 as PWM3 */
	VICVectAddr0 = (unsigned) PWM_ISR; /* PWM ISR Address */
	VICVectCntl0 = (0x00000020 | 8); /* Enable PWM IRQ slot */
	VICIntEnable = VICIntEnable | 0x00000100; /* Enable PWM interrupt */
	VICIntSelect = VICIntSelect | 0x00000000; /* PWM configured as IRQ */

// For single edge controlled PWM3
	PWMTCR = 0x02; 	/* Reset and disable counter for PWM */
	PWMPR = 0x1D;	/* Prescale value for 1usec, Pclk=30MHz*/
	PWMMR0 = 1000;	/* Time period of PWM wave, 1msec */
	PWMMR3 = value;	/* Ton of PWM wave */
	PWMMCR = 0x00000203;	/* Reset and interrupt on MR0 match, interrupt on MR3 match */
	PWMLER = 0x09;	/* Latch enable for PWM3 and PWM0 */
	PWMPCR = 0x0800;	/* Enable PWM3 and PWM0, single edge controlled PWM on PWM3 */
	PWMTCR = 0x09;	/* Enable PWM and counter */

	while (1)
	{
		while (value != 999)
		{
			PWMMR3 = value;
			PWMLER = 0x08;
			delay_ms(5);
			value++;
		}
		while (value != 1)
		{
			PWMMR3 = value;
			PWMLER = 0x08;
			delay_ms(5);
			value--;
		}	
	}
}

