#include "general.h"
#include "mem.h"
#include "commandmode.h"
#include "inputmode.h"
#include "visualmode.h"

int main(int argc, char** argv) {
    // General setup
    running = true;
    if (argc > 1) { strcpy(filename, argv[1]); }
    else strcpy(filename, "Untitled");

    // Setting ESCDELAY to 50 milliseconds to avoid delay when shifting
    // modes
    ESCDELAY = 50;

    InitMainWindow();   // Main window setup
    InitSubWindows();   // inputmode window and command window setup
    refresh();          // Refresh stdscr to make windows appear
    REFRESH();          // Draw boxes, and refresh both windows
    box(imScrBg->window, 0, 0);
    mvwprintw(imScrBg->window, 0, (int)((imScrBg->wCursCols -  strlen(filename))/2), filename);
    wrefresh(imScrBg->window);  // Need to refresh this only once

    LoadFile();
    REFRESH();

    currentMode = NORMALMODE;   // Set current mode to NORMALMODE
    // Set NORMAL_TEXT color attribute as the normal writing attr
    wattron(imScr->win->window, COLOR_PAIR(NORMAL_TEXT));    
    // Main loop
    while (running){
        switch (currentMode) {
            case NORMALMODE: Message("--NORMALMODE--"); break;
            case INPUTMODE: Message("--INPUTMODE--"); break;
            default: ExitProgram(ERR_UNDEFINED_MODE); break;
        }
        currentMode = ProcessKeyhit();
    }
    endwin();   // Just in case we exit improperly
    return 0;
}


