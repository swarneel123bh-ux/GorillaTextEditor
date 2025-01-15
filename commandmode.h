#ifndef COMMANDMODE_H
#define COMMANDMODE_H

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


void ProcessCommand();  // Get a command and do related task
void ExitProgram(int errcode);  // Deallocate all memory and exit program safely
void PrintError(int errcode);   // If there is an error of known type print that out 
void Message(const char* msg);  // Print msg to command window

#endif // COMMANDMODE_H