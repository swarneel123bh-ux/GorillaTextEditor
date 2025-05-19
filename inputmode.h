#ifndef INPUTMODE_H
#define INPUTMODE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <ncurses/ncurses.h>
#include "general.h"

// Macros for shortcuts
// Cursor position on the imscr

// Functions for keypress effects
void cursor_up(void);           // Normal UP arrow key effect
void cursor_down(void);         // Normal DOWN arrow key effect
void cursor_left(void);         // Normal LEFT arrow key effect
void cursor_right(void);        // Normal RIGHT arrow key effect
void crlf(void);                // Normal ENTER key effect
void bckspc(void);              // Normal BACKSPACE key effect
void PrintLoadedFile(void);     // After loading from memory, simply prints out to imscr
void InsertNewLine(void);       // Insert a new line underneath the current line 
void InsertNewLineAbove(void);  // Insert a new line above the current line 
void InsertInLine(int ch);      // Insert into current line
int RunInputMode(void);         // Basic tasks


#endif  // INPUTMODE_H
