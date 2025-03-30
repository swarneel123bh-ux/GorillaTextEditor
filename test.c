#include <ncurses.h>
#include <string.h>
#include <stdbool.h>

#define NORMAL_TEXT 1
#define HIGHLIGHTED_TEXT 2

#define min(a, b) ((a < b) ? a : b)
#define max(a, b) ((a > b) ? a : b)

int main() {
    // Initialize ncurses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE); // Enable special keys
    curs_set(1);          // Show the cursor
    start_color();        // Enable color functionality

    // Define color pairs
    init_pair(HIGHLIGHTED_TEXT, COLOR_BLACK, COLOR_YELLOW); // Highlighted text
    init_pair(NORMAL_TEXT, COLOR_WHITE, COLOR_BLACK);  // Normal text

    // Sample text
    const char *text = "This is an example text editor with highlight functionality.";
    int text_len = strlen(text);

    // Highlight states for each character (0 = normal, 1 = highlighted)
    int highlight[text_len];
    memset(highlight, 0, sizeof(highlight));

    // Cursor position
    int cursor_x = 0;

    // Display all the text first with NORMAL_TEXT colors
    attron(COLOR_PAIR(NORMAL_TEXT));    
    for (int i = 0; i < text_len; i++) {
        mvaddch(0, i, text[i]); // Print character at position (row 0, column i)
    }
    attroff(COLOR_PAIR(NORMAL_TEXT)); // Turn off normal color

    // Move the cursor to the current position
    move(0, cursor_x);
    refresh();

    // Main loop
    bool highlighting = false;
    int startx, endx;
    while (true) {
        int ch = getch(); // Get user input

        if (ch == 'q') { // Quit the program
            break;
        } else if (ch == KEY_RIGHT) {
            if (cursor_x < text_len - 1) {
                cursor_x ++;
                move(0, cursor_x);
            }
        } else if (ch == KEY_LEFT) {
            if (cursor_x > 0) {
                cursor_x--;
                move(0, cursor_x);
            }
        } else if (ch == ' ') { // Toggle highlighting with SPACE key
            if (!highlighting) {    // Start selecting
                highlighting = true;
                curs_set(0);
                startx = cursor_x;
            } else {
                highlighting = false;

                for (int i = 0; i < text_len - 1; i ++) {
                    move(0, i);
                    chgat(1, A_NORMAL, NORMAL_TEXT, NULL);
                }
                move(0, cursor_x);
                curs_set(1);
            }
        }

        endx = cursor_x;


        if (highlighting) {
            for (int i = 0; i < text_len - 1; i ++) {
                move(0, i);
                chgat(1, A_NORMAL, NORMAL_TEXT, NULL);
            }

            for (int i = min(startx, endx); i <= max(startx, endx); i ++){
                move(0, i);
                chgat(1, A_NORMAL, HIGHLIGHTED_TEXT, NULL);
            }
        }

        move(0, cursor_x);
        refresh();
    }

    // End ncurses mode
    endwin();
    return 0;
}