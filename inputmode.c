#include "inputmode.h"
#include "visualmode.h"

// After loading from memory, simply prints out to imscr
void PrintLoadedFile(void) {
    wattron(imscr, COLOR_PAIR(NORMAL_TEXT));

    for (int i = 0; i <= lines.lastIndex; i ++) {
        mvwprintw(imscr, sy, sx, "%s", lines.arr[my]->buf);
        my ++; sy ++;
    }

    my = 0;
    sy = 0;
    sx = 0;
    sy = 0;
    wattroff(imscr, COLOR_PAIR(NORMAL_TEXT));
    return;
}

// Normal UP arrow key effect
void cursor_up(void) {
    if (my == 0) { mx = 0; sx = 0; }
    else if (sy == 0) {
        if (my > 0) {
            wscrl(imscr, -1);
            mvwprintw(imscr, sy, 0, "%s", lines.arr[my]->buf);
            my --;
            sx = min((visualmodeactve)? lines.arr[my]->len - 1 : lines.arr[my]->len, sx);
            mx = sx;
        } else { mx = 0; sx = 0; }
    }
    else {
        sy --;
        my --;
        sx = min((visualmodeactve)? lines.arr[my]->len - 1 : lines.arr[my]->len, sx);
        mx = sx;
    }
    wmove(imscr, sy, sx);
}


// Normal DOWN arrow key effect
// If cursor y coord is at end of the screen, scroll
// If mem y coord is at end of linearr, do nothing
void cursor_down(void) {
    if (my == lines.lastIndex) {
        if (mx == lines.arr[my]->len) {}
        else { sx = lines.arr[my]->len; mx = sx; }
    } else if (sy == imscrnlines - 1) {
        wscrl(imscr, 1);
        my ++;
        mvwprintw(imscr, sy, 0, "%s", lines.arr[my]->buf);
        sx = min(sx, (visualmodeactve)? lines.arr[my]->len - 1 : lines.arr[my]->len); 
        mx = sx;
    } else {
        my ++;
        sy ++;
        sx = min((visualmodeactve)? lines.arr[my]->len - 1 : lines.arr[my]->len, sx);
        mx = sx;
    }
    wmove(imscr, sy, sx);
}

// Normal LEFT arrow key effect
void cursor_left(void) {
    if (sx > 0) { sx --; mx = sx; }
    else if (sy > 0) {
        sy --;
        my --;
        sx = (visualmodeactve)? lines.arr[my]->len - 1 : lines.arr[my]->len;
        mx = sx;
    } else if (my > 0) {
        wscrl(imscr, -1);
        my --;
        mvwprintw(imscr, sy, 0, "%s", lines.arr[my]->buf);
        sx = (visualmodeactve)? lines.arr[my]->len - 1 : lines.arr[my]->len;
        mx = sx;
    } else { }
    wmove(imscr, sy, sx);
}

// Normal RIGHT arrow key effect
void cursor_right(void) {
    // RIGHT ARROW KEY FAULTY, SEGFAULT WHEN AT END OF FILE
    if (!visualmodeactve) {
        if (sx < lines.arr[my]->len) { sx ++; mx = sx; }
        else if ((sy < imscrnlines - 1) && (my < lines.lastIndex)) {
            sy ++;
            my ++;
            sx = 0;
            mx = sx;
        } else if (my < lines.lastIndex) {
            wscrl(imscr, 1);
            my ++;
            mvwprintw(imscr, sy, 0, "%s", lines.arr[my]->buf);
            sx = 0; mx = 0;
        } else {}
    } else {
        if (sx < lines.arr[my]->len - 1) { sx ++; mx = sx; }
        else if ((sy < imscrnlines - 1) && (my < lines.lastIndex)) {
            sy ++;
            my ++;
            sx = 0;
            mx = sx;
        } else if (my < lines.lastIndex) {
            wscrl(imscr, 1);
            my ++;
            mvwprintw(imscr, sy, 0, "%s", lines.arr[my]->buf);
            sx = 0; mx = 0;
        } else {}
    }
    wmove(imscr, sy, sx);
    return;
}

// Normal ENTER key effect
void crlf(void) {
    InsertNewLine();
    char copyBuf[1000]; memset(copyBuf, 0, sizeof(char) * 1000);
    memcpy(copyBuf, (lines.arr[my]->buf + mx), sizeof(char) * strlen(lines.arr[my]->buf + mx));
    memset((lines.arr[my]->buf + mx), 0, sizeof(char) * strlen(lines.arr[my]->buf + mx));
    wclrtoeol(imscr);
    lines.arr[my]->len -= strlen(copyBuf);
    sy ++;
    my ++;
    sx = 0;
    mx = 0;
    wmove(imscr, sy, sx);
    char c_; int i = 0;
    while ((c_ = copyBuf[i++])) { InsertInLine(c_); }
    sx = 0;
    mx = 0;
    wmove(imscr, sy, sx);
    Refresh();
}

