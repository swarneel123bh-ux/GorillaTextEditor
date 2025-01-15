#ifndef COMMANDMODE_H
#define COMMANDMODE_H

void ProcessCommand();  // Get a command and do related task
void ExitProgram(int errcode);  // Deallocate all memory and exit program safely
void PrintError(int errcode);   // If there is an error of known type print that out 
void Message(const char* msg);  // Print msg to command window

#endif // COMMANDMODE_H