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

// Macros for general tasks
// Given an index, shift left
#define SHIFTLEFT(array, index, size) do { \
memmove(&(array)[index], &(array)[(index) + 1], ((size) - (index) - 1) * sizeof((array)[0])); \
} while (0) 

// Given an index, shift right without fill
#define SHIFTRIGHT(array, index, size) do { \
if ((size) > (index) + 1) { memmove(&(array)[(index) + 1], &(array)[index], ((size) - (index) - 1) * sizeof((array)[0])); } \
} while (0) 

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

/*
Show a message for a brief period of time and stop
Execution during that period
*/
#define QUICKMESSAGE(msg) {\
    Message(msg);\
    SLEEP(0.5);\
    wmove(cmWindow->window, 1, 1);\
    wclrtoeol(cmWindow->window);\
    REFRESH();\
    currentMode = NORMALMODE;\
    Message("--NORMALMODE--");\
}

// Some window constants
char filename[FILENAME_MAX];    // Name of currently opened file
bool running;                   // Exit flag
bool dirty;                     // Write-require check
int currentMode;                // State flag
int mainWindowMaxX;             // Max possible X coord
int mainWindowMaxY;             // Max possible Y coord

// subWindow
typedef struct subWindow {
    int wBegY;
    int wBegX;
    int wEndY;
    int wEndX;
    int wCursLines;
    int wCursCols;
    int wCursY; 
    int wCursX;
    WINDOW* window;
    char* title;
} subWin;
subWin* NewWindow(char* filename, int begY, int begX, int endY, int endX);

// Single line structure for text
typedef struct inputModeLine {
    int len;    // Min = 0
    int maxLen; // Equal to max usable columns (but is not same as cursor index)
    char* buf;  // Character aray
} line;
line* NewLine();    // Returns a completely empty line (in memory only)

subWin* mainWindow; // stdscr wrapper as main window
subWin* cmWindow;   // command window wrapper
subWin* imScrBg;    // Bagcground to the imScr window, so that the border doesnt move

typedef struct Clipboard{
    int contentLen;
    char* buf;
} clipboard;
clipboard* ClipBoard;

// General Functions
void InitMainWindow();  // Get all data for the stdscr
void InitSubWindows();  // Get and setup all data for the input and command windows
void LoadFile();        // If opened file exists then load all the data into the memory and screen
void ProcessKeyhit();   // Get a keyhit and do corresponding task depending on mode
void ProcessCommand();  // Get a command and do related task
void ExitProgram(int errcode);  // Deallocate all memory and exit program safely
void PrintError(int errcode);   // If there is an error of known type print that out 
void Message(const char* msg);  // Print msg to command window
void WriteToFile();

// Ascii keycodes
enum ASCII {
    NUL = 0,    // Null character
    SOH = 1,    // Start of Header
    STX = 2,    // Start of Text
    ETX = 3,    // End of Text
    EOT = 4,    // End of Transmission
    ENQ = 5,    // Enquiry
    ACK = 6,    // Acknowledge
    BEL = 7,    // Bell
    BS  = 8,    // Backspace
    TAB = 9,    // Horizontal Tab
    LF  = 10,   // Line Feed
    VT  = 11,   // Vertical Tab
    FF  = 12,   // Form Feed
    CR  = 13,   // Carriage Return
    SO  = 14,   // Shift Out
    SI  = 15,   // Shift In
    DLE = 16,   // Data Link Escape
    DC1 = 17,   // Device Control 1 (XON)
    DC2 = 18,   // Device Control 2
    DC3 = 19,   // Device Control 3 (XOFF)
    DC4 = 20,   // Device Control 4
    NAK = 21,   // Negative Acknowledge
    SYN = 22,   // Synchronous Idle
    ETB = 23,   // End of Transmission Block
    CAN = 24,   // Cancel
    EM  = 25,   // End of Medium
    SUB = 26,   // Substitute
    ESC = 27,   // Escape
    FS  = 28,   // File Separator
    GS  = 29,   // Group Separator
    RS  = 30,   // Record Separator
    US  = 31,   // Unit Separator
    SPACE = 32, // Space
    EXCLAMATION = 33,   // !
    DOUBLE_QUOTE = 34,  // "
    HASH = 35,          // #
    DOLLAR = 36,        // $
    PERCENT = 37,       // %
    AMPERSAND = 38,     // &
    SINGLE_QUOTE = 39,  // '
    LEFT_PAREN = 40,    // (
    RIGHT_PAREN = 41,   // )
    ASTERISK = 42,      // *
    PLUS = 43,          // +
    COMMA = 44,         // ,
    MINUS = 45,         // -
    PERIOD = 46,        // .
    SLASH = 47,         // /
    ZERO = 48,          // 0
    ONE = 49,           // 1
    TWO = 50,           // 2
    THREE = 51,         // 3
    FOUR = 52,          // 4
    FIVE = 53,          // 5
    SIX = 54,           // 6
    SEVEN = 55,         // 7
    EIGHT = 56,         // 8
    NINE = 57,          // 9
    COLON = 58,         // :
    SEMICOLON = 59,     // ;
    LESS_THAN = 60,     // <
    EQUALS = 61,        // =
    GREATER_THAN = 62,  // >
    QUESTION = 63,      // ?
    AT = 64,            // @
    A = 65,             // A
    B = 66,             // B
    C = 67,             // C
    D = 68,             // D
    E = 69,             // E
    F = 70,             // F
    G = 71,             // G
    H = 72,             // H
    I = 73,             // I
    J = 74,             // J
    K = 75,             // K
    L = 76,             // L
    M = 77,             // M
    N = 78,             // N
    O = 79,             // O
    P = 80,             // P
    Q = 81,             // Q
    R = 82,             // R
    S = 83,             // S
    T = 84,             // T
    U = 85,             // U
    V = 86,             // V
    W = 87,             // W
    X = 88,             // X
    Y = 89,             // Y
    Z = 90,             // Z
    LEFT_BRACKET = 91,  // [
    BACKSLASH = 92,     // \
    RIGHT_BRACKET = 93, // ]
    CARET = 94,         // ^
    UNDERSCORE = 95,    // _
    GRAVE = 96,         // `
    a = 97,             // a
    b = 98,             // b
    c = 99,             // c
    d = 100,            // d
    e = 101,            // e
    f = 102,            // f
    g = 103,            // g
    h = 104,            // h
    i = 105,            // i
    j = 106,            // j
    k = 107,            // k
    l = 108,            // l
    m = 109,            // m
    n = 110,            // n
    o = 111,            // o
    p = 112,            // p
    q = 113,            // q
    r = 114,            // r
    s = 115,            // s
    t = 116,            // t
    u = 117,            // u
    v = 118,            // v
    w = 119,            // w
    x = 120,            // x
    y = 121,            // y
    z = 122,            // z
    LEFT_CURLY = 123,   // {
    PIPE = 124,         // |
    RIGHT_CURLY = 125,  // }
    TILDE = 126,        // ~
    DEL = 127           // Delete
};

#endif // GENERAL_H