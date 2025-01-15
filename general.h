#ifndef GENERAL_H
#define GENERAL_H

#include <ncurses.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

// Constants
#define MAX_CLIPBOARD_BUFLEN 1000
#define MAX_LINE_BUFLEN imScr->win->wCursCols
#define NORMALMODE -1
#define COMMANDMODE 0
#define INPUTMODE 1
#define VISUALMODE 2

// Error codes
#define ERR_UNDEFINED_MODE -1
#define ERR_NOERROR 0
#define ERR_MEM_LINE_ADD_FAIL 1
#define ERR_WRITE_FAIL_FILE_OPEN_FAILURE 2

// Macros for shortcuts
#define MAX_Y mainWindow->wEndY
#define MAX_X mainWindow->wEndX
#define BOTTOM_RIGHT MAX_Y, MAX_X
#define TOP_LEFT 0, 0
#define min(a, b) (a < b)? a : b
#define max(a, b) (a > b)? a : b

// Refresh both cmWindow and imScr->win
#define REFRESH() {\
box(cmWindow->window, 0, 0); wrefresh(cmWindow->window);\
wrefresh(imScr->win->window);\
} 

// Sleep for given number of seconds
#define SLEEP(seconds) do { \
    unsigned int sec = (unsigned int)(seconds); \
    unsigned int usec = (unsigned int)((seconds - sec) * 1000000); \
    sleep(sec); \
    usleep(usec); \
} while (0);

// Some window constants
char filename[FILENAME_MAX];    // Name of currently opened file
bool running;                   // Exit flag
bool dirty;                     // Write-require check
int currentMode;                // State flag
int mainWindowMaxX;             // Max possible X coord
int mainWindowMaxY;             // Max possible Y coord
// General Functions
int ProcessKeyhit();   // Get a keyhit and do corresponding task depending on mode


#endif // GENERAL_H