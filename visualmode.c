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
    curs_set(0);
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
    ClearAllHighlight();
    Refresh();
    curs_set(1);
    wmove(imscr, sy, sx);
    return SIGNAL_SWITCH_TO_NORMALMODE;
}

void ClearAllHighlight(void) {
    for (int i = 0; i <= lines.lastIndex; i ++) {
        for (int j = 0; j <= lines.arr[i]->len; j ++) {
            mvwchgat(imscr, i, j, 1, A_NORMAL, NORMAL_TEXT, NULL);
        }
    }

    wmove(imscr, sy, sx);
    return;
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
            for (int j = x1; j <= lines.arr[i]->len; j ++) {
                mvwchgat(imscr, i, j, 1, A_NORMAL, NORMAL_TEXT, NULL);
            }
        } else if (i == y2) {
            for (int j = 0; j <= x2; j ++) {
                mvwchgat(imscr, i, j, 1, A_NORMAL, NORMAL_TEXT, NULL);
            }
        } else {
            for (int j = 0; j <= lines.arr[i]->len; j ++) {
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
            for (int j = x1; j <= lines.arr[i]->len; j ++) {
                mvwchgat(imscr, i, j, 1, A_NORMAL, HIGHLIGHTED_TEXT, NULL);
            }
        } else if (i == y2) {
            for (int j = 0; j <= x2; j ++) {
                mvwchgat(imscr, i, j, 1, A_NORMAL, HIGHLIGHTED_TEXT, NULL);
            }
        } else {
            for (int j = 0; j <= lines.arr[i]->len; j ++) {
                mvwchgat(imscr, i, j, 1, A_NORMAL, HIGHLIGHTED_TEXT, NULL);
            }
        }
    }
    return;
}

// Select a piece of text and copy to clipboard
void Copy(int starty, int startx, int endy, int endx) {
    // IDYK if i need to free the entire clipboard or not, because since im reallocating, freein the clipboard
    // will cause null pointer derefencing issues
    int miny = min(starty, endy);
    int y1, x1, y2, x2;

    // Find out starting and ending coordinates in proper manner
    if (miny == starty) { y1 = starty; x1 = startx; y2 = endy; x2 = endx;
    } else if (miny == endy) {
        y1 = endy; x1 = endx; y2 = starty; x2 = startx;
    } else {
        y1 = starty; x1 = startx; y2 = endy; x2 = endx;
    }

    // Clear the clipboard of the old data
    // If we clear only when we copy, then we can paste multiple times
    // Reallocate memory for cb
    cb.lastIndex = y2 - y1;
    line** temp = (line**) realloc(cb.arr, sizeof(line*) * (cb.lastIndex + 1));
    if (!temp) {
        ExitProgram(ERR_MEM_REALLOC_FAIL);
        return;

    }
    cb.arr = temp;
    // Set up empty memory to get data from the original line array
    for (int i = 0; i<= cb.lastIndex; i ++) {
        cb.arr[i] = Line();
    }

    // Copy the lines over properly
    for (int i = 0; i <= cb.lastIndex; i ++) {
        if (i == 0) {
            memcpy(cb.arr[i]->buf, lines.arr[y1 + i]->buf + x1, strlen(lines.arr[y1 + i]->buf + x1));
            cb.arr[i]->len += strlen(cb.arr[i]->buf);
        } else if (i == cb.lastIndex) {
            memcpy(cb.arr[i]->buf, lines.arr[y1 + i]->buf, strlen(lines.arr[y1 + i]->buf));
            memset(cb.arr[i]->buf + x2 + 1, 0, strlen(cb.arr[i]->buf + x2 + 0));
            cb.arr[i]->len += strlen(cb.arr[i]->buf);
        } else {
            memcpy(cb.arr[i]->buf, lines.arr[y1 + i]->buf, strlen(lines.arr[y1 + i]->buf));
            cb.arr[i]->len += strlen(cb.arr[i]->buf);
        }
    }

    return;
}

// Select a piece of text, copy to clipboard and delete from screen and memory
void Cut(int starty, int startx, int endy, int endx) {
    // IDYK if i need to free the entire clipboard or not, because since im reallocating, freein the clipboard
    // will cause null pointer derefencing issues
    int miny = min(starty, endy);
    int y1, x1, y2, x2;

    // Find out starting and ending coordinates in proper manner
    if (miny == starty) { y1 = starty; x1 = startx; y2 = endy; x2 = endx;
    } else if (miny == endy) {
        y1 = endy; x1 = endx; y2 = starty; x2 = startx;
    } else {
        y1 = starty; x1 = startx; y2 = endy; x2 = endx;
    }

    // Clear the clipboard of the old data
    // If we clear only when we copy, then we can paste multiple times
    // Reallocate memory for cb
    cb.lastIndex = y2 - y1;
    line** temp = (line**) realloc(cb.arr, sizeof(line*) * (cb.lastIndex + 1));
    if (!temp) {
        ExitProgram(ERR_MEM_REALLOC_FAIL);
        return;

    }
    cb.arr = temp;
    // Set up empty memory to get data from the original line array
    for (int i = 0; i<= cb.lastIndex; i ++) {
        cb.arr[i] = Line();
    }

    // Copy the lines over properly
    for (int i = 0; i <= cb.lastIndex; i ++) {
        if (i == 0) {
            memcpy(cb.arr[i]->buf, lines.arr[y1 + i]->buf + x1, strlen(lines.arr[y1 + i]->buf + x1));
            cb.arr[i]->len += strlen(cb.arr[i]->buf);
        } else if (i == cb.lastIndex) {
            memcpy(cb.arr[i]->buf, lines.arr[y1 + i]->buf, strlen(lines.arr[y1 + i]->buf));
            memset(cb.arr[i]->buf + x2 + 1, 0, strlen(cb.arr[i]->buf + x2 + 0));
            cb.arr[i]->len += strlen(cb.arr[i]->buf);
        } else {
            memcpy(cb.arr[i]->buf, lines.arr[y1 + i]->buf, strlen(lines.arr[y1 + i]->buf));
            cb.arr[i]->len += strlen(cb.arr[i]->buf);
        }
    }

    // Now delete the characters to be cut from the screen and memory
    wmove(imscr, y2, x2);
    for (int i = y2; i >= y1; i --) {
        if (i == y2) {
            for (int j = x2; j >= 0; j --) { bckspc(); }
        } else if (i == y1) {
            for (int j = lines.arr[y1]->len; j >= 0; j --) { bckspc(); }
        } else {
            for (int j = lines.arr[i]->len; j >= 0; j --) { bckspc(); }
        }
    }

    return;
}

void Paste(void) {
    
    // Paste all the lines in clipboard to the current cursor position

    int i;
    for (i = 0; i <= cb.lastIndex - 1; i ++) {  // We are going to print the last line separately to make sure
    // We dont accidentally print out another empty line at the end 
        for (int j = 0; j < cb.arr[i]->len; j ++) {
            InsertInLine(cb.arr[i]->buf[j]);
        }
        crlf();
    }
    for (int j = 0; j < cb.arr[i]->len; j ++) {
        InsertInLine(cb.arr[i]->buf[j]);
    }


    return;
}
