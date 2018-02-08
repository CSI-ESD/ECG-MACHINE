/**********************************************************************
*   File name: menuSimple.c
*   File owner: Luke Parsons
*   Most recent editor: Luke Parsons

 *  Created on: 23 Nov 2017
***********************************************************************/

#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include "displayLib.h"
#include "menuSimple.h"


/*******************************************
 *       DATA DEFINITIONS
 ******************************************/

/*
 * This is for updateMenuBoxes so it can keep track of the button presses
 * and not overflow
 */
enum Buttons menuButtonPressQueue[MAX_MENU_BUTTON_PRESS_QUEUE_SIZE]; //Might be used in f
/*
 * named as t[x][direction]Border
 * where x = thickness(t) of border (1-3)
 * direction = left | right
 */
const short t1Thickness = 3;
const char t1LeftBorder = ~0xE0;
const char t1RightBorder = ~0x07;

const short t2Thickness = 2;
const char t2LeftBorder = ~0xC0;
const char t2RightBorder = ~0x03;

const short t3Thickness = 1;
const char t3LeftBorder = ~0x80;
const char t3RightBorder = ~0x01;

/*
 * technically only need one of these for the upper & lower border
 * but it could be subject to change in the future for some reason
 * so declare both seperately. This will also make it clearer in the code
 */
const char topBorder = 0x00;
const char bottomBorder = 0x00;

/*******************************************
 *       STARTUP/WELCOME MESSAGE
 ******************************************/

#define MAX_STARTUP_MSG_CHARS 144
const unsigned char startupMessage1[MAX_STARTUP_MSG_CHARS] = "Spaces.the final fontier"
        "these are the voyages of the USSR Starship Banterprise.Its year long mission";

const unsigned char startupMessage2[MAX_STARTUP_MSG_CHARS] = "to google strange new words.to seek out new forum posts and datasheets.to boldly"
        " go where no    engineer has gone before";

const unsigned char helpMessageButtons[MAX_STARTUP_MSG_CHARS] = "Button 1     confirm    Button 2     scroll down";

const unsigned char helpMessageButtons2[MAX_STARTUP_MSG_CHARS] = "to alter values like test duration press button 1 on the option to begin"
        "changing it: button 1   increments   button 2   decrements";

/*******************************************
 *     MENU OPTION TEXT DEFINITIONS
 ******************************************/

/*
 * Return indicates the final option of every menu
 */
const short t3MenuTextMax = 4;
const char t3MenuText[t3MenuTextMax][TYPE3_MAX_UCA] =
{
     "Test      duration",
     "help pls", /*This should display a help message when pressed*/
     "search",
     "RETURN"
};

/*
 * These are the menu Sub-options for t3MenuText option number 1
 * This is just an example, but in the future there would be more of these,
 * held within a more advanced container which allows integer/float values for
 * options to be maintained within. For now, just text to use as an example.
 */
const short t3MenuSubOptions1Max=2;
const char t3MenuSubOptions1[t3MenuSubOptions1Max][TYPE3_MAX_UCA] =
{
    "3min", /*add up & down arrows after text when populating this text box*/
    "RETURN"
};

const short t2MenuTextMax = 3;
const char t2MenuText[t2MenuTextMax][TYPE2_MAX_UCA] = {
/*    |        |         |         | These indicate the newlines to give an idea of text formatting*/
     "Example of some longer text",
     "Another example of some longer text",
     "RETURN"
};

const short t1MenuTextMax = 2;
const char t1MenuText[t1MenuTextMax][TYPE1_MAX_UCA] = {
     "Here is a bunch of  text which \
      could be used as an information \
      hub for disclaimer or something ",
      "RETURN"
};


/*******************************************
 *     FUNCTION DEFINITIONS
 ******************************************/


/**********************************************************************
 * Function name: writeLineOfChars
 *
 * Writes a line of chars (charToWrite) VERTICAL(0) or HORIZONTAL(1)
 * as dictated by the enum LineDirection at position (1-96).
 ***********************************************************************/
void writeLineOfChars(char charToWrite, short position, LineDirection lineDir)
{
    int i = 0;
    if (VERTICAL == lineDir)
    {
        for (i = 0; i < MAX_PIXELS_VERT; i++)
        {
            displayBuffer[position-1][i] = charToWrite;
        }
    }
    else
    {
        for (i = 0; i < 12; i++)
        {
            displayBuffer[i][position-1] = charToWrite;
        }
    }
}


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
void writeFormattedText(const char *inText, int size, int positionX,
                        int positionY, short maxCharsPerLine, bool inverted)
{
    int i = 0;
    int index = 0;
    for (i = 0; i < size; i++)
    {
        if (maxCharsPerLine == index)
        {
            positionX -= maxCharsPerLine; //Reset cursor
            positionY += 8; //Newline
            index = 0;
        }
        writeChar( inText[i], positionX, positionY, inverted);
        ++positionX;
        ++index;
    }
}


