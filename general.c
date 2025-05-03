#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ncurses.h>
#include "commandmode.h"
#include "general.h"
#include "inputmode.h"
#include "visualmode.h"

// Refresh the screens
void Refresh(void) {
    wrefresh(cmscr);
    wrefresh(imscr);
    return;
}

// Wait for a given number of seconds
void Sleep(double seconds){
    unsigned int sec = (unsigned int)(seconds); \
    unsigned int usec = (unsigned int)((seconds - sec) * 1000000); \
    sleep(sec); \
    usleep(usec); \
    return;
}

// Save current line arr status to file in text format
void WriteToFile(void) {
    if (!strcmp(filename, "Untitled")) {
        const char* msg = "Enter filename :: ";
        Message(msg);
        echo();
        wmove(cmscr, 1, 0);
        wclrtoeol(cmscr);
        Refresh();
        mvwscanw(cmscr, 1, 0, "%s", filename);
        noecho();
    }

    FILE* f = fopen(filename, "w");
    if (!f) { ExitProgram(ERR_WRITE_FAIL_FILE_OPEN_FAILURE); }

    for (int i = 0; i <= lines.lastIndex; i ++) {
        fprintf(f, "%s\n", lines.arr[i]->buf);
    }

    fclose(f);
    dirty = false;
}

// Load raw text from file to line arr struct for writing to screen
void LoadFromFile(void) {
    if (!strcmp(filename, "Untitled")) { Message("No file opened as of now"); return; }
    
    // Try to open the file
    FILE* file = fopen(filename, "r");
    if (!file) { 
        Message("File does't exist, cannot read");
        return; 
    }  // File doesnt exist, no need to read

    char buffer[MAXBUFLEN + 1];
    memset(buffer, 0, sizeof(char) * (MAXBUFLEN + 1));
    while (fgets(buffer, MAXBUFLEN, file)) {
        buffer[strcspn(buffer, "\n")] = '\0';
        strcpy(lines.arr[my]->buf, buffer);
        lines.arr[my]->len += strlen(buffer);   
        memset(buffer, 0, sizeof(char) * (MAXBUFLEN));
        sy ++;
        my ++;
        lines.lastIndex ++;
    }
    fclose(file);   // Close the file

    // Reset the cursor position to 0, 0
    sy = 0;
    my = 0;
    sx = 0;
    sy = 0;
    return;
}

// Deallocate all memory and exit program safely
void ExitProgram(int errcode) {
    // Clear the lines array properly
    for (int i = 0; i <= lines.lastIndex; i ++) {
        free(lines.arr[i]->buf);    // Free the character buffer
        free(lines.arr[i]);         // Free the line pointer itself
    }

    running = false;
    PrintError(errcode);
    return;
}

// If there is an error of known type print that out
void PrintError(int errcode) {
    switch (errcode) {
        case ERR_NOERROR: { return; break; }
        case ERR_MEM_LINE_ADD_FAIL: { printf("Failed to add a line to memory\n"); break;}
        case ERR_UNDEFINED_MODE: { printf("Undefined mode variable %d \n", currentMode); break; }
        case ERR_WRITE_FAIL_FILE_OPEN_FAILURE: { printf("Could not open file %s to write to\n", filename); break; }
        default: return;
    }
    return;
}

// Return a new, empty line pointer
line* Line(void) {
    line* newLine = (line*) malloc(sizeof(line));
    newLine->buf = (char*) calloc(imscrncols, sizeof(char));
    newLine->len = 0;
    newLine->alcdSiz = imscrncols;
    return newLine;
}

// Print msg to command window
void Message(const char* msg) {
    wmove(cmscr, 0, 0);
    wclrtoeol(cmscr);
    mvwprintw(cmscr, 0, 0, msg);
    Refresh();
}

// Print a short-lived message to the console
void QuickMessage(const char* msg) {
    Message(msg);\
    Sleep(0.5);\
    wmove(cmscr, 0, 0);\
    wclrtoeol(cmscr);\
    Refresh();\
    currentMode = NORMALMODE;\
    Message("--NORMALMODE--");\
}

// Basic tasks
int RunNormalMode(void) {
    int keyhitresult = ProcessKeyhit();
    return keyhitresult;
}

