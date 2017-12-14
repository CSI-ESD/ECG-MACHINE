#include <msp430.h> 

#include "lcdHandler.h"
//#include "timer.h"
#include "ADC.h"
#include "stdio.h"
#include "string.h"
#include "stdbool.h"


int heartbeat = 1;
int adcReading = 1;

#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer0_A0(void) { // Timer0 A0 1ms interrupt service routine

    //Time to take a reading
    adcReading++;
    if (adcReading >= 40){
        ;
        adcReading = 1;
    }

    // Toggle the red LED 500ms on and 500ms off
    heartbeat++;
    if (heartbeat >= 500) {
        heartbeat = 1;
        P4OUT ^= 0x40;
    }
} //Timer

void initTimer(void)
{
    P4DIR &= ~0x40; //p4.6
    P4REN |= 0x40; // Set P1.1 pull-up resistor enabled
    P4OUT |= 0x40;
    P4DIR |= 0x40;  // Set P4.6 to output direction

    TA0CCR0 = 1023; // Count up to 1024
    TA0CCTL0 = 0x10; // Enable counter interrupts, bit 4=1
    TA0CCTL1 = OUTMOD_3;                      // TACCR1 set/reset
    TA0CCR1 = 1023;                           // TACCR1 PWM Duty Cycle
    TA0CTL = TASSEL_2 + MC_1; // Timer A using subsystem master clock, SMCLK(1.1 MHz)
                              // and count UP to create a 1ms interrupt
                              // PWM Period
}

void set_green_led( bool led_value )
{
    if( led_value )
    {
        P1OUT |= 0x01;        // Set P1.0 on using OR
    }
    else
    {
        P1OUT &= (0xFF-0x01); // Set P1.0 off using AND
    }
}

int main(void) {
    char adcRdg [5];
    char doneReading = 0;

    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5; // Disable the GPIO power-on default high-impedance mode

    initTimer();
    initDisplay();
    outputDisplayBuffer(0, 96);
    _BIS_SR (GIE);

    setText(0, 1, "Heartbeat ");
    outputDisplayBuffer(0,96);
    init_adc();
    while (1) {
        if (adcReading == 1){
            if (doneReading == 0){
                setText (0, 24, "    ");
                sprintf (adcRdg, "%d", rdg);
                setText(0, 24, adcRdg);
                setText(40, 24,"bits");
                outputDisplayBuffer(0, 96);

                doneReading = 1;
            }

        }else
            doneReading = 0;

    }

    return 0;
}
