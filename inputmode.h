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
/*
Adds a new line underneath current line, copies the content of the
current line to the right of current cursor position,
pastes the copied content into the new line
DOES NOT account for scrolling, needs to be checked separately
*/
#define IMSCR_CRLF() {\
    InsertNewLine();\
    char copyBuf[1000]; memset(copyBuf, 0, sizeof(char) * 1000);\
    memcpy(copyBuf, (IMSCR_CURLINE_BUF + IMSCR_MEM_X), sizeof(char) * strlen(IMSCR_CURLINE_BUF + IMSCR_MEM_X));\
    memset((IMSCR_CURLINE_BUF + IMSCR_MEM_X), 0, sizeof(char) * strlen(IMSCR_CURLINE_BUF + IMSCR_MEM_X));\
    wclrtoeol(imScr->win->window);\
    IMSCR_CURLINE->len -= strlen(copyBuf);\
    IMSCR_CURS_Y ++;\
    IMSCR_MEM_Y ++;\
    IMSCR_CURS_X = 0;\
    IMSCR_MEM_X = 0;\
    IMSCR_CURS_MOVE();\
    char c_; int i = 0;\
    while ((c_ = copyBuf[i++])) {\
        InsertInLine(c_);\
    }\
    IMSCR_CURS_X = 0;\
    IMSCR_MEM_X = 0;\
    IMSCR_CURS_MOVE();\
    REFRESH();\
}

typedef struct inputModeScreen {
    int nLines;         // Number of modifiable lines in memory
    int nPossibleLines; // Allocated size of lines**
    int memY;           // Corresponds to line* in lines
    int memX;           // Corresponds to line->buf[X]
    line** lines;       // Line array for text input
    subWin* win;        // Window wrapper for input mode screen
} imscr;
imscr* imScr;   // Contains all information regaring the inputmode screen

void InsertNewLine();           // Insert a new line underneath the current line 
void InsertNewLineAbove();      // Insert a new line above the current line 
void InsertInLine(int ch);      // Insert ch in current cursor position both in memory and screen
void Cut();                     // Cut function
void Copy();                    // Copy function 
void Paste(char* buf);                   // Paste Function

#endif // INPUTMODE_H