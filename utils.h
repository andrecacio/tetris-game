#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include "GLCD/GLCD.h" 

/* --- COLORI PEZZI STANDARD --- */
#define C_I  Cyan
#define C_O  Yellow
#define C_T  Magenta
#define C_J  Blue
#define C_L  Red      
#define C_S  Green
#define C_Z  Red
#define C_EMPTY Black

/* --- COLORI SPECIALI (DISTINTI) --- */
#define C_PWR_CLEAR  White   /* Bianco Brillante */
#define C_PWR_SLOW   0xC618  /* Grigio Argento (Silver) */
#define C_MALUS      0x8000  /* Rosso Scuro (Maroon) */

/* ID Logici */
#define TYPE_I 0
#define TYPE_O 1
#define TYPE_T 2
#define TYPE_J 3
#define TYPE_L 4
#define TYPE_S 5
#define TYPE_Z 6

#define STATE_PAUSE 0
#define STATE_PLAY  1
#define STATE_OVER  2

typedef struct {
  uint8_t type;
  uint8_t rot;
  int8_t  r;
  int8_t  c;
} Piece;

/* EXTERN */
extern volatile int joy_left_event, joy_right_event, joy_up_event, joy_click_event, joy_down_held;
extern volatile int key1_event, key2_event, int0_event;

extern volatile int boardgame[20][10]; 
extern volatile int game_state;
extern volatile int score;
extern volatile int lines;
extern volatile int high;
extern volatile Piece cur;
extern volatile Piece drawn_piece;
extern volatile int ui_dirty;
extern volatile int ui_full_redraw;

extern volatile Piece piece_to_draw_static;
extern volatile int flag_draw_static;

extern volatile int physics_frozen;
extern volatile int board_shifted;

/* Variabili Extra */
extern volatile uint32_t total_lines_cleared; 
extern volatile int powerup_slow_timer;

/* Funzioni */
const uint8_t (*shape_of(uint8_t type))[4][4];
uint16_t get_color_of(uint8_t type); 

int  can_place(uint8_t type, uint8_t rot, int new_r, int new_c);
void spawn_piece(void);
void lock_piece(void);
int  clear_lines(void);
void rng_seed(uint16_t s);
void reset_game(void);
void draw_piece_at(uint8_t type, uint8_t rot, int r, int c, int filled);
void render_all(void);
void UI_ShowLegend(void);

/* Hardware */
void ADC_Init(void);
uint16_t ADC_Read(void);
void DAC_Init(void);
void Play_Sound(uint32_t freq);

#endif