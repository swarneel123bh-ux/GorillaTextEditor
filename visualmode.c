#include "visualmode.h"
#include "mem.h"
#include "inputmode.h"

// NOTE :: NEED TO IMPLEMENT VISUALMODE BEFORE THESE CAN BE USED
/*
Copy the contents that fall between the starting coordinates and ending coordinates,
into the clipboard buffer
*/
void Copy() {}

/*
Copy the contents that fall between the starting and ending coords into the
clipboard buffer and also remove them from the screen properly
*/
void Cut() {}

/*
Paste the contents of a buffer at current cusor position and move cursor to the end of the pasted stuff
*/
void Paste(char* buf) {
    int ch;
    int i = 0;
    while ((ch = buf[i++])) InsertInLine(ch);
}