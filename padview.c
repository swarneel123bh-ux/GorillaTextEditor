#include <ncurses.h>

int main() {
    initscr();            // Initialize ncurses
    keypad(stdscr, TRUE); // Enable function keys and arrow keys
    noecho();             // Don't echo input characters
    cbreak();             // Disable line buffering
    curs_set(0);          // Hide the cursor

    // Create a pad (50x100 size)
    int pad_rows = 50, pad_cols = 100;
    WINDOW *pad = newpad(pad_rows, pad_cols);
	refresh();

    // Fill the pad with some content
    for (int i = 0; i < pad_rows; i++) {
        for (int j = 0; j < pad_cols; j++) {
            mvwprintw(pad, i, j, "%c", (i + j) % 26 + 'A');
        }
    }

    // Variables for the portion of the pad to display
    int pad_start_row = 0, pad_start_col = 0;
    int screen_rows = 20, screen_cols = 50; // Visible area on the screen

    // Display initial portion of the pad
    prefresh(pad, pad_start_row, pad_start_col, 0, 0, screen_rows - 1, screen_cols - 1);

    int ch;
    while ((ch = getch()) != 'q') { // Exit on 'q'
        switch (ch) {
            case KEY_UP:
                if (pad_start_row > 0) pad_start_row--;
                break;
            case KEY_DOWN:
                if (pad_start_row < pad_rows - screen_rows) pad_start_row++;
                break;
            case KEY_LEFT:
                if (pad_start_col > 0) pad_start_col--;
                break;
            case KEY_RIGHT:
                if (pad_start_col < pad_cols - screen_cols) pad_start_col++;
                break;
        }
        prefresh(pad, pad_start_row, pad_start_col, 0, 0, screen_rows - 1, screen_cols - 1);
    }

    delwin(pad); // Clean up
    endwin();    // Restore terminal
    return 0;
}