// Wrapper for backspace key effect
void bckspc(void) {
    if (sx > 0) {
        sx --;
        mx --;
        SHIFTLEFT(lines.arr[my]->buf, mx, lines.arr[my]->alcdSiz);
        lines.arr[my]->len --;
        wmove(imscr, sy, sx);
        wdelch(imscr);
        Refresh();
        dirty = true;
    } else {
        if (sy > 0) {
            // Copy all the stuff to the right of the cursor
            char copyBuf[1000]; memset(copyBuf, 0, sizeof(char) * 1000);
            memcpy(copyBuf, (lines.arr[my]->buf), strlen(lines.arr[my]->buf));
            wdeleteln(imscr);
            // Shif all lines from the next line up by 1
            SHIFTLEFT(lines.arr, my, lines.lastIndex + 1);
            lines.lastIndex --;
            sy --;
            my --;
            sx = lines.arr[my]->len;
            int lastLineEndx = sx;
            mx = sx;
            wmove(imscr, sy, sx);
            // Paste the copied content to the upper line and screen
            for (int i = 0; i < strlen(copyBuf); i ++) {
                InsertInLine(copyBuf[i]);
            }
            sx = lastLineEndx;
            mx = sx;
            wmove(imscr, sy, sx);
            Refresh();
            dirty = true;
        } else if (my > 0) {
            wscrl(imscr, -1);
            int lastLineEndx = lines.arr[my - 1]->len;
            // Copy all the stuff to the right of the cursor
            char copyBuf[1000]; memset(copyBuf, 0, sizeof(char) * 1000);
            memcpy(copyBuf, (lines.arr[my]->buf), strlen(lines.arr[my]->buf));
            wdeleteln(imscr);
            SHIFTLEFT(lines.arr, my, lines.lastIndex);
            lines.lastIndex --;
            my --;
            sx = lines.arr[my]->len;
            mx = sx;
            wmove(imscr, sy, sx);
            // Paste the copied content to the upper line and screen
            for (int i = 0; i < strlen(copyBuf); i ++) {
                InsertInLine(copyBuf[i]);
            }
            sx = lastLineEndx;
            mx = sx;
            wmove(imscr, sy, sx);
            Refresh();
            dirty = true;
        } else {}
    }
    return;
}

// Insert a new line underneath the current line
void InsertNewLine(void) {
    // Insert the new line on screen underneath current line
    sy ++;
    wmove(imscr, sy, sx);
    winsertln(imscr);  // Inserts new line above current line and pushes everything else down
    sy--; // Go back to original line position
    wmove(imscr, sy, sx);

    // Add new line to memory if close to using up all available lines
    // Increase allocated space
    if (lines.lastIndex + 1 >= (lines.alcdSiz - 10)) {
        // add a whole window worth of lines to the previous size
        lines.alcdSiz += lines.alcdSiz; // Dont forget to double this as well
        int newSize = sizeof(line*) * lines.alcdSiz;
        line** temp = (line**) realloc(lines.arr, newSize);
        if (!temp) {
            Message("ERROR : Cannot add new line; Exiting...");
            Sleep(3);
            ExitProgram(ERR_MEM_LINE_ADD_FAIL);
        }
        lines.arr = temp;    // Set lines to new allocated mem
    }

    lines.lastIndex ++;       // Increase the number of lines currently available
    // Shift the currently stored lines down by one slot at current line pos
    my ++;
    SHIFTRIGHT(lines.arr, my, lines.lastIndex + 1);    
    lines.arr[my] = Line(); // Set new mem pos to a line pointer
    my --;; // Line we are currently on should not change
    Refresh();
    return;
}

// Insert a new line above the current line
void InsertNewLineAbove(void) {
    sx = 0;
    mx = 0;
    wmove(imscr, sy, sx);
    crlf();
    sy --;
    my --;
    wmove(imscr, sy, sx);
    return;
}

// Insert into current line
void InsertInLine(int ch) {
    SHIFTRIGHT(lines.arr[my]->buf, mx, lines.arr[my]->alcdSiz);
    lines.arr[my]->buf[mx] = ch;
    lines.arr[my]->len ++;
    wattron(imscr, COLOR_PAIR(NORMAL_TEXT));    
    winsch(imscr, ch);
    wattroff(imscr, COLOR_PAIR(NORMAL_TEXT));    
    sx ++; 
    mx ++;
    wmove(imscr, sy, sx);
    Refresh();
    dirty = true;
    return;
}

// Do all the stuff that is required here
int RunInputMode(void) {
    int keyhitresult = ProcessKeyhit();
    return keyhitresult;
}
