#include "visualmode.h"
#include "general.h"
#include "commandmode.h"
#include "inputmode.h"

// Do all the stuff, including highlighting, copy/cut to clipboard (NEED TO DEFINE CLIPBOARD)
int RunVisualMode(void) {
    int starty, startx, endy, endx;
    starty = sy;
    startx = sx;
    endy = starty;
    endx = startx;
    int lasty = sy, lastx = sx;
    while (currentMode == VISUALMODE) {
        lasty = endy;      // Update last position to previous end
        lastx = endx;
        int keyhitresult = ProcessKeyhit();   // <-- First process movement
        ClearHighlight(starty, startx, lasty, lastx);   // Clear using lasty/lastx
        endy = sy;         // Update current end after movement
        endx = sx;
        Highlight(starty, startx, endy, endx);   // Draw new highlight
        Refresh();
        if (keyhitresult == SIGNAL_SWITCH_TO_NORMALMODE) break;
        else if (keyhitresult == SIGNAL_COPY_SELECTED_TEXT_AND_SWITCH_TO_NORMALMODE) {
            Copy(starty, startx, endy, endx);
            break;
        } else if (keyhitresult == SIGNAL_CUT_SELECTED_TEXT_AND_SWITCH_TO_NORMALMODE) {
            Cut(starty, startx, endy, endx);
            break;
        }
    }
    return SIGNAL_SWITCH_TO_NORMALMODE;
}

// Clear highlight of piece of text given by (starty, startx) to (endy, endx)
// NEED TO FIND TOPLEFT AND BOTTOMRIGHT OF EACH HIGhLIGHT SEGMENT
void ClearHighlight(int starty, int startx, int endy, int endx) {
    // Find out the y-coordinates for the start of the highlighting
    int miny = min(starty, endy);
    // Now find the x coord for the miny coord and x coord for maxy coord as well
    int y1, x1, y2, x2;

    if (miny == starty) {
        y1 = starty;
        x1 = startx;
        y2 = endy;
        x2 = endx;
    } else if (miny == endy) {
        y1 = endy;
        x1 = endx;
        y2 = starty;
        x2 = startx;
    } else {
        y1 = starty;
        x1 = startx;
        y2 = endy;
        x2 = endx;
    }

    if (y1 == y2) {
        x1 = min(startx, endx);
        x2 = max(startx, endx);
        for (int j = x1; j <= x2; j ++) {
            mvwchgat(imscr, y1, j, 1, A_NORMAL, NORMAL_TEXT, NULL);
        }
        return;
    }

    for (int i = y1; i <= y2; i ++) {
        if (i == y1) {
            for (int j = x1; j <= lines.arr[i]->len - 1; j ++) {
                mvwchgat(imscr, i, j, 1, A_NORMAL, NORMAL_TEXT, NULL);
            }
        } else if (i == y2) {
            for (int j = 0; j <= x2; j ++) {
                mvwchgat(imscr, i, j, 1, A_NORMAL, NORMAL_TEXT, NULL);
            }
        } else {
            for (int j = 0; j <= lines.arr[i]->len - 1; j ++) {
                mvwchgat(imscr, i, j, 1, A_NORMAL, NORMAL_TEXT, NULL);
            }
        }
    }

    return;
}

// Highlight a piece of text given by (starty, startx) to (endy, endx)
// NEED TO FIND TOPLEFT AND BOTTOMRIGHT OF EACH HIGhLIGHT SEGMENT
void Highlight(int starty, int startx, int endy, int endx) {
    // Find out the y-coordinates for the start of the highlighting
    int miny = min(starty, endy);
    // Now find the x coord for the miny coord and x coord for maxy coord as well
    int y1, x1, y2, x2;

    if (miny == starty) {
        y1 = starty;
        x1 = startx;
        y2 = endy;
        x2 = endx;
    } else if (miny == endy) {
        y1 = endy;
        x1 = endx;
        y2 = starty;
        x2 = startx;
    } else {
        y1 = starty;
        x1 = startx;
        y2 = endy;
        x2 = endx;
    }

    if (y1 == y2) {
        x1 = min(startx, endx);
        x2 = max(startx, endx);
        for (int j = x1; j <= x2; j ++) {
            mvwchgat(imscr, y1, j, 1, A_NORMAL, HIGHLIGHTED_TEXT, NULL);
        }
        return;
    }

    for (int i = y1; i <= y2; i ++) {
        if (i == y1) {
            for (int j = x1; j <= lines.arr[i]->len - 1; j ++) {
                mvwchgat(imscr, i, j, 1, A_NORMAL, HIGHLIGHTED_TEXT, NULL);
            }
        } else if (i == y2) {
            for (int j = 0; j <= x2; j ++) {
                mvwchgat(imscr, i, j, 1, A_NORMAL, HIGHLIGHTED_TEXT, NULL);
            }
        } else {
            for (int j = 0; j <= lines.arr[i]->len - 1; j ++) {
                mvwchgat(imscr, i, j, 1, A_NORMAL, HIGHLIGHTED_TEXT, NULL);
            }
        }
    }
    return;
}

// Select a piece of text and copy to clipboard
void Copy(int starty, int startx, int endy, int endx) {

    return;
}

// Select a piece of text, copy to clipboard and delete from screen
void Cut(int starty, int startx, int endy, int endx) {

    return;
}
