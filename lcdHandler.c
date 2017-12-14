/*
 * lcdHandler.c
 *
 *  Created on: 13 Dec 2017
 *      Author: m3-alden
 */

#include <msp430.h>
#include "lcdHandler.h"
#include "font8x8_basic.h"

char DisplayBuffer[96][96 / 8];
char reverse(char);

void initDisplay(void) {
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

    initDisplayBuffer(' ');
}

void initDisplayBuffer(char setting) {
    int i;
    int j, k;
    char ch = setting - ' ';
    for (k = 0; k < 12; k++) {
        for (i = 0; i < 12; i++) {
            for (j = 0; j < 8; j++) {
                DisplayBuffer[(k*8)+j][i] = reverse(font8x8_basic[ch][j]);
            }
        }
    }
}

void setText(char x, char y, char * str) {
    int i, j;
    char ch;
    j = 0;
    while (*str != '\0') {
        ch = (*str++) - ' ';
        for (i = 0; i < 8; i++) {
            DisplayBuffer[y + i][x / 8 + j] = reverse(font8x8_basic[ch][i]);
        }
        j += 1; //move to bext char position
    }
}

char reverse(char inchar) {
    char outchar;

    outchar = 0;

    if ((inchar & BIT0) > 0)
        outchar |= BIT7;
    if ((inchar & BIT1) > 0)
        outchar |= BIT6;
    if ((inchar & BIT2) > 0)
        outchar |= BIT5;
    if ((inchar & BIT3) > 0)
        outchar |= BIT4;
    if ((inchar & BIT4) > 0)
        outchar |= BIT3;
    if ((inchar & BIT5) > 0)
        outchar |= BIT2;
    if ((inchar & BIT6) > 0)
        outchar |= BIT1;
    if ((inchar & BIT7) > 0)
        outchar |= BIT0;

    return outchar;
}

void outputDisplayBuffer(char lo, char lh) {
    int line;
    int column;
    char command = 0x80;            // Write lines

    command = command ^ 0x40;       // VCOM bit

    P2OUT |= 0x10;                  // LCD CS high

    while (!(UCB0IFG & UCTXIFG))
        ;
    UCB0TXBUF = command;

    for (line = lo; line < lo+lh; line++) {
        while (!(UCB0IFG & UCTXIFG))
            ;
        UCB0TXBUF = reverse(line + 1);

        for (column = 0; column < 12; column++) {
            while (!(UCB0IFG & UCTXIFG))
                ;
            UCB0TXBUF = DisplayBuffer[line][column];
        }

        while (!(UCB0IFG & UCTXIFG))
            ;
        UCB0TXBUF = 0x00;            // End of line signal
    }

    while (!(UCB0IFG & UCTXIFG))
        ;
    UCB0TXBUF = 0x00;               // End of block signal

    while ((UCB0STATW & UCBBUSY))
        ;

    __delay_cycles(8);          //Ensure a 2us min delay to meet the LCD's thSCS

    P2OUT &= ~0x10;                 // LCD CS low
}

