#include "general.h"
#include "commandmode.h"
#include "inputmode.h"

// Normal UP arrow key effect
void IMSCR_CURS_NAV_UP() {
    if (IMSCR_MEM_Y == 0) { IMSCR_MEM_X = 0; IMSCR_CURS_X = 0; }
    else if (IMSCR_CURS_Y == 0) {
        if (IMSCR_MEM_Y > 0) {
            wscrl(imScr->win->window, -1);
            mvwprintw(imScr->win->window, IMSCR_CURS_Y, 0, "%s",
            IMSCR_CURLINE_BUF);
            IMSCR_MEM_Y --;
            IMSCR_CURS_X = min(IMSCR_CURLINE->len, IMSCR_CURS_X);
            IMSCR_MEM_X = IMSCR_CURS_X;
        } else { IMSCR_MEM_X = 0; IMSCR_CURS_X = 0; }
    }
    else {
        IMSCR_CURS_Y --;
        IMSCR_MEM_Y --;
        IMSCR_CURS_X = min(IMSCR_CURLINE->len, IMSCR_CURS_X);
        IMSCR_MEM_X = IMSCR_CURS_X;
    }
    IMSCR_CURS_MOVE();
}

// Normal DOWN arrow key effect
// If cursor y coord is at end of the screen, scroll
// If mem y coord is at end of linearr, do nothing
void IMSCR_CURS_NAV_DOWN() {
    if (IMSCR_MEM_Y == imScr->nLines - 1) {
        if (IMSCR_MEM_X == IMSCR_CURLINE->len) {}
        else { IMSCR_CURS_X = IMSCR_CURLINE->len; IMSCR_MEM_X = IMSCR_CURS_X; }
    } else if (IMSCR_CURS_Y == imScr->win->wCursLines - 1) {
        wscrl(imScr->win->window, 1);
        IMSCR_MEM_Y ++;
        mvwprintw(imScr->win->window, IMSCR_CURS_Y, 0, "%s",
        IMSCR_CURLINE_BUF);
        IMSCR_CURS_X = min(IMSCR_CURS_X, IMSCR_CURLINE->len);
        IMSCR_MEM_X = IMSCR_CURS_X;
    } else {
        IMSCR_MEM_Y ++;
        IMSCR_CURS_Y ++;
        IMSCR_CURS_X = min(IMSCR_CURS_X, IMSCR_CURLINE->len);
        IMSCR_MEM_X = IMSCR_CURS_X;
    }
    IMSCR_CURS_MOVE();
}

// Normal LEFT arrow key effect
void IMSCR_CURS_NAV_LEFT() {
    if (IMSCR_CURS_X > 0) { IMSCR_CURS_X --; IMSCR_MEM_X = IMSCR_CURS_X; }
    else if (IMSCR_CURS_Y > 0) {
        IMSCR_CURS_Y --;
        IMSCR_MEM_Y --;
        IMSCR_CURS_X = IMSCR_CURLINE->len;
        IMSCR_MEM_X = IMSCR_CURS_X;
    } else if (IMSCR_MEM_Y > 0) {
        wscrl(imScr->win->window, -1);
        IMSCR_MEM_Y --;
        mvwprintw(imScr->win->window, IMSCR_CURS_Y, 0, "%s",
        IMSCR_CURLINE_BUF);
        IMSCR_CURS_X = IMSCR_CURLINE->len;
        IMSCR_MEM_X = IMSCR_CURS_X;
    } else { }
    IMSCR_CURS_MOVE();
}

// Normal RIGHT arrow key effect
void IMSCR_CURS_NAV_RIGHT() {
    if (IMSCR_CURS_X < IMSCR_CURLINE->len) { IMSCR_CURS_X ++; IMSCR_MEM_X = IMSCR_CURS_X; }
    else if (IMSCR_CURS_Y < imScr->win->wCursLines) {
        IMSCR_CURS_Y ++;
        IMSCR_MEM_Y ++;
        IMSCR_CURS_X = 0;
        IMSCR_MEM_X = IMSCR_CURS_X;
    } else if (IMSCR_MEM_Y < imScr->nLines) {
        wscrl(imScr->win->window, 1);
        IMSCR_MEM_Y ++;
        mvwprintw(imScr->win->window, IMSCR_CURS_Y, 0, "%s",
        IMSCR_CURLINE_BUF);
        IMSCR_CURS_X = 0;
        IMSCR_MEM_X = 0;
    } else {}
    IMSCR_CURS_MOVE();
}

/*
Adds a new line underneath current line, copies the content of the
current line to the right of current cursor position,
pastes the copied content into the new line
DOES NOT account for scrolling, needs to be checked separately
*/
void IMSCR_CRLF() {
    InsertNewLine();
    char copyBuf[1000]; memset(copyBuf, 0, sizeof(char) * 1000);
    memcpy(copyBuf, (IMSCR_CURLINE_BUF + IMSCR_MEM_X), sizeof(char) * strlen(IMSCR_CURLINE_BUF + IMSCR_MEM_X));
    memset((IMSCR_CURLINE_BUF + IMSCR_MEM_X), 0, sizeof(char) * strlen(IMSCR_CURLINE_BUF + IMSCR_MEM_X));
    wclrtoeol(imScr->win->window);
    IMSCR_CURLINE->len -= strlen(copyBuf);
    IMSCR_CURS_Y ++;
    IMSCR_MEM_Y ++;
    IMSCR_CURS_X = 0;
    IMSCR_MEM_X = 0;
    IMSCR_CURS_MOVE();
    char c_; int i = 0;
    while ((c_ = copyBuf[i++])) {
        InsertInLine(c_);
    }
    IMSCR_CURS_X = 0;
    IMSCR_MEM_X = 0;
    IMSCR_CURS_MOVE();
    REFRESH();
}

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