/**********************************************************************
*   File name: menuSimple.h
*   File owner: Luke Parsons
*   Most recent editor: Luke Parsons
*
*   This is the header file declarations for data and functions
*   defined in menuSimple.c These are used in all operations relating
*   to the MENU displayState.
*
*   Disclosure: This is an initial version of the menu system which
 *  does not reflect the design specified in the low level design
 *  document for the User interface. It is a simpler, more hardcoded
 *  version that is meant to emulate what the system would look like.
 *
 *  Created on: 23 Nov 2017
 *
*   Version number: 0.3
*
*   major changes
***********************************************************************/
#include "buttons.h"

#ifndef MENUSIMPLE_H_
#define MENUSIMPLE_H_



/*******************************************
 *          MENU DECLARATIONS
 ******************************************/

#define MAX_PIXELS_VERT 96
#define MAX_PIXELS_HORIZ 96

#define MAX_MENU_BUTTON_PRESS_QUEUE_SIZE 30

/*
 * used primarily by writeLineOfChars though
 * works as a pretty good abstraction in any context
 * */
typedef enum{
    VERTICAL = 0,
    HORIZONTAL = 1
}LineDirection;

#define MAX_BOXES_ONSCREEN 3
#define MIN_BOXES_ONSCREEN 1

/* Size in bytes */
#define CHARFONT_SIZE_VERTICAL 8
#define CHARFONT_SIZE_HORIZONTAL 1

/*
 * refer to low level design for UI
 * Usable Character Area: values apply per box. E.g
 * type 1 x 117 = 117
 * type 2 x 50 = 100
 * type 3 x 30 = 90
 */
#define TYPE1_MAX_UCA 117
#define TYPE2_MAX_UCA 50
#define TYPE3_MAX_UCA 30

/*
 * named as t[x][direction]Border
 * where x = thickness(t) of border between 1-3 pixels
 * direction = left | right
 */
extern const short t1Thickness;
extern const char t1LeftBorder;
extern const char t1RightBorder;

extern const short t2Thickness;
extern const char t2LeftBorder;
extern const char t2RightBorder;

extern const short t3Thickness;
extern const char t3LeftBorder;
extern const char t3RightBorder;

/*
 * technically only need one of these for the upper & lower border
 * but it could be subject to change in the future for some reason.
 */
extern const char topBorder;
extern const char bottomBorder;

/*******************************************
 *       STARTUP/WELCOME MESSAGE
 ******************************************/

#define MAX_STARTUP_MSG_CHARS 144
extern const unsigned char startupMessage1[MAX_STARTUP_MSG_CHARS];

extern const unsigned char startupMessage2[MAX_STARTUP_MSG_CHARS];

extern const unsigned char helpMessageButtons[MAX_STARTUP_MSG_CHARS];

extern const unsigned char helpMessageButtons2[MAX_STARTUP_MSG_CHARS];

/*******************************************
 *     MENU OPTION TEXT DECLARATIONS
 ******************************************/

/*
 * type 3 menuText
 */
extern const char t3MenuText[4][TYPE3_MAX_UCA];

/*
 * These are the menu Sub-options for t3MenuText option number 1
 * This is just an example, but in the future there would be more of these,
 * held within a more advanced container which allows integer/float values for
 * options to be maintained within. For now, just text to use as an example.
 */
extern const char t3MenuSubOptions1[2][TYPE3_MAX_UCA];
extern const char t2MenuText[3][TYPE2_MAX_UCA];
extern const char t1MenuText[2][TYPE1_MAX_UCA];


/*******************************************
 *         FUNCTION PROTOTYPES
 *
 *  Function descriptions included in both
 *  .h and .c files to accommodate various
 *                  IDE's
 ******************************************/

/**********************************************************************
 * Function name: writeLineOfChars
 *
 * Writes a line of chars (charToWrite) VERTICAL(0) or HORIZONTAL(1)
 * as dictated by the enum LineDirection at position (1-96).
 ***********************************************************************/
void writeLineOfChars(char charToWrite, short position, LineDirection lineDir);

/**********************************************************************
 * NOTE: Belongs in DisplayLib though leaving it here prior to merge.
 *
 * Function name: writeFormattedText
 *
 * This function takes inText and writes it to the display starting at positionX/Y and
 * accounts for line & word wrapping. note that it will always reset the cursor back to
 * the starting positionX on wrapping using maxCharsPerLine to determine how many characters
 * it can write before it wraps.
 ***********************************************************************/
void writeFormattedText(const char *text, int size, int positionX,int positionY, short maxCharsPerLine, bool inverted);

/*
 * writes the Horizontal borders of the menu text boxes given
 * type = { 1, 2, 3 } where type is the number of boxes on-screen
 */
void writeHorizontalBorders( short type );

/*
 * writes the Vertical borders of the menu text boxes given
 * type = { 1, 2, 3 } where type is the number of boxes on-screen
 */
int writeVerticalBorders( short type );

/*
 * writes the menu text boxes given;
 * type = { 1, 2, 3 } where type is the number of boxes on-screen
 */
int writeTextBoxes( short type );

/*
 * Populates the boxNum with the MenuText. Type is required
 * in order to check for text that is too long to fit inside a single box
 * and would otherwise overwrite other pixels.
 */
int populateTextBox(const char *menuText, short type, short boxNum, bool inverted);

/*
 * Function to be called on startup of ECG Machine
 * and whenever entering menu from another displayState.
 * Otherwise updateMenuBoxes() should be called in the LCD handler
 */
void initialiseMenuBoxes( int type );

/*
 * function called whenever already in displayState MENUBOX
 * and a button press has occurred which the screen needs to be updated for.
 */
int updateMenuBoxes( enum Buttons buttonPress );


#endif /* MENUSIMPLE_H_ */
