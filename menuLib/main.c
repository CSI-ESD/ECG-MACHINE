#include <msp430.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "menuSimple.h"
#include "timerSimple.h"
#include "displayLib.h"
#include "buttons.h"
/*
 * main.c
 */

// ghost variables (in an ideal world this would be in its own header file but I am lazy)
int redLED1 = 0;
int greenLED1 = 0;
int button1 = 0;
int heartbeat = 1;

#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer0_A0 (void)   // Timer0 A0 1ms interrupt service routine
{
        /* Toggle the red LED 500ms on and 500ms off */

    heartbeat++;
    if (heartbeat >= 500)
        {
            // P4OUT ^= 0x40;            /* Toggle P4.6 (red LED) using exclusive-OR      OLD ALTERNATIVE METHOD */
               heartbeat = 1;
               if (redLED1 == 1)
               {
               redLED1 = 0;
               }
               else
               {
               redLED1 = 1;
               }
        }
}

/*
 * Hi-level display state variable
 *
 * NOTE: This should be in displayLib but prior to merge I will keep it in my own files.
 *
 * STARTUP is after booting on for the first time, it begins
 * this state and cannot return to it.
 * IDLE is the state when the user is in the ECG screen but
 * has not begun a test. E.g. for calibrating axes or BPM Font
 * MENUSCREEN is as the name suggests when the user is in the
 * menuscreen, this is the default state the system enters into
 * once it leaves STARTUP
 * INTEST is the state the system enters into when the user has
 * begun a test.
 */
typedef enum{
    STARTUP=0,
    IDLE=1,
    MENUSCREEN=2,
    INTEST=3,
    ERROR=4
}DisplayState;

/* This should only change once we get into the menu */
DisplayState currDisplayState = STARTUP;

/*
 * This LCD Handler uses cases to build the scene and the default case
 * to output it to the display buffer. Therefore only insert a break in
 * a case where you don't want it to output to the displayBuffer. For example
 * when you might need to change a config under the hood but don't need to update
 * the display. This also allows the user to create secondary cases which the switch
 * statement can fall into should something need to be done before writing to display
 */
void LCDHandler(enum Buttons buttonPress)
{
    /*
     * Save the previous state in case the statechange is necessary as in the case
     * of MENUSCREEN
     */
    DisplayState prevDisplayState = currDisplayState;


    switch (currDisplayState)
    {
    case STARTUP:
        /* First time calling LCD handler so init & display welcome message
         * & Wait few secs before going to menu screen. Welcome message
         * Could tell the user what buttons do what & could wait until
         * the user presses confirm to continue.
         */
        initDisplayBuffer(0xff);
        writeFormattedText( startupMessage1, strlen(startupMessage1), 0, 0, 12, false);
        outputDisplayBuffer();
        writeFormattedText( startupMessage2, strlen(startupMessage2), 0, 0, 12, false);
        delaySeconds( 5 );
        outputDisplayBuffer();
        delaySeconds( 5 );
        writeFormattedText( helpMessageButtons, strlen(helpMessageButtons), 0, 0, 12, false);
        outputDisplayBuffer();
        delaySeconds( 5 );
        /* Then go to menu case after leaving message on-screen for a few seconds*/
        currDisplayState = MENUSCREEN;
        /* don't break so we fall into next chosen Case*/

    case IDLE:
        /* Display ECG Axes & BPM element with no values */


    case MENUSCREEN:
        if ( MENUSCREEN != prevDisplayState )
        {   /*Initalise Display Buffer */
            initDisplayBuffer(0x00);
            /*Initialise menu & hand control back to main */
            initialiseMenuBoxes(3);
        }
        else
        {   /* Otherwise just send button press to Menu Handler */
            //updateMenuBoxes( int buttonPress );
        }


    case INTEST:
        /* TODO Implement INTEST CASE*/

    case ERROR:
        /* TODO Implement ERROR CASE */

    default:
        outputDisplayBuffer();
        break;
    }
}

void buttonSimulator( enum Buttons buttonPress, short numPresses)
{
    int u = 0;
    for (u = numPresses; u > 0 ; --u)
    {
        updateMenuBoxes( buttonPress );
        outputDisplayBuffer();
    }

}

