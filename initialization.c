/**********************************************************************
*   File name: initialization.c
*   File owner: Tobias Madge
*   Most recent editor: Tobias Madge
*   This file is the shorthand functions for the hardware initialization of the ECD device
***********************************************************************/
#include <msp430.h>
#include <stdbool.h>

void generalInit (void){  // most of these can and should be sorted at some point
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;
    P1DIR |= 0x01;
    P1DIR &= ~0x02; // Set P1.1 to input direction (switch 2)
    P1REN |= 0x02; // Set P1.1 pull-up resistor enabled
    P4DIR |= 0x40; // Set P4.0 to output direction

}

void timerInit (void){
    TA0CCR0 = 1024; // Count up to 1024
    TA0CCTL0 = 0x10; // Enable counter interrupts, bit 4=1
    TA0CTL = TASSEL_2 + MC_1; // Timer A using subsystem master clock, SMCLK(1.1 MHz)       // and count UP to create a 1ms interrupt
    _BIS_SR(GIE); // interrupts enabled

}

void ledInit (void){
    P1OUT &= ~0x01; // Set P1.0 off (Green LED)
}

void lcdInit (void){
         P4DIR |= 0x04; // Set P4.2 to output direction (LCD Power On)
         P4DIR |= 0x08; // Set P4.3 to output direction (LCD Enable)
}

void spiInit (){ // most of these can and should be sorted at some point
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
}


void buttonInit (){

}

void sensorInit (){

}

void fullInit(){
    generalInit();
    timerInit();
    ledInit();
    lcdInit();
    spiInit();
}

