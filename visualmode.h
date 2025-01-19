#ifndef VISUALMODE_H
#define VISUALMODE_H

/*
Deals with highlighting text and returning coordinates to for
copy/cut/pasting stuff
*/

#include "general.h"

// highlighting colors
#define NORMAL_TEXT 1
#define HIGHLIGHTED_TEXT 2

// Selection Modes
#define BY_CHAR 3
#define BY_LINE 4

// Macros for shortcuts
#define FLIP_HIGHLIGHT() {\
int curColor = PAIR_NUMBER(winch(imScr->win->window) & A_COLOR);\
wchgat(imScr->win->window, 1, A_NORMAL,\
(curColor == NORMAL_TEXT)? HIGHLIGHTED_TEXT : NORMAL_TEXT,\
NULL);\
}

// Flags
bool visualmode_running;

//Function definitons

/*
Select a part of text, highlight the selected part and
return the coordinates for clipboard reference
*/
void Select(int mode, int* startY, int* startX, int* endY, int* endX);          

void Cut();                     // Cut function
void Copy();                    // Copy function 
void Paste(char* buf);          // Paste Function

#endif  // VISUALMODE_H