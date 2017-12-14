/*
 * lcdHandler.h
 *
 *  Created on: 13 Dec 2017
 *      Author: m3-alden
 */

#ifndef LCDHANDLER_H_
#define LCDHANDLER_H_

void initDisplay (void);
extern char DisplayBuffer[96][12];
void initDisplayBuffer(char);
void outputDisplayBuffer(char,char);
void setText (char, char, char *);


#endif /* LCDHANDLER_H_ */
