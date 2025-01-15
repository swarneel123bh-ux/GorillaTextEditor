#include "commandmode.h"
#include "mem.h"
#include "inputmode.h"
#include "general.h"

// Print msg out to command window
void Message(const char* msg) {
    wmove(cmWindow->window, 1, 1);
    wclrtoeol(cmWindow->window);
    mvwprintw(cmWindow->window, 1, 1, msg);
    REFRESH();
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