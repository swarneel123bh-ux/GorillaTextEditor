#include "visualmode.h"
#include "mem.h"
#include "general.h"
#include "inputmode.h"

/*
Select a part of text, highlight the selected part and
return the coordinates for clipboard reference

Depending on mode, we can either select stuff character by character
or line by line

if mode is BYLINE then we highlight a non-highlighter line and select that,
and de-highlight a highlighted line and deselect that

if mode is BYCHAR then we do the same thing by character by character

See vim-motions for reference
*/
void Select(int mode, int* startY, int* startX, int* endY, int* endX) {
    switch (mode) {
        case BY_LINE:
        case BY_CHAR: {
            while (visualmode_running) {
                int ch = getch();   // Get keyhit
                switch (ch) {
                    case ESC: {
                        currentMode = NORMALMODE;
                        *endY = IMSCR_CURS_X;
                        *endX = IMSCR_CURS_Y;
                        return;
                        break;
                    }

                    // Navigation keys
                    case k:
                    case KEY_UP: { 
                        FLIP_HIGHLIGHT();
                        IMSCR_CURS_NAV_UP();
                        break;
                    }

                    case j:
                    case KEY_DOWN: {
                        FLIP_HIGHLIGHT();
                        IMSCR_CURS_NAV_DOWN();
                        break;
                    }

                    case h: 
                    case KEY_LEFT: {
                        FLIP_HIGHLIGHT();
                        IMSCR_CURS_NAV_LEFT();
                        break;
                    }

                    case l:
                    case KEY_RIGHT: {
                        FLIP_HIGHLIGHT();
                        IMSCR_CURS_NAV_RIGHT();
                        break;
                    }
                
                    default: {}
                }
            }
            break;
        }
    }
}

// note :: need to implement visualmode before these can be used
/*
copy the contents that fall between the starting coordinates and ending coordinates,
into the clipboard buffer
*/
void copy() {}

/*
copy the contents that fall between the starting and ending coords into the
clipboard buffer and also remove them from the screen properly
*/
void cut() {}

/*
paste the contents of a buffer at current cusor position and move cursor to the end of the pasted stuff
*/
void paste(char* buf) {
    int ch;
    int i = 0;
    while ((ch = buf[i++])) InsertInLine(ch);
}