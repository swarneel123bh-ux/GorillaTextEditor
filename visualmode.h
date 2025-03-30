#ifndef VISUALMODE_H
#define VISUALMODE_H

/*
Deals with highlighting text and returning coordinates to for
copy/cut/pasting stuff
*/

#include "general.h"

// Selection Modes
#define BY_CHAR 3
#define BY_LINE 4

// SOMETHING WRONG HERE
#define CLEAR_HIGHLIGHT(starty, startx, endy, endx) {\
    wchgat(imScr->win->window, 1, A_NORMAL, NORMAL_TEXT, NULL);\
}

// SOMETHING WRONG HERE
/*
Cannot seem to run through lines while highlighting 
Seems to get stuck in an infinite loop inside wchgat when using 
loops here
*/
#define HIGHLIGHT(starty, startx, endy, endx) {\
    wchgat(imScr->win->window, 1, A_NORMAL, HIGHLIGHTED_TEXT, NULL);\
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