/**********************************************************************
 * Function name: writeHorizontalBorders
 *
 * Using type as an indicator for where to write the borders, populates the
 * outputDisplayBuffer with the border values corresponding to the number of
 * boxes required for the Upper & lower borders of the menuBoxes.
 ***********************************************************************/
void writeHorizontalBorders(short type)
{
    int i = 0, j = 0;
    int index = 1;

    if (MIN_BOXES_ONSCREEN < type < MAX_BOXES_ONSCREEN)
    {
        switch (type)
        {
        case 1:
            /* write top borders*/
            for (i = index; i <= t1Thickness; i++)
            {
                writeLineOfChars(topBorder, i, HORIZONTAL);
            }
            /* write bottom borders*/
            index = ((MAX_PIXELS_HORIZ / type) - (t1Thickness-1)); /*re-adjust index */
            for (i = 0; i < t1Thickness; i++)
            {
                writeLineOfChars(bottomBorder, index++, HORIZONTAL);
            }
            break;

        case 2:
            /* write top borders */
            for (j = 0; j < type; j++)
            {
                index = ((j * (MAX_PIXELS_HORIZ / type)) + 1);
                for (i = 0; i <= t2Thickness; i++)
                {
                    writeLineOfChars(topBorder, index++, HORIZONTAL);
                }
            }

            /* write Bottom borders */
            for (j = 1; j <= type; j++)
            {
                index = ((j * (MAX_PIXELS_HORIZ / type)) - (t2Thickness - 1));
                for (i = 0; i <= t2Thickness; i++)
                {
                    writeLineOfChars(bottomBorder, index++, HORIZONTAL);
                }
            }
            break;

        case 3:
            /* write top borders */
            for (j = 0; j < type; j++)
            {
                index = ((j * (MAX_PIXELS_HORIZ / type)) + 1);
                for (i = 0; i <= t3Thickness; i++)
                {
                    writeLineOfChars(topBorder, index++, HORIZONTAL);
                }
            }
            /* write Bottom borders */
            for (j = 1; j <= type; j++)
            {
                index = ((j * (MAX_PIXELS_HORIZ / type)) - (t3Thickness));
                for (i = 0; i <= t3Thickness; i++)
                {
                    writeLineOfChars(bottomBorder, index++, HORIZONTAL);
                }
            }
            break;
        }
    }
    else
    {
        return; /* add error handling later */
    }
}

/**********************************************************************
 * Function name: writeVerticalBorders
 *
 * Using type as an indicator for where to write the borders, populates the
 * outputDisplayBuffer with the border values corresponding to the number of
 * boxes required for the left and right borders of the menuBoxes.
 ***********************************************************************/
int writeVerticalBorders(short type)
{
    if (MIN_BOXES_ONSCREEN < type < MAX_BOXES_ONSCREEN)
    {
        switch (type)
        {

        case 1:
            /* write left border */
            writeLineOfChars(t1LeftBorder, 1, VERTICAL);
            /* write right border */
            writeLineOfChars(t1RightBorder, 12, VERTICAL);
            break;
        case 2:
            /* write left border */
            writeLineOfChars(t2LeftBorder, 1, VERTICAL);
            /* write right border */
            writeLineOfChars(t2RightBorder, 12, VERTICAL);
            break;

        case 3:
            /* write left border */
            writeLineOfChars(t3LeftBorder, 1, VERTICAL);
            /* write right border */
            writeLineOfChars(t3RightBorder, 12, VERTICAL);
            break;
        }
        return 0; /*Needs return statement*/
    }
    else
    {
        return -1; /* add error handling later */
    }
}

/**********************************************************************
 * Function name: writeTextBoxes
 *
 * Using the write[Horizontal/Vertical]Borders() and type as an indicator
 * for the number of boxes to write, populates the outputDisplayBuffer with
 * all borders required for the type.
 ***********************************************************************/
int writeTextBoxes(short type)
{
    if (MIN_BOXES_ONSCREEN < type < MAX_BOXES_ONSCREEN)
    {
        /*
         * First draw our right and left borders which
         * makes dealing with overwriting simpler.
         */
        writeVerticalBorders(type);
        /* Next the horizontal lines */
        writeHorizontalBorders(type);

        return 0; /*Need return statement */
    }
    else
    {
        return -1; /* add error handling later */
    }

}

/* TODO FIGURE OUT BEST WAY TO HIGHLIGHT TEXT/TEXTBOX */

/**********************************************************************
 * Function name: populateTextBox
 *
 * Populates the boxNum with the MenuText. Type is required
 * in order to check for text that is too long to fit inside a single box
 * and would otherwise overwrite other pixels. Currently only in charge
 * of handling text though will in future be extended to deal with
 * integer values like a duration.
 ***********************************************************************/
