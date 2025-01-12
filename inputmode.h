#ifndef INPUTMODE_H
#define INPUTMODE_H

#include "general.h"

// Macros for shortcuts
// Cursor position on the imScr->win->window
#define IMSCR_CURS_X imScr->win->wCursX
#define IMSCR_CURS_Y imScr->win->wCursY
#define IMSCR_MEM_X imScr->memX
#define IMSCR_MEM_Y imScr->memY
#define IMSCR_CURS_POS IMSCR_CURS_Y, IMSCR_CURS_X
#define IMSCR_CURS_MOVE() wmove(imScr->win->window, IMSCR_CURS_Y, IMSCR_CURS_X) // Moving the cursor to current IMSCR_CURS_POS
#define IMSCR_CURLINE imScr->lines[IMSCR_MEM_Y] // Gives the current line's object
#define IMSCR_CURLINE_BUF IMSCR_CURLINE->buf // Gives the current line's character buffer
#define IMSCR_CURCHAR IMSCR_CURLINE_BUF[IMSCR_MEM_X] // Gives the character the cursor is currently on

// Macros for keypresses
typedef struct inputModeScreen {
    int nLines;         // Number of modifiable lines in memory
    int nPossibleLines; // Allocated size of lines**
    int memY;           // Corresponds to line* in lines
    int memX;           // Corresponds to line->buf[X]
    line** lines;       // Line array for text input
    subWin* win;        // Window wrapper for input mode screen
} imscr;
imscr* imScr;   // Contains all information regaring the inputmode screen

// Functions for keypress effects
void IMSCR_CURS_NAV_UP();       // Normal UP arrow key effect
void IMSCR_CURS_NAV_DOWN();     // Normal DOWN arrow key effect
void IMSCR_CURS_NAV_LEFT();     // Normal LEFT arrow key effect
void IMSCR_CURS_NAV_RIGHT();    // Normal RIGHT arrow key effect
void IMSCR_CRLF();              // Normal ENTER key effect

void InsertNewLine();           // Insert a new line underneath the current line 
void InsertNewLineAbove();      // Insert a new line above the current line 
void InsertInLine(int ch);      // Insert ch in current cursor position both in memory and screen
void Cut();                     // Cut function
void Copy();                    // Copy function 
void Paste(char* buf);                   // Paste Function

#endif // INPUTMODE_H