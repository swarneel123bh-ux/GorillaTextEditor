#include "commandmode.h"
#include "general.h"

// Do whatever is to be done, return a value that acts as a signal
int RunCommandMode(void) {
    char command[100];
    memset(command, 0, sizeof(char) * 100);
    echo();
    wmove(cmscr, 1, 0);
    wclrtoeol(cmscr);
    Refresh();
    mvwscanw(cmscr, 1, 0, "%s", command);
    noecho();

    if (!strcmp(command, "q")) {
        if (dirty) { 
            QuickMessage("Unwritten changes exit, save first to exit"); 
            return SIGNAL_SWITCH_TO_NORMALMODE;
        } 
        else { return SIGNAL_EXIT_PROGRAM; }
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

    QuickMessage("COMMANDMODE :: Invalid command");
    currentMode = NORMALMODE;
    return SIGNAL_SWITCH_TO_NORMALMODE;

}
