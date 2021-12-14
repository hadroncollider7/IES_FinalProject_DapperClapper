#include <msp430.h> 

/* Global variables */
//volatile unsigned int preClap = 0; // make single clap work first

/************************************************************************************
 * IES Final Project: Dapper Clapper
 * Version: 12-05-21
 * Inputs:
 *  - 3V (battery supply) -> AVCC
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
	P6SEL |= BIT6;                          /* ADC special function */
	P2SEL |= BIT0;                          /* TA1.1 special function */
	P1DIR |= BIT7+BIT5+BIT4+BIT3+BIT2;      /* configure ports as output */

	/*********************************************************************************
	 * CONFIGURE ADC12
	 * Voltage reference: 1.5 V - 0 v
	 * Clock: ADC120SC (5 MHz MODCLK)
	 * Conversion sequence: repeat single channel
	 * Conversion address: ADC12MEM0
	 * Input channel: A6
	 *********************************************************************************/
	REFCTL0 &= ~REFMSTR;                /* Clear REFMSTR bit to hand control of ref to ADC12 */
    ADC12CTL0 &= ~(ADC12ENC+ADC12SC);   /* clear to allow modification */
	ADC12CTL0 |= ADC12ON + ADC12REFON + ADC12SHT0_2;
    ADC12CTL0 |= ADC12MSC;              /* first rising of SHI triggers repeating conversions */
    ADC12CTL1 |= ADC12CONSEQ_2 + ADC12SSEL_0 + ADC12SHP + ADC12SHS_0;
    ADC12MCTL0 |= ADC12INCH_6 + ADC12SREF_1;
    ADC12IE |= ADC12IE0;
    ADC12IFG &= ~ADC12IFG0;

    volatile unsigned int j;
    j = 0x30;
    do j--;
    while (j != 0);                     /* Delay for reference start-up */

    /*********************************************************************************
     * CONFIGURE TIMER TA1.1
     * Note: The PWM frequency needs to be at least 20 KHz to drive the motor.
     * Clock: SMCKL (1.048576 MHz)
     * TAR counts: 50
     * frequency_PWM: 20.9715 KHz
     * Output mode 7: reset/set
     * Make sure to start the fan OFF
     *********************************************************************************/
//    TA1CTL |= TAIE + TASSEL_2;
//    TA1CCTL0 |= CCIE + OUTMOD_7;
//    TA1CCTL0 &= ~CCIFG;
//    TA1CCR0 = 0x31;     /* CCR0 = 49 */
//    TA1CCR1 = 0x00;     /* CCR1 = 0. The fan is off */

    /*********************************************************************************
     * START THE ADC AND TIMER
     *********************************************************************************/
    ADC12CTL0 |= ADC12ENC;  /* enable ADC12 conversion */
//    TA0CTL |= TACLR;        /* Clears TAR and divider logic (divider settings unchanged. Automatically resets */
//    TA1CTL |= MC_1;         /* start the timer */

    P1OUT &= ~BIT4;
    ADC12CTL0 |= ADC12SC;

    __bis_SR_register(GIE); /* enable GIE to allow interrupts. */
    __no_operation();         /* set breakpoint for debugging */

    while(1);
}

/*********************************************************************************
 * ADC12 INTERRUPT SERVICE ROUTINE
 * Cutoff voltage: 80 mV, NADC = 0x83
 *********************************************************************************/
#pragma vector = ADC12_VECTOR
__interrupt void ADC12_ISR(void)
{
    /* Make clapping work first, then implement preclap if there is time */
    if (ADC12MEM0 >= 0x0AAA)
    {
        P1OUT ^= BIT4;      /* toggle mode 1 LED */
        //        TA1CCR1 = 44;
        volatile unsigned int j;
        j = 0xFFFF;
        do j--;
        while (j != 0);
    }
    else
    {
        ADC12IFG &= ~ADC12IFG0;
    }
}
