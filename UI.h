#ifndef UI_H
#define UI_H

#include <stdint.h>

void UI_DrawStatic(void);
void UI_ClearBoard(void);
void UI_DrawCell(int r, int c, int filled);


void UI_ClearRow(int r);

void UI_UpdateScore(int value);
void UI_UpdateHigh(int value);
void UI_UpdateLines(int value);
void UI_UpdateState(int state);

#endif
