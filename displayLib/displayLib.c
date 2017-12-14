/**********************************************************************
*   File name: displayLib.c
*   File owner: Alexandru Constantin
*   Most recent editor: Alexandru Constantin
*   This is the implementation file for displayLib.h
***********************************************************************/
#include <msp430.h>
#include <ctype.h>
#include <stdbool.h>
#include "displayLib.h"

/**********************************************************************
*   Function name: writeText
*   This function writes an array of chars with small space in between
*   in order to improve readability
***********************************************************************/
    void writeText(char text[], int size, int positionX,int positionY, bool inverted){
         int i=0;
         for(i=0; i<size; i++){
             writeChar(text[i], positionX, positionY, inverted);
             positionX++;
         }
     }

/**********************************************************************
*   Function name: writeChar
*   This function writes a char on the screen at the specified position
*	Input type: char
***********************************************************************/
    void writeChar(char c, int positionX,int positionY){
		int i=0;
		if(isalpha(c)){
			c = toupper(c);
		}
        int chIndex = charToFontIndex(c);
		writeCharFromIndex(chIndex,positionX,positionY,inverted);
     }

/**********************************************************************
*   Function name: writeCharFromIndex
*   This function writes a char on the screen at the specified position
*	Input type: int    
***********************************************************************/
	void writeCharFromIndex(int charIndex, int positionX,int positionY, bool inverted){
         int i=0;
         unsigned char *toBeWritten = getChar(charIndex,1);
         for (i=0; i<8; i++){
         	if(!inverted){
				displayBuffer[positionX][i + positionY] = ~toBeWritten[i];
            }else{
				displayBuffer[positionX][i + positionY] = toBeWritten[i];
            }
		 }
	}

/**********************************************************************
*   Function name: getChar
*   This function is used to fetch characters from the font tables
***********************************************************************/
    unsigned char *getChar(int chIndex, int size){
        switch(size){
            case 1:; //font size one
            int i = 0;
            for(i=0;i<8;i++){
                result[i] = sizeOneFont[chIndex][i];
            }
            return result;
            //rest of the cases to be implemented
        }
        return 0;
    }

/**********************************************************************
*   Function name: charToFontIndex
*   This function is used to convert the char into an index number which
*   can be then used to fetch the hex values from the font table
***********************************************************************/
    int charToFontIndex(char character){
        const static struct{
                int input_character;
                int output_character;
            }
            conversion[] = {
            { (int)'0', 0 },
            { (int)'1', 1 },
            { (int)'2', 2 },
            { (int)'3', 3 },
            { (int)'4', 4 },
            { (int)'5', 5 },
            { (int)'6', 6 },
            { (int)'7', 7 },
            { (int)'8', 8 },
            { (int)'9', 9 },
            { (int)'A', 10 },
            { (int)'B', 11 },
            { (int)'C', 12 },
            { (int)'D', 13 },
            { (int)'E', 14 },
            { (int)'F', 15 },
            { (int)'G', 16 },
            { (int)'H', 17 },
            { (int)'I', 18 },
            { (int)'J', 19 },
            { (int)'K', 20 },
            { (int)'L', 21 },
            { (int)'M', 22 },
            { (int)'N', 23 },
            { (int)'O', 24 },
            { (int)'P', 25 },
            { (int)'Q', 26 },
            { (int)'R', 27 },
            { (int)'S', 28 },
            { (int)'T', 29 },
            { (int)'U', 30 },
            { (int)'V', 31 },
            { (int)'W', 32 },
            { (int)'X', 33 },
            { (int)'Y', 34 },
            { (int)'Z', 35 },
            { (int)':', 36 },
            { (int)'.', 37 },
            { (int)' ', 38 }
            };
            int i;

            for (i = 0; i < sizeof(conversion) / sizeof(conversion[0]); i++){
                if (conversion[i].input_character == (int)character){
                    return conversion[i].output_character;
                }
            }
            return -1;
        }

/**********************************************************************
*   Function name: reverse
*   This function is used to revert 8 bit values. As an example, taking
*   0x01 as input, the function will return 0x80. It is used by the
*   lcdHandler to skip form last column to next line first column.
***********************************************************************/
    char reverse(char inchar){
        char outchar = 0;
        if ((inchar & BIT0) > 0) outchar |= BIT7;
        if ((inchar & BIT1) > 0) outchar |= BIT6;
        if ((inchar & BIT2) > 0) outchar |= BIT5;
        if ((inchar & BIT3) > 0) outchar |= BIT4;
        if ((inchar & BIT4) > 0) outchar |= BIT3;
        if ((inchar & BIT5) > 0) outchar |= BIT2;
        if ((inchar & BIT6) > 0) outchar |= BIT1;
        if ((inchar & BIT7) > 0) outchar |= BIT0;
        return outchar;
    }

/**********************************************************************
*   Function name: initDisplayBuffer
*   Function used to initialize the display buffer it will insert the
*   value supplied as input throughout the displayBuffer array
***********************************************************************/
    void initDisplayBuffer(char setting){
        int i,j;
        for (i=0; i<96; i++){
            for(j=0; j<12; j++){
                displayBuffer[i][j] = setting;
            }
        }
    }

/**********************************************************************
*   Function name: outputDisplayBuffer
*   Function used to output to the display buffer
***********************************************************************/
    void outputDisplayBuffer(){
        int line,column;
        char command = 0x80;            // Write lines

        command = command ^ 0x40;       // VCOM bit
        P2OUT |= 0x10;                  // LCD CS high

        while (!(UCB0IFG & UCTXIFG));

        UCB0TXBUF = command;

        for (line=0; line<96; line++){
            while (!(UCB0IFG & UCTXIFG));

            UCB0TXBUF = reverse(line+1);

            for (column=0; column<12; column++){
                while (!(UCB0IFG & UCTXIFG));

                UCB0TXBUF = displayBuffer[column][line];
            }

            while (!(UCB0IFG & UCTXIFG));

            UCB0TXBUF = 0x00;            // End of line signal
        }
        while (!(UCB0IFG & UCTXIFG));

        UCB0TXBUF = 0x00;               // End of block signal

        while ((UCB0STATW & UCBBUSY));

        __delay_cycles(8);              //Ensure a 2us min delay to meet the LCD's thSCS

        P2OUT &= ~0x10;                 // LCD CS low
    }

/**********************************************************************
*   Function name: displayScene
*   Function used to display the pre-built scene from the display buffer
***********************************************************************/
    void displayScene(){
        if (UCB0IFG & UCTXIFG){
            if (line == 0){
                P2OUT |= 0x10;                  // Enable LCD CS
                UCB0TXBUF = 0x80;
                column = 0;
                line++;
            }
            else if ((line >= 1) && (line <= 96)){
                if (column == 0){
                    UCB0TXBUF = reverse(line);
                    column++;
                }
                else if ((column >= 1) && (column <= 12)){
                    UCB0TXBUF =  displayBuffer[line-1][column-1];
                    column++;
                }
                else{
                    UCB0TXBUF = 0x00;
                    column = 0;
                    line++;
                }
            }
            else if (line == 97){
                UCB0TXBUF = 0x00;
                line++;
            }
            else if (line == 98){
                line++;
            }
            else{
                if ((UCB0STATW & UCBBUSY) == 0){
                    //Ensure a 2us min delay to meet the LCD's thSCS
                    //__delay_cycles(16);
                    line = 0;
                    P2OUT &= ~0x10;                 // Disable LCD CS
                }
            }
        }
    }
