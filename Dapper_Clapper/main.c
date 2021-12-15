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
 *  - pins 2.2, 1.5, 1.4, 1.3, 1.2 -> LEDs
 *  - P2.0 -> TA1.1
 ************************************************************************************/
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	/*********************************************************************************
	 * CONFIGURE I/O
	 *********************************************************************************/
	P1SEL &= ~(BIT5+BIT4+BIT3+BIT2);        /* GPIO */
	P2SEL &= ~BIT2;
	P6SEL |= BIT6;                          /* ADC special function */
	P2SEL |= BIT0;                          /* TA1.1 special function */
	P2DIR |= BIT0;                          /* PWM output */
//	P2DS |= BIT0;                       /* full output drive strength */
	P1DIR |= BIT5+BIT4+BIT3+BIT2;      /* configure ports as output */
	P2DIR |= BIT2;
    P1OUT &= ~(BIT4 + BIT3 + BIT2);
    P1OUT |= BIT5;
    P2OUT &= ~BIT2;
    P2DS |= BIT0;           /* Full drive strength for PWM output */

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
    TA1CTL |= TASSEL_2;
    TA1CCTL0 &= ~CCIFG;
    TA1CCR0 = 20;     /* CCR0 = 49 */
    TA1CCTL1 |= OUTMOD_7;
    TA1CCR1 = 0;

    /*********************************************************************************
     * START THE ADC AND TIMER
     *********************************************************************************/
    ADC12CTL0 |= ADC12ENC;  /* enable ADC12 conversion */
    TA1CTL |= MC_1 + TACLR;         /* start the timer */
    ADC12CTL0 |= ADC12SC;

    __bis_SR_register(GIE); /* enable GIE to allow interrupts. */
    __no_operation();         /* set breakpoint for debugging */

    while(1);
}

/*********************************************************************************
 * ADC12 INTERRUPT SERVICE ROUTINE
 * Cutoff voltage:  1 V, NADC = 0x0AAA
 *********************************************************************************/
#pragma vector = ADC12_VECTOR
__interrupt void ADC12_ISR(void)
{
    /* Make clapping work first, then implement preclap if there is time */
    if (ADC12MEM0 >= 0x0AAA)
    {
        P2OUT |= BIT2;
        volatile unsigned int j;
//        j = 0xFFFF;
//        do j--;
//        while (j != 0);


        switch(TA1CCR1)
        {
        case 0:
            /* MODE 1 (LOW POWER) */
            P1OUT |= BIT4;              /* configure LEDs */
            P1OUT &= ~BIT3;
            P1OUT &= ~BIT2;
            P1OUT &= ~BIT5;
            TA1CCR1 = 4;               /* 32 % duty cycle */
            /* delay for stability */
            j = 0xFFFF;
            do j--;
            while (j != 0);

            break;
        case 4:
            /* MODE 2 (FULL POWAHHHH!!) */
             P1OUT &= ~BIT4;              /* configure LEDs */
             P1OUT |= BIT3;
             P1OUT &= ~BIT2;
             P1OUT &= ~BIT5;
             TA1CCR1 = 5;               /* 64 % duty cycle */
             /* delay for stability */
             j = 0xFFFF;
             do j--;
             while (j != 0);
             break;
        case 5:
            /* MODE 3 (THAT WASN'T EVEN MY FINAL FORM!!!) */
             P1OUT &= ~BIT4;              /* configure LEDs */
             P1OUT &= ~BIT3;
             P1OUT |= BIT2;
             P1OUT &= ~BIT5;
             TA1CCR1 = 6;               /* 99.9 % duty cycle */
             /* delay for stability */
             j = 0xFFFF;
             do j--;
             while (j != 0);
             break;
        case 6:
            /* MODE 0 (HUMAN MODE) */
             P1OUT &= ~BIT4;              /* configure LEDs */
             P1OUT &= ~BIT3;
             P1OUT &= ~BIT2;
             P1OUT |= BIT5;
             TA1CCR1 = 0;               /* 0 % duty cycle */
             /* delay for stability */
             j = 0xFFFF;
             do j--;
             while (j != 0);
             break;
        default:
            /* MODE 0 (HUMAN MODE) */
             P1OUT &= ~BIT4;              /* configure LEDs */
             P1OUT &= ~BIT3;
             P1OUT &= ~BIT2;
             P1OUT |= BIT5;
             TA1CCR1 = 0;               /* 0 % duty cycle */
             /* delay for stability */
             j = 0xFFFF;
             do j--;
             while (j != 0);
             break;
         }
   }
    else
    {
        P2OUT &= ~BIT2;
        ADC12IFG &= ~ADC12IFG0;
    }
}
