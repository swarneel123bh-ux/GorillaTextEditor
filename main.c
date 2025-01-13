#include "general.h"
#include "inputmode.h"  // Have to include this to avoid cicular inclusions

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

// Initialize ncurses, get stdscr data and make stdscr wrapper
void InitMainWindow() {
    initscr();
    noecho();
    cbreak();
    getmaxyx(stdscr, mainWindowMaxY, mainWindowMaxX);
    // Reduce both max X and Y because of some weird behaviour that i cannot understand
    mainWindowMaxY --;
    mainWindowMaxX --;
    mainWindow = NewWindow(NULL, 0, 0, mainWindowMaxY, mainWindowMaxX);
    ClipBoard = (clipboard*) malloc (sizeof(clipboard));
    ClipBoard->buf = (char*) malloc (sizeof(char) * MAX_CLIPBOARD_BUFLEN);
    keypad(stdscr, true); // Set keypad to true to allow cursor movement when in normalmode
}

// Initialize imScr and command window
void InitSubWindows() {
    imScr = (imscr*) malloc(sizeof(imscr));
    imScr->win = NewWindow(filename, 1, 1, mainWindow->wEndY - 4, mainWindow->wEndX - 1);
    keypad(imScr->win->window, true);   // Set keypad to true for inputmode window
    scrollok(imScr->win->window, TRUE); // Set scrolling for imscr window
    imScrBg = NewWindow(NULL, 0, 0, mainWindow->wEndY - 3, mainWindow->wEndX); box(imScrBg->window, 0, 0);
    cmWindow = NewWindow(NULL, imScr->win->wEndY + 2, 0, BOTTOM_RIGHT);
    imScr->nPossibleLines = imScr->win->wCursLines;
    imScr->lines = (line**) malloc(sizeof(line*) * imScr->nPossibleLines);
    imScr->lines[0] = NewLine();
    imScr->nLines = 1;
}

// If opened file exists, then load all the data into mem and scr
void LoadFile() {
    if (!strcmp(imScr->win->title, "Untitled")) return;
    
    // Try to open the file
    FILE* file = fopen(imScr->win->title, "r");
    if (!file) { 
        Message("File does't exist, cannot read");
        return; 
    }  // File doesnt exist, no need to read

    char buffer[MAX_LINE_BUFLEN + 1];
    memset(buffer, 0, sizeof(char) * (MAX_LINE_BUFLEN + 1));
    while (fgets(buffer, MAX_LINE_BUFLEN, file)) {
        // Remove the newline character if present
        buffer[strcspn(buffer, "\n")] = '\0';
        // Since screen starts empty, first add the line then do
        // CRLF routine
        // Copy into the buffer 
        strcpy(IMSCR_CURLINE_BUF, buffer);
        // Fix the len
        IMSCR_CURLINE->len += strlen(buffer);   
        // Print the line out
        mvwprintw(imScr->win->window, IMSCR_CURS_POS, "%s", buffer); 
        memset(buffer, 0, sizeof(char) * (MAX_LINE_BUFLEN + 1));
        InsertNewLine();
        IMSCR_CURS_Y ++;
        IMSCR_MEM_Y ++;
    }

    fclose(file);   // Close the file

    // Reset the cursor position to 0, 0
    IMSCR_CURS_Y = 0;
    IMSCR_MEM_Y = 0;
    IMSCR_CURS_X = 0;
    IMSCR_MEM_X = 0;
    IMSCR_CURS_MOVE();
}

// Returns a window wrapper
subWin* NewWindow(char* title, int begY, int begX, int endY, int endX) {
    subWin* win = (subWin*) malloc(sizeof(subWin));
    win->wBegY = begY;
    win->wBegX = begX;
    win->wEndY = endY;
    win->wEndX = endX;
    win->wCursY = 0;
    win->wCursX = 0;
    win->wCursLines = win->wEndY - win->wBegY + 1;
    win->wCursCols = win->wEndX - win->wBegX + 1;
    win->window = newwin(win->wCursLines, win->wCursCols, win->wBegY, win->wBegX);
    if (title) win->title = title;
    else win->title = "";
    return win;
}

// Returns a new line
line* NewLine() {
    line* l = (line*) malloc(sizeof(line));
    l->len = 0;
    l->maxLen = imScr->win->wCursCols;
    l->buf = (char*) malloc(sizeof(char) * l->maxLen);
    memset(l->buf, 0, l->maxLen * sizeof(char));
    return l;
}

// Print msg out to command window
void Message(const char* msg) {
    wmove(cmWindow->window, 1, 1);
    wclrtoeol(cmWindow->window);
    mvwprintw(cmWindow->window, 1, 1, msg);
    REFRESH();
}

// Does whatever a keypress would do depending upon the state 
int ProcessKeyhit() {   // Returns currentMode after alteration
    int ch = wgetch(imScr->win->window);

    switch (ch) {
        case ESC: { // Needs two hits to change immediately, else waits 1 second idk why
            if (currentMode != NORMALMODE) currentMode = NORMALMODE;
            break;
        }

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
        case i: {
            if (currentMode == NORMALMODE) {
                currentMode = INPUTMODE;
            } 
            else { InsertInLine(ch); dirty = true; }
            break;
        }

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

        // NEED TO INCLUDE SCROLLING HERE
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

// Get a command and do the related task
void ProcessCommand() {
    char command[100];
    echo();
    mvwscanw(cmWindow->window, 1, 17, "%s", command);
    noecho();

    if (!strcmp(command, "q")) {
        if (dirty) {
            QUICKMESSAGE("--Unwritten changes exist, use :w first--"); }
        else { ExitProgram(ERR_NOERROR); }
    }
    if (!strcmp(command, "q!")) ExitProgram(ERR_NOERROR); 
    if (!strcmp(command, "w")) { WriteToFile(); }
    if (!strcmp(command, "wq")) { WriteToFile(); ExitProgram(ERR_NOERROR); }
    else { 
        QUICKMESSAGE("--COMMANDMODE--> Invalid command")
        currentMode = NORMALMODE;
        return;
    }
}

// Deallocate all memory and exit safely
void ExitProgram(int errcode) {
    // Free each line's buffer and the line itself
    for (int i = 0; i == imScr->nLines - 1; i ++) { free(imScr->lines[i]->buf); free(imScr->lines[i]); }
    free(imScr->lines); // Free the line array malloced
    free(imScr->win);   // Free the subWin object
    free(imScr);    // Free imscr object
    free(cmWindow); // Free the subWin object
    free(imScrBg);  // Free the background subWin pointer
    free(ClipBoard->buf);   // Free the clipboad buffer
    free(ClipBoard);    // Free the clipboard pointer
    running = false;
    endwin();

    free(mainWindow);
    if (errcode != 0) { PrintError(errcode); }
    exit(errcode);
}

// Print out any error that happened
void PrintError(int errcode) {
    switch (errcode) {
        case ERR_NOERROR: { printf("Exited successfully\n"); break; }
        case ERR_MEM_LINE_ADD_FAIL: {
            printf("Exit with ERRORS:: Could not add new line. realloc() returned NULL pointer\n");
            break;
        }
        case ERR_WRITE_FAIL_FILE_OPEN_FAILURE: {
            printf("Exit with ERRORS:: Failed to open file for writing\n");
            break;
        }
        case ERR_UNDEFINED_MODE: {
            printf("Exit with ERRORS:: currentMode variable undefined\n");
            break;
        }

        default: { break; } // Default case is redundant
    }
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