// Get a keyhit and do the related action
int ProcessKeyhit(void) {
    int ch = wgetch(imscr);

    switch (ch) {
        case ESC: { 
            return SIGNAL_SWITCH_TO_NORMALMODE;
            break;
        }

        // Start writing at the beginning of current line
        case I: {
            if (currentMode == NORMALMODE) {
                sx = 0;
                mx  = 0;
                wmove(imscr, sy, sx);
                return SIGNAL_SWITCH_TO_INPUTMODE;
            } 
            else { 
                InsertInLine(ch); 
                dirty = true; 
                return SIGNAL_SWITCH_TO_INPUTMODE;
            }
            break;
        }
        // Start writing at current cursor position
        case i: {
            if (currentMode == NORMALMODE) {
                return SIGNAL_SWITCH_TO_INPUTMODE;
            } 
            else { 
                InsertInLine(ch); 
                dirty = true; 
                return SIGNAL_SWITCH_TO_INPUTMODE;
            }
            break;
        }
        // Start writing at the end of current line
        case A: {
            if (currentMode == NORMALMODE) {
                sx = lines.arr[my]->len;
                mx = sx;
                wmove(imscr, sy, sx);
                return SIGNAL_SWITCH_TO_INPUTMODE;
            }
            else { 
                InsertInLine(ch); 
                dirty = true;
                return SIGNAL_SWITCH_TO_INPUTMODE;
            }
            break;
        }
        // Start writing at the next x position on current line
        case a: { 
            if (currentMode == NORMALMODE) {
                sx = min(sx + 1, lines.arr[my]->len);
                mx = sx;
                wmove(imscr, sy, sx);
                return SIGNAL_SWITCH_TO_INPUTMODE;
            }
            else { 
                InsertInLine(ch); 
                dirty = true;
                return SIGNAL_SWITCH_TO_INPUTMODE;
            }
            break;
        }
        // Insert line above current line and start writing from beginning
        case O: {
            if (currentMode == NORMALMODE) {
                InsertNewLineAbove();
                sx = 0;
                mx = 0;
                wmove(imscr, sy, sx);
                dirty = true;
                Refresh();
                return SIGNAL_SWITCH_TO_INPUTMODE;
            } 
            else { 
                InsertInLine(ch); 
                dirty = true;
                return SIGNAL_SWITCH_TO_INPUTMODE;
            }
            break;
        }
        // Insert line under current line and start writing from beginning
        case o: { 
            if (currentMode == NORMALMODE) {
                if (sy > 0) {
                    InsertNewLine();
                    sy ++;
                    my ++;
                    sx = 0;
                    mx = 0;
                    wmove(imscr, sy, sx);
                    dirty = true;
                }
                Refresh();
                return SIGNAL_SWITCH_TO_INPUTMODE;
            } 
            else { 
                InsertInLine(ch);
                dirty = true;
                return SIGNAL_SWITCH_TO_INPUTMODE;
            }
            break;
        }

        // VISUALMODE
        case v: {
            // Do whatever was required here (cut/copy)
            if (currentMode == NORMALMODE) return SIGNAL_SWITCH_TO_VISUALMODE;
            else { InsertInLine('v'); dirty = true; return SIGNAL_SWITCH_TO_INPUTMODE; }
            break;
        }
        case V: {
            if (currentMode == NORMALMODE) return SIGNAL_SWITCH_TO_VISUALMODE;
            else { InsertInLine('V'); dirty = true; return SIGNAL_SWITCH_TO_INPUTMODE; }
            break;
        }

        case c: {
            // If in visualmode, then copy the selected text and return to normalmode
            // Else insert 'c' at current cursor position
            if (currentMode == VISUALMODE) { return SIGNAL_COPY_SELECTED_TEXT_AND_SWITCH_TO_NORMALMODE; }
            else { InsertInLine('c'); dirty = true; return SIGNAL_SWITCH_TO_INPUTMODE; }
            break;
        }

        case x: {
            // If in visualmode, then cut the selected text and return to normalmode
            // Else insert 'x' at current cursor position
            if (currentMode == VISUALMODE) { return SIGNAL_CUT_SELECTED_TEXT_AND_SWITCH_TO_NORMALMODE; }
            else { InsertInLine('c'); dirty = true; return SIGNAL_SWITCH_TO_INPUTMODE; }
            break;
        }

        case p: {
            if (currentMode == NORMALMODE) {
                Paste();
                return SIGNAL_SWITCH_TO_NORMALMODE;
            } else if (currentMode == VISUALMODE) {
                // NEED TO DO SOME EXTRA STUFF HERE
                Paste();
                return SIGNAL_SWITCH_TO_NORMALMODE;
            } else if (currentMode == INPUTMODE) {
                InsertInLine('p');
                dirty = true;
                return SIGNAL_SWITCH_TO_INPUTMODE;
            }
            break;
        }
        
        case P: {
            if (currentMode == NORMALMODE) {
                Paste();
                return SIGNAL_SWITCH_TO_NORMALMODE;
            } else if (currentMode == VISUALMODE) {
                // NEED TO DO SOME EXTRA STUFF HERE
                Paste();
                return SIGNAL_SWITCH_TO_NORMALMODE;
            } else if (currentMode == INPUTMODE) {
                InsertInLine('P');
                dirty = true;
                return SIGNAL_SWITCH_TO_INPUTMODE;
            }
            break;
        }


        case COLON: {
            if (currentMode == NORMALMODE) {
                return SIGNAL_SWITCH_TO_COMMANDMODE;
            }
            else if (currentMode == INPUTMODE) {
                InsertInLine(':');
                dirty = true;
                return SIGNAL_SWITCH_TO_INPUTMODE;
            }
            break;
        }

        // Tab key
        case TAB: {
            if (currentMode == INPUTMODE) { 
                for (int i = 0; i < 4; i ++) {
                    InsertInLine(' ');
                }
            }
            return SIGNAL_SWITCH_TO_INPUTMODE;
            break;
        }

        // END key
        case KEY_END: {
            return SIGNAL_SWITCH_TO_INPUTMODE;
            break;
        }

        // Delete key
        case BS:
        case DEL: { 
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
                    SHIFTLEFT(lines.arr, my, lines.lastIndex + 1);
                    lines.lastIndex --;
                    sy --;
                    my --;
                    sx = lines.arr[my]->len - 1;
                    mx = sx;
                    wmove(imscr, sy, sx);
                    Refresh();
                    dirty = true;
                } else if (my > 0) {
                    wscrl(imscr, -1);
                    SHIFTLEFT(lines.arr, my, lines.lastIndex);
                    lines.lastIndex --;
                    my --;
                    sx = lines.arr[my]->len - 1;
                    mx = sx;
                    wmove(imscr, sy, sx);
                    Refresh();
                    dirty = true;
                }
            }
            return SIGNAL_SWITCH_TO_INPUTMODE;
            break;
        }

        // ENTER key
        case CR:
        case LF: {
            if (currentMode == INPUTMODE) {
                if (sy < imscrnlines - 1) { crlf(); }
                else {
                    // Scroll the window up, move cursor UP by one line
                    // Insert new line from there
                    wscrl(imscr, 1);
                    sy -= 1;
                    wmove(imscr, sy, sx);
                    crlf();
                }
                dirty = true;
                return SIGNAL_SWITCH_TO_INPUTMODE;
            } else if (currentMode == NORMALMODE) { 
                cursor_down();
                return SIGNAL_SWITCH_TO_NORMALMODE;
            }
            break;
        }

        // Navigation keys
        case k: {   // UP
            if (currentMode != INPUTMODE) { 
                cursor_up(); 
                return (currentMode == NORMALMODE)? SIGNAL_SWITCH_TO_NORMALMODE : SIGNAL_SWITCH_TO_VISUALMODE;
            }
            else { 
                InsertInLine('k'); 
                return SIGNAL_SWITCH_TO_INPUTMODE;
            }
            break; 
        }
        case KEY_UP: { 
            cursor_up(); 
            return (currentMode == NORMALMODE)? SIGNAL_SWITCH_TO_NORMALMODE : \
            (currentMode == INPUTMODE)? SIGNAL_SWITCH_TO_INPUTMODE : SIGNAL_SWITCH_TO_VISUALMODE;
            break; 
        }
        case j: { // DOWN
            if (currentMode != INPUTMODE) { 
                cursor_down();
                return (currentMode == NORMALMODE)? SIGNAL_SWITCH_TO_NORMALMODE : SIGNAL_SWITCH_TO_VISUALMODE;
            }
            else { 
                InsertInLine('j'); 
                return SIGNAL_SWITCH_TO_INPUTMODE;
            }
            break;
        }
        case KEY_DOWN: { 
            cursor_down(); 
            return (currentMode == NORMALMODE)? SIGNAL_SWITCH_TO_NORMALMODE : \
            (currentMode == INPUTMODE)? SIGNAL_SWITCH_TO_INPUTMODE : SIGNAL_SWITCH_TO_VISUALMODE;
            break; 
        }
        case h: {   // LEFT
            if (currentMode != INPUTMODE) { 
                cursor_left();
                return (currentMode == NORMALMODE)? SIGNAL_SWITCH_TO_NORMALMODE : SIGNAL_SWITCH_TO_VISUALMODE;
            }
            else { 
                InsertInLine('h');
                return SIGNAL_SWITCH_TO_INPUTMODE;
            }
            break;
        }
        case KEY_LEFT: { 
            cursor_left();
            return (currentMode == NORMALMODE)? SIGNAL_SWITCH_TO_NORMALMODE : \
            (currentMode == INPUTMODE)? SIGNAL_SWITCH_TO_INPUTMODE : SIGNAL_SWITCH_TO_VISUALMODE;
            break;
        }
        case l: {   // RIGHT
            if (currentMode != INPUTMODE) {
                cursor_right();
                return (currentMode == NORMALMODE)? SIGNAL_SWITCH_TO_NORMALMODE : SIGNAL_SWITCH_TO_VISUALMODE;
            }
            else { 
                InsertInLine('l');
                return SIGNAL_SWITCH_TO_INPUTMODE;
            }
            break;
        }
        case KEY_RIGHT: {
            cursor_right();
            return (currentMode == NORMALMODE)? SIGNAL_SWITCH_TO_NORMALMODE : \
            (currentMode == INPUTMODE)? SIGNAL_SWITCH_TO_INPUTMODE : SIGNAL_SWITCH_TO_VISUALMODE;
            break;
        } 

        default: { 
            if (currentMode == INPUTMODE) {
                InsertInLine(ch);
                dirty = true;
                return SIGNAL_SWITCH_TO_INPUTMODE;
            } else { return SIGNAL_SWITCH_TO_NORMALMODE; } 
            break;
        }
    }

    return SIGNAL_SWITCH_TO_NORMALMODE;
}
