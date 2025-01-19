#include "general.h"
#include "inputmode.h"
#include "mem.h"
#include "visualmode.h"

// Initialize ncurses, get stdscr data and make stdscr wrapper
void InitMainWindow() {
    // Init ncurses window screens
    initscr();
    noecho();   // Disable echoing
    cbreak();   // Enable combination inputs
    start_color();  // Enable colors
    // Define color pairs
    init_pair(HIGHLIGHTED_TEXT, COLOR_BLACK, COLOR_YELLOW); // Highlighted text
    init_pair(NORMAL_TEXT, COLOR_WHITE, COLOR_BLACK);  // Normal text
    getmaxyx(stdscr, mainWindowMaxY, mainWindowMaxX);   // Get max-coords
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
