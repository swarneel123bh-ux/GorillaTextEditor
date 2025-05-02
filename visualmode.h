#ifndef VISUALMODE_H
#define VISUALMODE_H

int RunVisualMode(void);    // basic tasks
void ClearHighlight(int starty, int startx, int endy, int endx); // Clear the highlighting of a piece of text given by (starty, startx) to (endy, endx)
void Highlight(int starty, int startx, int endy, int endx); // Highlight a piece of text given by (starty, startx) to (endy, endx)
// Might wanna remove Select() entirely and implement sepearate cut-copy functions instead
void Copy(int starty, int startx, int endy, int endx);      // Select a piece of text and copy to clipboard
void Cut(int starty, int startx, int endy, int endx);       // Select a piece of text, copy to clipboard and delete from screen


#endif // VISUALMODE_H
