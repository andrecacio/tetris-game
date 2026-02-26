#include "utils.h"
#include "GLCD/UI.h"
#include "game_config.h" 
#include "LPC17xx.h"

/* --- VARIABILI --- */
volatile int joy_left_event = 0, joy_right_event = 0, joy_up_event = 0;
volatile int joy_click_event = 0, joy_down_held = 0;
volatile int key1_event = 0, key2_event = 0, int0_event = 0;

volatile int boardgame[20][10] = {0};
volatile int game_state = STATE_PAUSE; 
volatile Piece cur = {99,0,0,0}; 
volatile Piece drawn_piece;
volatile int score = 0, lines = 0, high = 0;
volatile int ui_dirty = 0, ui_full_redraw = 0;
volatile Piece piece_to_draw_static;
volatile int flag_draw_static = 0;
volatile int physics_frozen = 0, board_shifted = 0;

volatile uint32_t total_lines_cleared = 0;
volatile int powerup_slow_timer = 0;

volatile int music_enabled = 1;
volatile int current_note_index = 0;
volatile int note_duration_counter = 0;

static uint16_t lfsr = 0xACE1u;

/* Forme Standard */
static const uint8_t ishaped[4][4][4] = {{{0,0,0,0},{1,1,1,1},{0,0,0,0},{0,0,0,0}},{{0,1,0,0},{0,1,0,0},{0,1,0,0},{0,1,0,0}},{{0,0,0,0},{1,1,1,1},{0,0,0,0},{0,0,0,0}},{{0,1,0,0},{0,1,0,0},{0,1,0,0},{0,1,0,0}}};
static const uint8_t oshaped[4][4][4] = {{{0,1,1,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}},{{0,1,1,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}},{{0,1,1,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}},{{0,1,1,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}}};
static const uint8_t tshaped[4][4][4] = {{{0,1,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}},{{0,1,0,0},{0,1,1,0},{0,1,0,0},{0,0,0,0}},{{0,0,0,0},{1,1,1,0},{0,1,0,0},{0,0,0,0}},{{0,1,0,0},{1,1,0,0},{0,1,0,0},{0,0,0,0}}};
static const uint8_t jshaped[4][4][4] = {{{1,0,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}},{{0,1,1,0},{0,1,0,0},{0,1,0,0},{0,0,0,0}},{{0,0,0,0},{1,1,1,0},{0,0,1,0},{0,0,0,0}},{{0,1,0,0},{0,1,0,0},{1,1,0,0},{0,0,0,0}}};
static const uint8_t lshaped[4][4][4] = {{{0,0,1,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}},{{0,1,0,0},{0,1,0,0},{0,1,1,0},{0,0,0,0}},{{0,0,0,0},{1,1,1,0},{1,0,0,0},{0,0,0,0}},{{1,1,0,0},{0,1,0,0},{0,1,0,0},{0,0,0,0}}};
static const uint8_t sshaped[4][4][4] = {{{0,1,1,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}},{{0,1,0,0},{0,1,1,0},{0,0,1,0},{0,0,0,0}},{{0,1,1,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}},{{0,1,0,0},{0,1,1,0},{0,0,1,0},{0,0,0,0}}};
static const uint8_t zshaped[4][4][4] = {{{1,1,0,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}},{{0,0,1,0},{0,1,1,0},{0,1,0,0},{0,0,0,0}},{{1,1,0,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}},{{0,0,1,0},{0,1,1,0},{0,1,0,0},{0,0,0,0}}};

void rng_seed(uint16_t s){ if (s!=0) lfsr = s; }
static uint16_t rng16(void) { lfsr = (uint16_t)((lfsr >> 1) ^ (uint16_t)(-(int)(lfsr & 1u) & 0xB400u)); return lfsr; }
static uint8_t rand7(void) { return (uint8_t)(rng16() % 7u); }
static uint8_t randRange(int min, int max) { return (uint8_t)((rng16() % (max - min + 1)) + min); }

const uint8_t (*shape_of(uint8_t type))[4][4] {
  switch(type) {
    case TYPE_I: return ishaped; case TYPE_O: return oshaped; case TYPE_T: return tshaped;
    case TYPE_J: return jshaped; case TYPE_L: return lshaped; case TYPE_S: return sshaped;
    default: return zshaped;
  }
}

uint16_t get_color_of(uint8_t type) {
  if (type == 7) return C_MALUS; 
  switch(type) {
    case TYPE_I: return C_I; case TYPE_O: return C_O; case TYPE_T: return C_T;
    case TYPE_J: return C_J; case TYPE_L: return C_L; case TYPE_S: return C_S; case TYPE_Z: return C_Z;
    case PWR_CLEAR_HALF: return C_PWR_CLEAR; 
    case PWR_SLOW:       return C_PWR_SLOW;
    default:     return White;
  }
}