int populateTextBox(const char *menuText, short type, short boxNum, bool inverted)
{
    int size = 0;
    int posYIndex=0;
    if ((MIN_BOXES_ONSCREEN < type < MAX_BOXES_ONSCREEN)
            && (MIN_BOXES_ONSCREEN < boxNum < MAX_BOXES_ONSCREEN))
    {
        switch (type)
        {
        case 1:
            size = strlen(menuText);
            /* Write the first entry in the type 1 menu text beginning
             *  at (1,3), using newlines every 10 characters.
             */
            if (size > 0)
            {
                writeFormattedText(menuText, size, 1, t1Thickness, 10, inverted);
            }
            break;
        case 2:
            size = strlen(menuText);
            /* Write the first entry in the type 1 menu text beginning
             *  at (1,3), using newlines every 10 characters.
             */
            if (size > 0)
            {
                posYIndex = (((boxNum - 1) * (MAX_PIXELS_HORIZ / type))
                        + t2Thickness + 1);
                writeFormattedText(menuText, size, 1, posYIndex, 10, inverted);
            }
            break;
        case 3:
            size = strlen(menuText);

            if (size > 0)
            {
                posYIndex = (((boxNum - 1) * (MAX_PIXELS_HORIZ / type))
                        + t3Thickness + 1);
                writeFormattedText(menuText, size, 1, posYIndex, 10, inverted);
            }
            break;
        }
        return 0; /*Error handling later on*/
    }
    else
    {
        return -1; /* add error handling later */
    }
}

/**********************************************************************
 * Function name: initialiseMenuBoxes
 *
 * Initialisation called whenever entering menu
 * from different screen. Currently only type 3 implemented.
 ***********************************************************************/
void initialiseMenuBoxes( int type )
{
    initDisplayBuffer(0xff);
    writeTextBoxes(type);
    //clearButtonPressQueue();
    populateTextBox(t3MenuText[0], type, 1, true);
    populateTextBox(t3MenuText[1], type, 2, false);
    populateTextBox(t3MenuText[2], type, 3, false);
    /* TODO Add highlight box here */
}

/**********************************************************************
 * Function name: floatToDuration
 *
 * converts the flooating point value for the menu test duration
 * to a string value that can be used to call populate text boxes and
 * update the screen. Populates the tempDurationText global(temporarily).
 ***********************************************************************/

short menuOptionNumber=0; //These three essentially work as an index to know where we are & how to update
short highlightedOption=1;
short menuNestedLevel=1;  //Text to output to boxes
const short levelOneMax=4;
int tempDuration = 3; //Default 3 mins
char tempDurationText[5]; //Default is 3m and min=1 max=5.00

int floatToDuration( float durationIn)
{
    //This might be too processing intensive but because it's always gonna be a small number
    //it doesn't matter so much...i hope
    int ret = snprintf(tempDurationText, sizeof(tempDurationText), "%5.2f", tempDuration);

    if (ret < 0) {
        return 0;
    }
    if (ret > sizeof(tempDurationText)) {
        // Result was truncated
    }
    return 1;
}

/**********************************************************************
 * Function name: updateMenuBoxes
 *
 * function called whenever already in displayState MENUBOX
 * and a button press has occurred which the screen needs to be updated for.
 * outputDisplayBuffer() should be called every time after calling this function
 ***********************************************************************/

// SORRY THIS SHIT IS UNREADABLE - Much better version in the future to manage this easier