int main(void) {

    // Initialisation

    WDTCTL = WDTPW | WDTHOLD;    // Stop watchdog timer

    PM5CTL0 &= ~LOCKLPM5;       //Disable the power-on high-impedance mode

    //inputs
    P1DIR &= ~0x02;         // Setting P1.1 to input (switch2)
    P1REN |= 0x02;          // Set P1.1 resistor to be pull up enabled


    //outputs

    P1DIR |= 0x01;              //Set P1.0 to output
    P4DIR |=0x40;               //setting P4.0 to output

    //other
    TA0CCR0 = 1024;            // Count up to 1024
    TA0CCTL0 = 0x10;            // Enable counter interrupts, bit 4=1
    TA0CTL =  TASSEL_2 + MC_1;  // Timer A using subsystem master clock, SMCLK(1.1 MHz)
                                // and count UP to create a 1ms interrupt

    // LCD
     P4DIR |= 0x04; // Set P4.2 to output direction (LCD Power On)
     P4DIR |= 0x08; // Set P4.3 to output direction (LCD Enable)
     // SPI Ports
     P1SEL0 &= ~0x40; // Set P1.6 to output direction (SPI MOSI)
     P1SEL1 |= 0x40; // Set P1.6 to output direction (SPI MOSI)
     P1DIR |= 0x40; // Set P1.6 to output direction (SPI MOSI)
     P2SEL0 &= ~0x04; // Set P2.2 to SPI mode (SPI CLK)
     P2SEL1 |= 0x04; // Set P2.2 to SPI mode (SPI CLK)
     P2DIR |= 0x04; // Set P2.2 to output direction (SPI CLK)
     P2DIR |= 0x10; // Set P2.4 to output direction (SPI CS)
     // SPI Interface
     UCB0CTLW0 |= UCSWRST;
     UCB0CTLW0 &= ~(UCCKPH + UCCKPL + UC7BIT + UCMSB);
     UCB0CTLW0 &= ~(UCSSEL_3);
     UCB0CTLW0 |= UCSSEL__SMCLK;
     UCB0BRW = 8;
     UCB0CTLW0 |= (UCMSB + UCCKPH + 0x00 + UCMST + UCSYNC + UCMODE_0);
     UCB0CTLW0 &= ~(UCSWRST);
     P4OUT |= 0x04; // Turn LCD Power On
     P4OUT |= 0x08; // Enable LCD
     P1OUT &= ~0x01; // Set P1.0 off (Green LED)

    _BIS_SR(GIE);               // interrupts enabled

    // mains variable declarations

    volatile unsigned int i;     //volatile to prevent the code composer trying to optmize it and create issues


    initDisplayBuffer(0xff);
    outputDisplayBuffer();
    while (1)
    {
        //Super ghetto mini unit-test suite
        writeFormattedText(startupMessage1, strlen(startupMessage1), 0, 0, 12, true); // WORKS
        outputDisplayBuffer();
        initDisplayBuffer(0xff);
        writeFormattedText(startupMessage2, strlen(startupMessage2), 0, 0, 12, true); // WORKS
        outputDisplayBuffer();
        initDisplayBuffer(0xff);
        writeFormattedText(helpMessageButtons, strlen(helpMessageButtons), 0, 0, 12, true); // WORKS
        outputDisplayBuffer();
        initDisplayBuffer(0xff);
        writeFormattedText(helpMessageButtons2, strlen(helpMessageButtons2), 0, 0, 12, true); // WORKS
        outputDisplayBuffer();

        initDisplayBuffer(0xff);
        writeTextBoxes(2);  //WORKS
        populateTextBox(t3MenuSubOptions1[0], 2, 1, true);
        writeUniqueChar(39, 10, 6, true);
        writeUniqueChar(40, 10, 15, true);
        populateTextBox(t3MenuSubOptions1[1], 2, 2, false);
        outputDisplayBuffer();

        for (i = 1; i <= 3; i++)
        {
            initDisplayBuffer(0xff);
            writeTextBoxes(i);  //WORKS
            outputDisplayBuffer();
        }

        initialiseMenuBoxes(3); //WORKS
        outputDisplayBuffer();
        buttonSimulator( Button2, 4 ); // Cyclic Scroll
        buttonSimulator( Button1, 2 ); // Confirm on Test duration & again to configure it
        buttonSimulator( Button2, 3 ); // Decrement duration to 1m then try again to decrement
        buttonSimulator( Button1, 5 ); // Increment to 5m then try again to increment


    }


    return 0;
}
