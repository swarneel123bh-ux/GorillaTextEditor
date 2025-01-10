#include "general.h"
#include "inputmode.h"

/*
Insert a new line UNDER the current line

Create a line in memory and shift everything to the right of current line
to the right. Then add new line to current position

Ncurses inserts line ABOVE current y coord so we need to take care of that
by going down by 1 first and then inserting the line and then coming back

This wont be an issue in normal func calls and the function would keep all coords unchanged
*/
void InsertNewLine() {
    // Insert the new line on screen underneath current line
    IMSCR_CURS_Y ++;
    IMSCR_CURS_MOVE();
    winsertln(imScr->win->window);  // Inserts new line above current line and pushes everything else down
    IMSCR_CURS_Y--; // Go back to original line position
    IMSCR_CURS_MOVE();

    // Add new line to memory if close to using up all available lines
    // Increase allocated space
    if (imScr->nLines >= (imScr->nPossibleLines - 10)) {
        // add a whole window worth of lines to the previous size
        imScr->nPossibleLines += imScr->nPossibleLines; // Dont forget to double this as well
        int newSize = sizeof(line*) * imScr->nPossibleLines;
        line** temp = (line**) realloc(imScr->lines, newSize);
        if (!temp) {
            Message("ERROR : Cannot add new line; Exiting...");
            SLEEP(3);
            ExitProgram(ERR_MEM_LINE_ADD_FAIL);
        }
        imScr->lines = temp;    // Set lines to new allocated mem
    }

    imScr->nLines ++;       // Increase the number of lines currently available
    // Shift the currently stored lines down by one slot at current line pos
    IMSCR_MEM_Y ++;
    SHIFTRIGHT(imScr->lines, IMSCR_MEM_Y, imScr->nPossibleLines);    
    imScr->lines[IMSCR_MEM_Y] = NewLine(); // Set new mem pos to a line pointer
    IMSCR_MEM_Y --; // Line we are currently on should not change
    REFRESH();
}

/*
Inserts a new line ABOVE the current line, all functionality same as normal
insert except this
*/
void InsertNewLineAbove() {
    // Inserts new line above current line and pushes everything else down
    winsertln(imScr->win->window);  

    // Add new line to memory if close to using up all available lines
    // Increase allocated space
    if (imScr->nLines >= (imScr->win->wCursLines - 5)) {
        int newSize = sizeof(line*) * ((sizeof(imScr->lines) / sizeof(line*) + imScr->nPossibleLines));
        line** temp = (line**) realloc(imScr->lines, newSize);
        if (!temp) {
            Message("ERROR : Cannot add new line\n");
            SLEEP(3);
            ExitProgram(ERR_MEM_LINE_ADD_FAIL);
        }
        imScr->lines = temp;    // Set lines to new allocated mem
        imScr->nPossibleLines += imScr->nPossibleLines;
    }

    imScr->nLines ++;       // Increase the number of lines currently available
    // Shift the currently stored lines down by one slot at current line pos
    SHIFTRIGHT(imScr->lines, IMSCR_MEM_Y, imScr->nLines);    
    imScr->lines[IMSCR_MEM_Y] = NewLine(); // Set new mem pos to a line pointer
    REFRESH();
}


/*
Given the character ch, insert it into the current position on the imScr->win
*/
void InsertInLine(int ch) {
    SHIFTRIGHT(IMSCR_CURLINE_BUF, IMSCR_MEM_X, IMSCR_CURLINE->maxLen);
    IMSCR_CURLINE_BUF[IMSCR_MEM_X] = ch;
    IMSCR_CURLINE->len ++;
    winsch(imScr->win->window, ch);
    IMSCR_CURS_X ++; 
    IMSCR_MEM_X ++;
    IMSCR_CURS_MOVE(); 
    REFRESH();
}


// NOTE :: NEED TO IMPLEMENT VISUALMODE BEFORE THESE CAN BE USED
/*
Copy the contents that fall between the starting coordinates and ending coordinates,
into the clipboard buffer
*/
void Copy() {}

/*
Copy the contents that fall between the starting and ending coords into the
clipboard buffer and also remove them from the screen properly
*/
void Cut() {}

/*
Paste the contents of a buffer at current cusor position and move cursor to the end of the pasted stuff
*/
void Paste(char* buf) {
    int ch;
    int i = 0;
    while ((ch = buf[i++])) InsertInLine(ch);
}