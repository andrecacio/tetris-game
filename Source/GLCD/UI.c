#include <stdint.h>
#include "GLCD/GLCD.h"

#define CELL        12
#define BOARD_X0    5
#define BOARD_Y0    5
#define BOARD_W     (10 * CELL)
#define BOARD_H     (20 * CELL)

#define HUD_X0      (BOARD_X0 + BOARD_W + 10)
#define HUD_Y0      10

static void LCD_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
	int i;
  for (i = 0; i < h; i++) {
    LCD_DrawLine(x, y + i, x + w - 1, y + i, color);
  }
}

static void DrawBoardFrame(void) {
  LCD_DrawLine(BOARD_X0-1, BOARD_Y0-1, BOARD_X0+BOARD_W, BOARD_Y0-1, White);
  LCD_DrawLine(BOARD_X0-1, BOARD_Y0-1, BOARD_X0-1, BOARD_Y0+BOARD_H, White);
  LCD_DrawLine(BOARD_X0+BOARD_W, BOARD_Y0-1, BOARD_X0+BOARD_W, BOARD_Y0+BOARD_H, White);
  LCD_DrawLine(BOARD_X0-1, BOARD_Y0+BOARD_H, BOARD_X0+BOARD_W, BOARD_Y0+BOARD_H, White);
}

static void DrawCell(int row, int col, uint16_t color) {
  uint16_t x = BOARD_X0 + col * CELL;
  uint16_t y = BOARD_Y0 + row * CELL;

  LCD_FillRect(x+1, y+1, CELL-2, CELL-2, color);
}