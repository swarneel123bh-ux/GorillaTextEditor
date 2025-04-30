#include "commandmode.h"
#include "general.h"

// Get a command and do related task
void ProcessCommand(void) {
    wmove(cmscr, 0, 0);
    wclrtoeol(cmscr);
    Refresh();

    char command[100];
    echo();
    mvwscanw(cmscr, 0, 0, "%s", command);
    noecho();

    if (!strcmp(command, "q")) {
        if (dirty) {
            QuickMessage("--Unwritten changes exist, use :w first--"); }
        else { ExitProgram(ERR_NOERROR); }
    }
    if (!strcmp(command, "q!")) ExitProgram(ERR_NOERROR); 
    if (!strcmp(command, "w")) { WriteToFile(); }
    if (!strcmp(command, "wq")) { WriteToFile(); ExitProgram(ERR_NOERROR); }
    else { 
        QuickMessage("--COMMANDMODE--> Invalid command");
        currentMode = NORMALMODE;
        return;
    }
}

// Do whatever is to be done, return a value that acts as a signal
int RunCommandMode(void) {
    char command[100];
    echo();
    mvwscanw(cmscr, 1, 0, "%s", command);
    noecho();

    if (!strcmp(command, "q")) {
        if (dirty) { Message("Unwritten changes exit, save first to exit"); } else { return SIGNAL_EXIT_PROGRAM; }
    }
    if (!strcmp(command, "q!")){
        return SIGNAL_EXIT_PROGRAM;
    }
    if (!strcmp(command, "w")) { 
        WriteToFile();
        return SIGNAL_SWITCH_TO_NORMALMODE;
    }
    if (!strcmp(command, "wq")) { 
        WriteToFile(); 
        return SIGNAL_EXIT_PROGRAM;
    }
    else { 
        QuickMessage("--COMMANDMODE--> Invalid command");
        currentMode = NORMALMODE;
        return SIGNAL_SWITCH_TO_NORMALMODE;
    }

    return SIGNAL_SWITCH_TO_NORMALMODE;
}