/* --- UI: LEGENDA (ZONA Y: 160 -> 220) --- */
void UI_ShowLegend(void) {
    /* Uso righe singole per risparmiare spazio e renderlo leggibile */
    GUI_Text(135, 160, (uint8_t *)"-- LEGEND --", White, Black);

    GUI_Text(135, 180, (uint8_t *)"WHT: 1/2 CLR", C_PWR_CLEAR, Black);
    
    GUI_Text(135, 200, (uint8_t *)"GRY: SLOW MO", C_PWR_SLOW,  Black);
    
    GUI_Text(135, 220, (uint8_t *)"RED: MALUS",   C_MALUS,     Black);
}

void ADC_Init(void) {
    LPC_SC->PCONP |= (1 << 12);
    LPC_PINCON->PINSEL3 |= (3UL << 30); 
    LPC_ADC->ADCR = (1 << 5) | (4 << 8) | (1 << 21);  
}

uint16_t ADC_Read(void) {
    volatile int timeout = 5000;
    LPC_ADC->ADCR |= (1 << 24); 
    while (!(LPC_ADC->ADGDR & (1UL << 31)) && timeout > 0) timeout--;
    return (LPC_ADC->ADGDR >> 4) & 0xFFF;
}

void DAC_Init(void) {
    LPC_PINCON->PINSEL1 |= (1 << 21); 
    LPC_SC->PCONP       |= (1 << 29); 
}

void DAC_SetOutput(uint16_t value) {
    LPC_DAC->DACR = ((value & 0x3FF) << 6);
}

void Play_Sound(uint32_t freq) {
    LPC_DAC->DACR = (0x3FF << 6); 
}

void Stop_Sound(void) { DAC_SetOutput(0); }
void Update_Background_Music(void) {}

/* --- LOGICA POWERUP --- */
/* Le scritte dinamiche ora vanno da Y=250 in giù */
static void spawn_powerup(void) {
    int r, c;
    static struct { int r, c; } candidates[200];
    int count = 0;

    for(r=0; r<20; r++) {
        for(c=0; c<10; c++) {
            if(boardgame[r][c] > 0 && boardgame[r][c] <= 7) {
                candidates[count].r = r;
                candidates[count].c = c;
                count++;
            }
        }
    }

    if(count > 0) {
        int idx = rng16() % count;
        r = candidates[idx].r;
        c = candidates[idx].c;
        
        int ptype = (rng16() % 2) == 0 ? PWR_CLEAR_HALF : PWR_SLOW;
        boardgame[r][c] = ptype + 1;
        
        board_shifted = 1; 
        ui_dirty = 1;
        
        Play_Sound(NOTE_G4);
        /* ZONA Y=250 (Sicura) */
        GUI_Text(135, 250, (uint8_t *)"PWR SPAWN!", Green, Black);
    }
}

static void apply_malus(void) {
    int r, c, holes = 0;
    for(c=0; c<10; c++) if(boardgame[0][c] != 0) { game_state = STATE_OVER; ui_dirty = 1; return; }
    for(r=0; r<19; r++) for(c=0; c<10; c++) boardgame[r][c] = boardgame[r+1][c];
    for(c=0; c<10; c++) boardgame[19][c] = 8; 
    while(holes < 3) {
        c = randRange(0, 9);
        if(boardgame[19][c] != 0) { boardgame[19][c] = 0; holes++; }
    }
    board_shifted = 1; ui_dirty = 1;
    Play_Sound(NOTE_C4);
    /* ZONA Y=270 (Sicura) */
    GUI_Text(135, 270, (uint8_t *)"MALUS UP! ", Red, Black);
}

static void apply_clear_half(void) {
    int r, c;
    /* SPECIFICA: Calcolo punti gruppi da 4 */
    int lines_cleared = 10; 
    int groups_of_4 = lines_cleared / 4; 
    int remainder = lines_cleared % 4;

    for(r=19; r>=10; r--) for(c=0; c<10; c++) boardgame[r][c] = 0;
    for(r=19; r>=10; r--) {
        for(c=0; c<10; c++) {
            boardgame[r][c] = boardgame[r-10][c];
            boardgame[r-10][c] = 0;
        }
    }
    score += (groups_of_4 * 600) + (remainder * 100);
    
    board_shifted = 1; ui_dirty = 1;
    /* ZONA Y=290 (Sicura) */
    GUI_Text(135, 290, (uint8_t *)"HALF CLEAR", C_PWR_CLEAR, Black);
}

