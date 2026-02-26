#include <stdint.h>
#include <stdio.h>
#include "GLCD.h"
#include "UI.h"
#include "../utils.h" 

#define CELL        12 
#define BOARD_X0    5 
#define BOARD_Y0    5
#define BOARD_W     (10 * CELL) 
#define BOARD_H     (20 * CELL) 
#define HUD_X0      (BOARD_X0 + BOARD_W + 10)
#define HUD_Y0      10

void LCD_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
  int i;
  for (i = 0; i < h; i++) {
      LCD_DrawLine(x, y + i, x + w - 1, y + i, color);
  }
}

void DrawBoardFrame(void) {
  LCD_DrawLine(BOARD_X0-1, BOARD_Y0-1, BOARD_X0+BOARD_W, BOARD_Y0-1, White);
  LCD_DrawLine(BOARD_X0-1, BOARD_Y0-1, BOARD_X0-1, BOARD_Y0+BOARD_H, White);
  LCD_DrawLine(BOARD_X0+BOARD_W, BOARD_Y0-1, BOARD_X0+BOARD_W, BOARD_Y0+BOARD_H, White);
  LCD_DrawLine(BOARD_X0-1, BOARD_Y0+BOARD_H, BOARD_X0+BOARD_W, BOARD_Y0+BOARD_H, White);
}

void UI_ClearBoard(void) {
    LCD_FillRect(BOARD_X0, BOARD_Y0, BOARD_W, BOARD_H, Black);
    DrawBoardFrame(); 
}


void UI_ClearRow(int r) {
    uint16_t x = BOARD_X0;
    uint16_t y = (uint16_t)(BOARD_Y0 + r * CELL);
    if (r >= 0 && r < 20) {
        LCD_FillRect(x, y, BOARD_W, CELL, Black);
    }
}

void UI_DrawCell(int r, int c, int color_val) {
  uint16_t x = BOARD_X0 + c * CELL;
  uint16_t y = BOARD_Y0 + r * CELL;
  if(r >= 0 && r < 20 && c >= 0 && c < 10) {
      LCD_FillRect(x+1, y+1, CELL-2, CELL-2, (uint16_t)color_val);
  }
}

void UI_DrawStatic(void) {
  DrawBoardFrame();
  GUI_Text(HUD_X0, HUD_Y0 +  0, (uint8_t*)"SCORE:", White, Black);
  GUI_Text(HUD_X0, HUD_Y0 + 40, (uint8_t*)"HIGH:",  White, Black);
  GUI_Text(HUD_X0, HUD_Y0 + 80, (uint8_t*)"LINES:", White, Black);
  GUI_Text(HUD_X0, HUD_Y0 + 120,(uint8_t*)"STATE:", White, Black);
	
}




void UI_UpdateScore(int value) {
    char buf[20];
    sprintf(buf, "%d    ", value); 
    GUI_Text(HUD_X0, HUD_Y0 + 16, (uint8_t*)buf, Yellow, Black);
}

void UI_UpdateHigh(int value) {
    char buf[20];
    sprintf(buf, "%d    ", value);
    GUI_Text(HUD_X0, HUD_Y0 + 56, (uint8_t*)buf, Green, Black);
}

void UI_UpdateLines(int value) {
    char buf[20];
    sprintf(buf, "%d    ", value);
    GUI_Text(HUD_X0, HUD_Y0 + 96, (uint8_t*)buf, Cyan, Black);
}

void UI_UpdateState(int state) {
    if(state == 1)      GUI_Text(HUD_X0, HUD_Y0 + 136, (uint8_t*)"PLAYING  ", Green, Black);
    else if(state == 2) GUI_Text(HUD_X0, HUD_Y0 + 136, (uint8_t*)"GAME OVER", Red, Black);
    else                GUI_Text(HUD_X0, HUD_Y0 + 136, (uint8_t*)"PAUSED   ", Red, Black);
}
