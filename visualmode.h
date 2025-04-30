#ifndef VISUALMODE_H
#define VISUALMODE_H

int RunVisualMode(void);    // basic tasks
void ClearHighlight(int* starty, int* startx, int* endy, int* endx); // Clear the highlighting of a piece of text given by (starty, startx) to (endy, endx)
void Highlight(int* starty, int* startx, int* endy, int* endx); // Highlight a piece of text given by (starty, startx) to (endy, endx)
void Select(int* starty, int* startx, int* endy, int* endx);    // Select a piece of text given by (starty, startx) to (endy, endx)

#endif // VISUALMODE_H
