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
	 * Voltage reference: 2.5 V - 0 v
	 * Clock: ADC120SC (5 MHz MODCLK)
	 * Conversion sequence: repeat single channel
	 * Conversion address: ADC12MEM0
	 * Input channel: A6
	 *********************************************************************************/
	REFCTLO0 &= ~REFMSTR;   /* Clear REFMSTR bit to hand control of ref to ADC12 */
    ADC12CTL0 &= ~(ADC12ENC+ADCSC)  /* clear to allow modification */
	ADC12CTL0 |= ADC12ON + ADC12REF2_5V + ADC12SHT0_2;
    ADC12CTL0 |= ADC12MSC;  /* first rising of SHI triggers repeating conversions */
    ADC12CTL1 |= ADC12CONSEQ_2 + ADC12SSEL_0 + ADC12SHP ADC12SHS_0;
    ADC12MCTL0 |= ADC12INCH_6 + ADC12REF_1;
    ADC12IE |= ADC12IE0;
    ADC12IFG &= ~ADC12IFG0;


    /*********************************************************************************
     * CONFIGURE TIMER TA1.1
     * Note: The PWM frequency needs to be at least 20 KHz to drive the motor.
     * Clock: SMCKL (1.048576 MHz)
     * TAR counts: 50
     * frequency_PWM: 20.9715 KHz
     * Output mode 7: reset/set
     *********************************************************************************/
    TA1CTL |= TAIE + TACLR + TASSEL_2;
    TA1CCTL0 |= CCIE + OUTMOD_7;
    TA1CCTL0 &= ~CCIFG;
    TA1CCR0 = 0x31;     /* CCR0 = 49 */


	return 0;
}