int updateMenuBoxes( enum Buttons buttonPress )
{
    int type = 3; //Default unless overrided
    if (Button1 == buttonPress)
    {   // Confirm
        switch (menuNestedLevel)
        {
        case 1: //menuNestedLevel
            switch (highlightedOption)
            {
            case 1: //Test duration
                type = 2;
                initDisplayBuffer(0xff);
                writeTextBoxes(type);
                populateTextBox(t3MenuSubOptions1[0], type, 1, true);
                populateTextBox(t3MenuSubOptions1[1], type, 2, false);
                menuOptionNumber = 1;
                menuNestedLevel = 2;
                highlightedOption = 1;
                break;
            case 2: //Help Pls
                type = 1;
                initDisplayBuffer(0xff);
                writeFormattedText(helpMessageButtons, strlen(helpMessageButtons), 0, 0, 12, true);
                menuOptionNumber = 2;
                menuNestedLevel = 2;
                highlightedOption = 1;
                break;
            case 3: // search
                //TODO Implement
                break;
            case t3MenuTextMax: //Return to ECG screen
                // TODO Implement ECG screen
                break;
            }
            break;

        case 2: //menuNestedLevel
            switch (menuOptionNumber)
            {
            case 1: //Test duration
                if(1 == highlightedOption)
                {   //go into configuration mode (menuNestedLevel = 3)
                    writeCharFromIndex(39, 10, 3, true);
                    writeCharFromIndex(40, 10, 12, true);
                    menuNestedLevel = 3;
                    menuOptionNumber = 1;
                }
                else if (2 == highlightedOption)
                {   //return to initial menu screen
                    initialiseMenuBoxes(3);
                    menuOptionNumber = 0;
                    menuNestedLevel = 1;
                    highlightedOption = 1;
                }
                break;
            case 2: //Help Pls
                //return to initial menu screen
                initialiseMenuBoxes(3);
                menuOptionNumber = 0;
                menuNestedLevel = 1;
                highlightedOption = 1;
                break;
            }
            break;

        case 3: //MenuNestedLevel
            switch( menuOptionNumber)
            {
            case 1: //Test duration increment by .5 of a second
                type = 2;
                if(!( tempDuration >= 5.00))
                {
                    ++tempDuration;
                    writeChar((tempDuration + '0'), 1, 3, true); //Hoping this won't erase the arrows
                }
                break;
            case 3: //For when we implement search
                break;
            }
            break;
        }

    }
    else if (Button2 == buttonPress)
    {   //Scroll Down
        switch (menuNestedLevel)
        {
        case 1: //menuNestedLevel
            type = 3;
            switch (highlightedOption)
            {
            case 1:
                populateTextBox(t3MenuText[0], type, 1, false);
                populateTextBox(t3MenuText[1], type, 2, true);
                populateTextBox(t3MenuText[2], type, 3, false);
                ++highlightedOption;
                break;
            case 2:
                populateTextBox(t3MenuText[0], type, 1, false);
                populateTextBox(t3MenuText[1], type, 2, false);
                populateTextBox(t3MenuText[2], type, 3, true);
                ++highlightedOption;
                break;
            case 3:
                initDisplayBuffer(0xff);
                writeTextBoxes(3);
                populateTextBox(t3MenuText[1], type, 1, false);
                populateTextBox(t3MenuText[2], type, 2, false);
                populateTextBox(t3MenuText[3], type, 3, true);
                ++highlightedOption;
                break;
            case t3MenuTextMax: //Return to first option
                populateTextBox(t3MenuText[0], type, 1, true);
                populateTextBox(t3MenuText[1], type, 2, false);
                populateTextBox(t3MenuText[2], type, 3, false);
                highlightedOption=1;
                break;
            }
            break;

        case 2: //menuNestedLevel
            switch (menuOptionNumber)
            {
            case 1: //Test Duration
                type = 2;
                if (1 == highlightedOption)
                {
                    populateTextBox(t3MenuSubOptions1[0], type, 1, false);
                    populateTextBox(t3MenuSubOptions1[1], type, 2, true);
                    highlightedOption = 2;
                }
                else if (2 == highlightedOption)
                {
                    populateTextBox(t3MenuSubOptions1[0], type, 1, true);
                    populateTextBox(t3MenuSubOptions1[1], type, 2, false);
                    highlightedOption = 1;
                }
                break;

            case 2: //Help Pls
                type = 1;
                if (1 == highlightedOption)
                {
                    initDisplayBuffer(0xff);
                    writeFormattedText(helpMessageButtons, strlen(helpMessageButtons), 0, 0, 12, true);
                    highlightedOption = 2;
                }
                else if (2 == highlightedOption)
                {
                    initDisplayBuffer(0xff);
                    writeFormattedText(helpMessageButtons2, strlen(helpMessageButtons2), 0, 0, 12, true);
                    highlightedOption = 1;
                }
                break;

            case 3: // Search
                //TODO
                break;

            }
            break;

        case 3: //menuNestedLevel
            type = 2;
            switch (menuOptionNumber)
            {
            case 1: //Test duration decrement by .5 of a second
                if(!( tempDuration <= 1 ))
                {
                    --tempDuration;
                    writeChar((tempDuration + '0'), 1, 3, true); //Hoping this won't erase the arrows
                }
                break;
            case 3: //For when we implement search
                break;
            }
            break;
        }
    }
    else if ( Num_buttons ) //This can be used as a temporary indicator to mean "Both buttons pressed"
    {
        switch(menuNestedLevel)
        {
        case 3:
            switch(menuOptionNumber)
            {
            case 1:
                writeChar(' ', 10, 3,  false);
                writeChar(' ', 10, 12, false);
                menuOptionNumber = 1;
                menuNestedLevel = 2;
                highlightedOption = 1;
                break;
            case 3:
                //placeholder for search in the future
                break;
            }
            break;
        }
    }
    else
    {
        // We dun goofed
        return -1;
    }

    return 1;
}
