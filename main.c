#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ncurses.h>
#include "commandmode.h"
#include "inputmode.h"
#include "visualmode.h"
#include "general.h"

int main(int argc, char** argv) {
    // Initial setup
    initscr();
    noecho();
    cbreak();
    start_color();
    getmaxyx(stdscr, stdscrendy, stdscrendx); stdscrendy --; stdscrendx --; // Reduction by 1 is necessary, idk why
    stdscrstarty = stdscrstartx = 0;
    stdscrnlines = stdscrendy - stdscrstarty + 1;
    stdscrncols = stdscrendx - stdscrstartx + 1;
    imscrbgstarty = imscrbgstartx = 0;
    imscrbgendy = stdscrendy - 4;
    imscrbgendx = stdscrendx;
    imscrbgnlines = imscrbgendy - imscrbgstarty + 1;
    imscrbgncols = imscrbgendx - imscrbgstartx + 1;
    imscrstarty = imscrbgstarty + 1;
    imscrstartx = imscrbgstartx + 1;
    imscrendy = imscrbgendy - 1;
    imscrendx = imscrbgendx - 1;
    imscrnlines = imscrendy - imscrstarty + 1;
    imscrncols = imscrendx - imscrstartx + 1;
    cmscrbgstarty = imscrbgendy + 1;
    cmscrbgendy = stdscrendy;
    cmscrbgstartx = 0;
    cmscrbgendx = stdscrendx;
    cmscrbgnlines = cmscrbgendy - cmscrbgstarty + 1;
    cmscrbgncols = cmscrbgendx - cmscrbgstartx + 1;
    cmscrstarty = cmscrbgstarty + 1;
    cmscrendy = cmscrbgendy - 1;
    cmscrstartx = cmscrbgstartx + 1;
    cmscrendx = cmscrbgendx - 1;
    cmscrnlines = cmscrendy - cmscrstarty + 1;
    cmscrncols = cmscrendx - cmscrstartx + 1;
    imscrbg = newwin(imscrbgnlines, imscrbgncols, imscrbgstarty, imscrbgstartx);
    imscr = newwin(imscrnlines, imscrncols, imscrstarty, imscrstartx);
    cmscrbg = newwin(cmscrbgnlines, cmscrbgncols, cmscrbgstarty, cmscrbgstartx);
    cmscr = newwin(cmscrnlines, cmscrncols, cmscrstarty, cmscrstartx);
    box(imscrbg, 0, 0);
    box(cmscrbg, 0, 0);
    Refresh();
    wrefresh(imscrbg);
    wrefresh(imscr);
    wrefresh(cmscrbg);
    wrefresh(cmscr);
    init_pair(HIGHLIGHTED_TEXT, COLOR_BLACK, COLOR_YELLOW); // Highlighted text
    init_pair(NORMAL_TEXT, COLOR_WHITE, COLOR_BLACK);  // Normal text 
    scrollok(imscr, TRUE);   // Allow scrolling
    // Setting ESCDELAY to 50 milliseconds to avoid delay when shifting modes
    ESCDELAY = 50;
    // Set keypad to true to allow cursor movement when in normalmode
    keypad(stdscr, true); 
    keypad(imscr, true); 
    keypad(cmscr, true);

    // memory setup
    // Initializeing the lines array
    // Loading 1000 empty line pointers to memory and keeping it constant should not affect performance i think
    lines.arr = (line**) malloc (sizeof(line*) * MAXBUFLEN);    
    lines.lastIndex = 0;
    lines.alcdSiz = MAXBUFLEN;  // This size is in units not bytes
    for (int i = 0; i < MAXBUFLEN; i ++) {
        lines.arr[i] = Line();
    }

    // Load from file
    memset(filename, 0, sizeof(char) * FILENAME_MAX);
    strcpy(filename, (argc > 1)? argv[1] : "Untitled");
    mvwprintw(imscrbg, 0, (int)((imscrbgncols - strlen(filename)) / 20), "%s", filename);
    LoadFromFile();
    PrintLoadedFile();  // Print the contents loaded from file
    Refresh();

    // Main loop
    running = true;
    currentMode = NORMALMODE;
    Message("NORMALMODE");
    int retval = -999;
    while (running) {
        // Process current mode operation
        switch (currentMode) {
            case NORMALMODE:    { retval = RunNormalMode(); break; }
            case VISUALMODE:    { retval = RunVisualMode(); break; }
            case INPUTMODE:     { retval = RunInputMode(); break; }
            case COMMANDMODE:   { retval = RunCommandMode(); break; }
            default:            { ExitProgram(ERR_UNDEFINED_MODE); }
        }

        // Process the return value from each mode
        switch (retval) {
            case SIGNAL_EXIT_PROGRAM: {
                running = false;
                ExitProgram(ERR_NOERROR);
                break;
            }
            case SIGNAL_SWITCH_TO_NORMALMODE: {
                currentMode = NORMALMODE;
                Message("NORMALMODE");
                break;
            }
            case SIGNAL_SWITCH_TO_INPUTMODE: {
                currentMode = INPUTMODE;
                wmove(imscr, sy, sx);
                Message("INPUTMODE");
                break;
            }
            case SIGNAL_SWITCH_TO_VISUALMODE: {
                currentMode = VISUALMODE;
                wmove(imscr, sy, sx);
                Message("VISUALMODE");
                break;
            }
            case SIGNAL_SWITCH_TO_COMMANDMODE: {
                Message("COMMANDMODE");
                currentMode = COMMANDMODE;
                break;
            }
            default: { break; }
        }
    }

    endwin();

    return 0;
}


