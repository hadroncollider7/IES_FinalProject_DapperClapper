#include <msp430.h> 


/************************************************************************************
 * IES Final Project: Dapper Clapper
 * Version: 12-05-21
 * Inputs:
 *  - 3.3V (battery supply) -> AVCC
 *  - GND (battery supply) -> AVSS
 *  - Mic analog -> P6.6 (A6)
 * Outputs:
 *  - pins 1.7, 1.5, 1.4, 1.3, 1.2 -> LEDs
 *  - P2.0 -> TA1.1
 ************************************************************************************/
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	/*********************************************************************************
	 * CONFIGURE I/O
	 *********************************************************************************/
	P1SEL &= ~(BIT7+BIT5+BIT4+BIT3+BIT2);   /* GPIO */
	P6SEL |= BIT6;      /* ADC special function */
	P2SEL |= BIT0;      /* TA1.1 special function */
	P1DIR |= BIT7+BIT5+BIT4+BIT3+BIT2;  /* configure ports as output */

	/*********************************************************************************
	 * CONFIGURE ADC12
	 *********************************************************************************/
	REFCTLO0 &= ~REFMSTR;   /* Clear REFMSTR bit to hand control of ref to ADC12 */
    ADC12CTL0 &= ~(ADC12ENC+ADCSC)  /* clear to allow modification */
	ADC12CTL0 |= ADC12ON + ADC12REF2_5V + ADC12SHT0_2;
    ADC12CTL0 |= ADC12MSC;  /* first rising of SHI triggers repeating conversions */

    /*********************************************************************************
     * CONFIGURE TIMER TA1.1
     *********************************************************************************/



	return 0;
}