void reset_game(void) {
  int r,c; game_state = STATE_PAUSE; score = 0; lines = 0; total_lines_cleared = 0; powerup_slow_timer = 0;
  for(r=0; r<20; r++) for(c=0; c<10; c++) boardgame[r][c] = 0;
  cur.type = 99; cur.rot = 0; cur.r = 0; cur.c = 0;
  flag_draw_static = 0; ui_full_redraw = 0; board_shifted = 1; ui_dirty = 1; physics_frozen = 0;
  
  /* IMPORTANTE: Pulisci SOLO la zona messaggi (Y > 240) */
  /* NON toccare la zona Legenda (Y < 240) */
  GUI_Text(135, 250, (uint8_t *)"          ", Black, Black);
  GUI_Text(135, 270, (uint8_t *)"          ", Black, Black);
  GUI_Text(135, 290, (uint8_t *)"          ", Black, Black);
}

int can_place(uint8_t type, uint8_t rot, int new_r, int new_c) {
  const uint8_t (*S)[4][4]; int pr, pc, br, bc; if (type > 6) return 0; S = shape_of(type);
  for(pr=0; pr<4; pr++) for(pc=0; pc<4; pc++) {
    if(!S[rot][pr][pc]) continue; br = new_r + pr; bc = new_c + pc;
    if(bc < 0 || bc >= 10 || br >= 20) return 0;
    if(br >= 0 && boardgame[br][bc] != 0) return 0;
  }
  return 1;
}

void spawn_piece(void) {
  cur.type = rand7(); cur.rot = 0; cur.r = 0; cur.c = 3;
  if(!can_place(cur.type, cur.rot, cur.r, cur.c)) { if(score > high) high = score; game_state = STATE_OVER; ui_full_redraw = 0; ui_dirty = 1; }
}

void lock_piece(void) {
  const uint8_t (*S)[4][4] = shape_of(cur.type); int pr, pc, br, bc; if (cur.type > 6) return;
  for(pr=0; pr<4; pr++) for(pc=0; pc<4; pc++) {
    if (S[cur.rot][pr][pc]) {
      br = cur.r + pr; bc = cur.c + pc;
      if(br>=0 && br<20 && bc>=0 && bc<10) boardgame[br][bc] = cur.type + 1;
    }
  }
  score += 10;
  Play_Sound(NOTE_D4);
}

int clear_lines(void) {
  int cleared = 0, r, c, rr, full, pwr_half = 0, pwr_slow = 0;
  uint32_t old_total;

  for(r=19; r>=0; r--) {
    full = 1; for(c=0; c<10; c++) if(boardgame[r][c] == 0) { full = 0; break; }
    if(full) {
      /* CHECK POWERUPS */
      for(c=0; c<10; c++) {
          if(boardgame[r][c] == (PWR_CLEAR_HALF + 1)) pwr_half = 1;
          if(boardgame[r][c] == (PWR_SLOW + 1))       pwr_slow = 1;
      }
      cleared++;
      for(rr=r; rr>0; rr--) for(c=0; c<10; c++) boardgame[rr][c] = boardgame[rr-1][c];
      for(c=0; c<10; c++) boardgame[0][c] = 0;
      r++;
    }
  }

  if(pwr_slow) { 
      powerup_slow_timer = 300; /* 15 Secondi */
      Play_Sound(NOTE_E4); 
      /* ZONA Y=290 (Sicura) */
      GUI_Text(135, 290, (uint8_t *)"SLOW MO!  ", C_PWR_SLOW, Black);
  }
  if(pwr_half) { apply_clear_half(); Play_Sound(NOTE_G4); return 1; }

  if (cleared > 0) {
    lines += cleared;
    old_total = total_lines_cleared;
    total_lines_cleared += cleared;
    
    /* MODALITÀ TEST: 1 e 2 per Powerup/Malus rapido */
    if( (total_lines_cleared / 1) > (old_total / 1) ) spawn_powerup();
    if( (total_lines_cleared / 2) > (old_total / 2) ) apply_malus();

    if (cleared == 4) score += 600; else score += 100 * cleared;
    Play_Sound(NOTE_C4);
  }
  return cleared;
}

void draw_piece_at(uint8_t type, uint8_t rot, int r, int c, int filled) {
  const uint8_t (*S)[4][4]; uint16_t color; int pr, pc, br, bc; if (type > 6) return; S = shape_of(type); color = filled ? get_color_of(type) : C_EMPTY;
  for(pr=0; pr<4; pr++) for(pc=0; pc<4; pc++) { if(S[rot][pr][pc]) { br = r + pr; bc = c + pc; if(br>=0 && br<20 && bc>=0 && bc<10) UI_DrawCell(br, bc, color); } }
}
void render_all(void) {
  int r, c, content; UI_ClearBoard();
  for(r=0; r<20; r++) { for(c=0; c<10; c++) { content = boardgame[r][c]; if (content > 0) UI_DrawCell(r, c, get_color_of((uint8_t)(content - 1))); } }
  if(game_state != STATE_OVER && cur.type <= 6) draw_piece_at(cur.type, cur.rot, cur.r, cur.c, 1);
}