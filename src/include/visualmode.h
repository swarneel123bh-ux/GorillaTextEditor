#ifndef VISUALMODE_H
#define VISUALMODE_H

extern int visualmodeactve;

int RunVisualMode(void);    // basic tasks
void ClearHighlight(int starty, int startx, int endy, int endx); // Clear the highlighting of a piece of text given by (starty, startx) to (endy, endx)
void ClearAllHighlight(void);   // Basically undo any and all highlighting
void Highlight(int starty, int startx, int endy, int endx); // Highlight a piece of text given by (starty, startx) to (endy, endx)
// Might wanna remove Select() entirely and implement sepearate cut-copy functions instead
void Copy(int starty, int startx, int endy, int endx);      // Select a piece of text and copy to clipboard
void Cut(int starty, int startx, int endy, int endx);       // Select a piece of text, copy to clipboard and delete from screen
void Paste(void);   // Paste whatever content in clipboard at current cursor location


#endif // VISUALMODE_H