// Does whatever a keypress would do depending upon the state 
int ProcessKeyhit() {   // Returns currentMode after alteration
    int ch = wgetch(imScr->win->window);

    switch (ch) {
        case ESC: { // Needs two hits to change immediately, else waits 1 second idk why
            if (currentMode != NORMALMODE) currentMode = NORMALMODE;
            break;
        }

        // Start writing at the beginning of current line
        case I: {
            if (currentMode == NORMALMODE) {
                currentMode = INPUTMODE;
                IMSCR_CURS_X = 0;
                IMSCR_MEM_X = 0;
                IMSCR_CURS_MOVE();
            } 
            else { InsertInLine(ch); dirty = true; }
            break;
        }
        // Start writing at current cursor position
        case i: {
            if (currentMode == NORMALMODE) {
                currentMode = INPUTMODE;
            } 
            else { InsertInLine(ch); dirty = true; }
            break;
        }
        // Start writing at the end of current line
        case A: {
            if (currentMode == NORMALMODE) {
                currentMode = INPUTMODE;
                IMSCR_CURS_X = IMSCR_CURLINE->len;
                IMSCR_MEM_X = IMSCR_CURS_X;
                IMSCR_CURS_MOVE();
            }
            else { InsertInLine(ch); dirty = true; }
            break;
        }
        // Start writing at the next x position on current line
        case a: { 
            if (currentMode == NORMALMODE) {
                currentMode = INPUTMODE;
                IMSCR_CURS_X = min(IMSCR_CURS_X + 1, IMSCR_CURLINE->len);
                IMSCR_MEM_X = IMSCR_CURS_X;
                IMSCR_CURS_MOVE();
            }
            else { InsertInLine(ch); dirty = true; }
            break;
        }
        // Insert line above current line and start writing from beginning
        case O: {
            if (currentMode == NORMALMODE) {
                currentMode = INPUTMODE;
                InsertNewLineAbove();
                IMSCR_CURS_X = 0;
                IMSCR_MEM_X = 0;
                IMSCR_CURS_MOVE();
                dirty = true;
                REFRESH();
            } 
            else { InsertInLine(ch); dirty = true; }
            break;
        }
        // Insert line under current line and start writing from beginning
        case o: { 
            if (currentMode == NORMALMODE) {
                currentMode = INPUTMODE;
                
                if (IMSCR_CURS_Y > 0) {
                    InsertNewLine();
                    IMSCR_CURS_Y ++;
                    IMSCR_MEM_Y ++;
                    IMSCR_CURS_X = 0;
                    IMSCR_MEM_X = 0;
                    IMSCR_CURS_MOVE();

                    dirty = true;
                }
                REFRESH();
            } 
            else { InsertInLine(ch); dirty = true; }
            break;
        }

        // VISUALMODE
        case v: {
            if (currentMode == INPUTMODE) { dirty = true; InsertInLine(ch); break; }
            if (currentMode == VISUALMODE) { break;}

            Message("--VISUALMODE--"); 
            currentMode = VISUALMODE;

            // Can have issues when selection causes scrolling
            // NEED TO IMPLEMENT CORRECTLY RESTORING POS
            int cur_x_pos = IMSCR_CURS_X;
            int cur_y_pos = IMSCR_CURS_Y;
            int end_x_pos = cur_x_pos;
            int end_y_pos = cur_y_pos;
            
            visualmode_running = true;
            Select(BY_CHAR, &cur_y_pos, &cur_x_pos, &end_y_pos, &end_x_pos);

            // Do whatever was required here (cut/copy)
            break;
        }

        case COLON: {
            if (currentMode == NORMALMODE) {
                currentMode = COMMANDMODE;
                Message("--COMMANDMODE--> ");
                ProcessCommand();
            }
            else if (currentMode == INPUTMODE) {
                InsertInLine(':');
                dirty = true;
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
            break;
        }

        // END key
        case KEY_END: {
            break;
        }

        // Delete key
        case BS:
        case DEL: { 
            if (IMSCR_CURS_X > 0) {
                IMSCR_CURS_X --;
                IMSCR_MEM_X --;
                SHIFTLEFT(IMSCR_CURLINE_BUF, IMSCR_MEM_X, IMSCR_CURLINE->maxLen);
                IMSCR_CURLINE->len --;
                IMSCR_CURS_MOVE();
                wdelch(imScr->win->window);
                REFRESH();
                dirty = true;
            } else {
                if (IMSCR_CURS_Y > 0) {
                    SHIFTLEFT(imScr->lines, IMSCR_MEM_Y, imScr->nLines);
                    imScr->nLines --;
                    IMSCR_CURS_Y --;
                    IMSCR_MEM_Y --;
                    IMSCR_CURS_X = IMSCR_CURLINE->len;
                    IMSCR_MEM_X = IMSCR_CURS_X;
                    IMSCR_CURS_MOVE();
                    REFRESH();
                    dirty = true;
                    // Need to delete the old line both from mem and from scr
                } else if (IMSCR_MEM_Y > 0) {
                    wscrl(imScr->win->window, -1);
                    SHIFTLEFT(imScr->lines, IMSCR_MEM_Y, imScr->nLines);
                    imScr->nLines --;
                    IMSCR_MEM_Y --;
                    IMSCR_CURS_X = IMSCR_CURLINE->len;
                    IMSCR_MEM_X = IMSCR_CURS_X;
                    IMSCR_CURS_MOVE();
                    REFRESH();
                    dirty = true;
                    // Need to delete the old line both from mem and from scr
                }
            }
            break;
        }

        // ENTER key
        case CR:
        case LF: {
            if (currentMode == INPUTMODE) {
                if (IMSCR_CURS_Y < imScr->win->wCursLines - 1) { IMSCR_CRLF(); }
                else {
                    // Scroll the window up, move cursor UP by one line
                    // Insert new line from there
                    wscrl(imScr->win->window, 1);
                    IMSCR_CURS_Y -= 1;
                    IMSCR_CURS_MOVE();
                    IMSCR_CRLF();
                }
                dirty = true;
            } else if (currentMode == NORMALMODE) { }
            break;
        }

        // Navigation keys
        case k: {   // UP
            if (currentMode != INPUTMODE) { IMSCR_CURS_NAV_UP(); }
            else { InsertInLine('k'); }
            break; 
        }
        case KEY_UP: { IMSCR_CURS_NAV_UP(); break; }
        case j: { // DOWN
            if (currentMode != INPUTMODE) { IMSCR_CURS_NAV_DOWN(); }
            else { InsertInLine('j'); }
            break;
        }
        case KEY_DOWN: { IMSCR_CURS_NAV_DOWN(); break; }
        case h: {   // LEFT
            if (currentMode != INPUTMODE) { IMSCR_CURS_NAV_LEFT(); }
            else { InsertInLine('h'); }
            break;
        }
        case KEY_LEFT: { IMSCR_CURS_NAV_LEFT(); break; }
        case l: {   // RIGHT
            if (currentMode != INPUTMODE) { IMSCR_CURS_NAV_RIGHT(); }
            else { InsertInLine('l'); }
            break;
        }
        case KEY_RIGHT: { IMSCR_CURS_NAV_RIGHT(); break; } 

        default: { 
            if (currentMode == INPUTMODE) {
                InsertInLine(ch);
                dirty = true;
            } else {} 
            break;
        }
    }

    return currentMode;
}

void WriteToFile() {
    if (!strcmp(filename, "Untitled")) {
        const char* msg = "--COMMANDMODE--> Enter filename :: ";
        Message(msg);
        echo();
        mvwscanw(cmWindow->window, 1, strlen(msg), "%s", filename);
        noecho();
    }

    FILE* f = fopen(filename, "w");
    if (!f) { ExitProgram(ERR_WRITE_FAIL_FILE_OPEN_FAILURE); }

    // We count backwards because lines array works like a stack
    // I dont know why i must have made some stupid error when 
    // coding that out
    
    for (int i = 0; i < imScr->nLines; i ++) {
        fprintf(f, "%s\n", imScr->lines[i]->buf);
    }

    fclose(f);

    dirty = false;
}