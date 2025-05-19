#ifndef GENERAL_H
#define GENERAL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <ncurses/ncurses.h>

// Constants
#define MAXBUFLEN 1000  // Maximum size any buffer can have, redefine this constant when required
#define MAXCLIPBOARDBUFLEN 100  // Maximum size the clipboard buffer can have
#define NORMALMODE 0
#define COMMANDMODE 1
#define INPUTMODE 2
#define VISUALMODE 3

// Signals
#define SIGNAL_EXIT_PROGRAM 0
#define SIGNAL_SWITCH_TO_NORMALMODE 1
#define SIGNAL_SWITCH_TO_INPUTMODE 2
#define SIGNAL_SWITCH_TO_VISUALMODE 3
#define SIGNAL_SWITCH_TO_COMMANDMODE 4
#define SIGNAL_COPY_SELECTED_TEXT_AND_SWITCH_TO_NORMALMODE 5
#define SIGNAL_CUT_SELECTED_TEXT_AND_SWITCH_TO_NORMALMODE 6

// highlighting colors
#define NORMAL_TEXT 1
#define HIGHLIGHTED_TEXT 2

// Error codes
#define ERR_NOERROR 0
#define ERR_UNDEFINED_MODE 1
#define ERR_MEM_LINE_ADD_FAIL 2
#define ERR_WRITE_FAIL_FILE_OPEN_FAILURE 3
#define ERR_MEM_REALLOC_FAIL 4

// Macros for shortcuts
#define min(a, b) (a < b)? a : b
#define max(a, b) (a > b)? a : b

#define SHIFTLEFT(array, index, size) do { \
memmove(&(array)[index], &(array)[(index) + 1], ((size) - (index) - 1) * sizeof((array)[0])); \
} while (0) 

#define SHIFTRIGHT(array, index, size) do { \
if ((size) > (index) + 1) { memmove(&(array)[(index) + 1], &(array)[index], ((size) - (index) - 1) * sizeof((array)[0])); } \
} while (0) 

// Structure definitions
typedef struct line {
    int alcdSiz;    // Allocated size, will depend on the size of the screen
    int len;        // Occupied length
    char* buf;      // Actual character buffer
} line;

typedef struct lineArr {
    int alcdSiz;    // Total number of lines that can be in memory, can expand if necessary
    int lastIndex;  // Total number of lines in memory right now - 1 (essentially the last index in the array)
    line** arr;     // Actual array of line pointers, is of size alcdSiz
} lineArr;

typedef struct Clipboard {
    int lastIndex;  // Gives the number of lines stored in clipboard
    line** arr;     // Stores the pointers to the actual lines, need to dynamically allocate and reallocate space for lines
} clipboard;


// Global vars
bool running;                   // Exit flag
bool dirty;                     // Write-require check
int currentMode;                // State flag
char filename[FILENAME_MAX];    // is what its named, idk where FILENAME_MAX const comes from stdio.h file
// Dimension data for the different screens
int stdscrstarty, stdscrstartx, stdscrendy, stdscrendx, stdscrnlines, stdscrncols;
int imscrstarty, imscrstartx, imscrendy, imscrendx, imscrnlines, imscrncols;
int cmscrstarty, cmscrstartx, cmscrendy, cmscrendx, cmscrnlines, cmscrncols;
int imscrbgstarty, imscrbgstartx, imscrbgendy, imscrbgendx, imscrbgnlines, imscrbgncols;
int cmscrbgstarty, cmscrbgstartx, cmscrbgendy, cmscrbgendx, cmscrbgnlines, cmscrbgncols;
WINDOW* imscr;      // Actual data screen where we can edit text
WINDOW* imscrbg;    // Just to make drawing background easy
WINDOW* cmscr;      // Actual terminal screen to edit commands
WINDOW* cmscrbg;    // Just to make drawing background easy
int sx, sy, mx, my;   // x,y are screen coords, will be limited by dimensions,
// mx, my are memory indices, so will be limited by number of lines on screen
lineArr lines;      // Struct to contain text data in structured format
clipboard cb;       // Clipboard struct, to be used for copy-paste-cut-paste functionality


// General Functions
line* Line(void);           // Returns an empty line struct pointer thats completely set up as per current settings
int ProcessKeyhit(void);   // Get a keyhit and do corresponding task depending on mode
void Sleep(double seconds); // Sleep for a certain given amount of time
void Refresh(void); // Refresh both cmscr and imscr
void WriteToFile(void); // Save data
void LoadFromFile(void);    // Load text data from file <filname>
void Message(const char* msg); // Print a message to the console
void QuickMessage(const char* msg); // Print a short-lived message to the console
void ExitProgram(int errcode);  // Deallocate all memory and exit program safely
void PrintError(int errcode);   // If there is an error of known type print that out 
int RunNormalMode(void);        // Basic tasks

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
    BACKSLASH = 92,     // BACKSLASH
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
