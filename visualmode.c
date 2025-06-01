#include "visualmode.h"
#include "general.h"
#include "commandmode.h"
#include "inputmode.h"

// Do all the stuff, including highlighting, copy/cut to clipboard (NEED TO DEFINE CLIPBOARD)
int RunVisualMode(void) {
    visualmodeactve = 1;
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
    visualmodeactve = 0;
    return SIGNAL_SWITCH_TO_NORMALMODE;
}

// Unhighlight everything, used when changing modes
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
    int y1, x1, y2, x2;
    if (endy < starty) {
        y1 = endy;
        x1 = endx;
        y2 = starty;
        x2 = startx;
    } else if (endy > starty) {
        y1 = starty;
        x1 = startx;
        y2 = endy;
        x2 = endx;
    } else { // y1 == y2
        y1 = starty;
        x1 = min(startx, endx);
        y2 = endy;
        x2 = max(startx, endx);
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
    int y1, x1, y2, x2;
    if (endy < starty) {
        y1 = endy;
        x1 = endx;
        y2 = starty;
        x2 = startx;
    } else if (endy > starty) {
        y1 = starty;
        x1 = startx;
        y2 = endy;
        x2 = endx;
    } else { // y1 == y2
        y1 = starty;
        x1 = min(startx, endx);
        y2 = endy;
        x2 = max(startx, endx);
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

    /*
    FIGURE OUT HOW TO GET THE TOPLEFT AND BOTTOMRIGHT COORDS
    */    


    int y1, x1, y2, x2;
    if (endy < starty) {
        y1 = endy;
        x1 = endx;
        y2 = starty;
        x2 = startx;
    } else if (endy > starty) {
        y1 = starty;
        x1 = startx;
        y2 = endy;
        x2 = endx;
    } else { // y1 == y2
        y1 = starty;
        x1 = min(startx, endx);
        y2 = endy;
        x2 = max(startx, endx);
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
    for (int i = 0; i <= cb.lastIndex; i ++) {
        cb.arr[i] = Line();
    }

    for (int i = 0; i <= cb.lastIndex; i ++){
        if (i == 0){
            if (y1 == y2) { memcpy(cb.arr[i]->buf, lines.arr[y1 + i]->buf + x1, x2 - x1 + 1); }
            else { memcpy(cb.arr[i]->buf, lines.arr[y1 + i]->buf + x1, lines.arr[y1 + i]->len - x1); }
            cb.arr[i]->len += strlen(cb.arr[i]->buf);
        } else if (i == cb.lastIndex) {
            memcpy(cb.arr[i]->buf, lines.arr[y1 + i]->buf, x2 + 1);
            cb.arr[i]->len += strlen(cb.arr[i]->buf);
        } else {
            memcpy(cb.arr[i]->buf, lines.arr[y1 + i]->buf, lines.arr[y1 + i]->len - 1);
            cb.arr[i]->len += strlen(cb.arr[i]->buf);
        }
    }

    return;
}

// Select a piece of text, copy to clipboard and delete from screen and memory
void Cut(int starty, int startx, int endy, int endx) {
    /*
    FIGURE OUT HOW TO GET THE TOPLEFT AND BOTTOMRIGHT COORDS
    */    

    int y1, x1, y2, x2;
    if (endy < starty) {
        y1 = endy;
        x1 = endx;
        y2 = starty;
        x2 = startx;
    } else if (endy > starty) {
        y1 = starty;
        x1 = startx;
        y2 = endy;
        x2 = endx;
    } else { // y1 == y2
        y1 = starty;
        x1 = min(startx, endx);
        y2 = endy;
        x2 = max(startx, endx);
    }

    FILE* f = fopen("Coords.txt", "w");
    if (f) {
        fprintf(f, "(y1, x1) = (%d, %d)\n(y2, x2) = (%d, %d)\n", y1, x1, y2, x2);
        fclose(f);
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
    for (int i = 0; i <= cb.lastIndex; i ++) {
        cb.arr[i] = Line();
    }

    for (int i = 0; i <= cb.lastIndex; i ++){
        if (i == 0){
            if (y1 == y2) { memcpy(cb.arr[i]->buf, lines.arr[y1 + i]->buf + x1, x2 - x1 + 1); }
            else { memcpy(cb.arr[i]->buf, lines.arr[y1 + i]->buf + x1, lines.arr[y1 + i]->len - x1); }
            cb.arr[i]->len += strlen(cb.arr[i]->buf);
        } else if (i == cb.lastIndex) {
            memcpy(cb.arr[i]->buf, lines.arr[y1 + i]->buf, x2 + 1);
            cb.arr[i]->len += strlen(cb.arr[i]->buf);
        } else {
            memcpy(cb.arr[i]->buf, lines.arr[y1 + i]->buf, lines.arr[y1 + i]->len - 1);
            cb.arr[i]->len += strlen(cb.arr[i]->buf);
        }
    }

    // Now delete the characters to be cut from the screen and memory
    // Decide the cases, based on where to go
    // Basic idea is we travel from bottom right corner of selection to the top left of the selection

    /*
    1. If y1 == y2, x2 < lines.arr[y1]->len :: Move to (y2, x2 + 1)
    2. If y1 == y2, x2 == lines.arr[y1]->len :: 
        2.1. If y2 < lines.lastIndex :: move to (y2 + 1, 0)
        2.2. If y2 == lines.lastIndex :: move to (y2, x2 + 1)
    3. Case 3 : y2 > y1 :: Same as subcases 2.1 and 2.2
    */

    if (x2 == lines.arr[y2]->len - 1) {
        // memory coords MAY OR MAY NOT BE THE SAME AS SCREEN COORDS
        if (y2 == lines.lastIndex) {
            sy = y2;
            my = sy;
            sx = x2 + 1;
            mx = sx;
        } else {
            sy = y2 + 1;
            my = sy;
            sx = 0;
            mx = sx;
        }
    } else {
        sy = y2;
        my = sy;
        sx = x2 + 1;
        mx = sx;
    }

    wmove(imscr, sy, sx);

    // Find the number of characters to be deleted
    int nbckspcs = 0;
    for (int i = 0; i <= cb.lastIndex; i ++) {
        nbckspcs += (cb.arr[i]->len);
    }

    // Make as many backspace calls as calculated
    // Log all the coords where a bckspc was called
    f = fopen("Coords.txt", "w");
    if (f) {
        for (int i = 0; i <= nbckspcs; i ++) {
            fprintf(f, "bckspc :: (y, x) = (%d, %d)\n", sy, sx);
            bckspc();
        }
        fclose(f);
    }

    Refresh();
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
    // No need to insert another line after the last one
    for (int j = 0; j < cb.arr[i]->len; j ++) {
        InsertInLine(cb.arr[i]->buf[j]);
    }

    return;
}
