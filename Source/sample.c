#include "LPC17xx.h"
#include "GLCD/GLCD.h" 
#include "timer/timer.h"
#include "RIT/RIT.h"        
#include "joystick/joystick.h"
#include "button_EXINT/button.h"
#include "GLCD/UI.h"
#include "../utils.h"
#include "../game_config.h"
#include <stdint.h>

/* NESSUNA VARIABILE QUI (Sono tutte in utils.c) */

void erase_piece_restore(const Piece *p) {
    const uint8_t (*S)[4][4];
    int pr, pc, br, bc;
    if(p->type > 6) return;
    S = shape_of(p->type);
    for(pr=0; pr<4; pr++) {
        for(pc=0; pc<4; pc++) {
            if(S[p->rot][pr][pc]) {
                br = p->r + pr; bc = p->c + pc;
                if(br>=0 && br<20 && bc>=0 && bc<10) {
                    int cell = boardgame[br][bc];
                    if (cell > 0) UI_DrawCell(br, bc, get_color_of((uint8_t)(cell - 1)));
                    else UI_DrawCell(br, bc, C_EMPTY);
                }
            }
        }
    }
}

static void redraw_row_from_board(int r) {
    int c;
    UI_ClearRow(r);
    for (c=0; c<10; c++) {
        int cell = boardgame[r][c];
        if (cell > 0) UI_DrawCell(r, c, get_color_of((uint8_t)(cell - 1)));
    }
}

static void shadow_apply_locked_piece(uint8_t shadow[20][10], const Piece *p) {
    const uint8_t (*S)[4][4];
    int pr, pc, br, bc;
    if (p->type > 6) return;
    S = shape_of(p->type);
    for(pr=0; pr<4; pr++) for(pc=0; pc<4; pc++) {
        if(S[p->rot][pr][pc]) {
            br = p->r + pr; bc = p->c + pc;
            if(br>=0 && br<20 && bc>=0 && bc<10) shadow[br][bc] = (uint8_t)(p->type + 1);
        }
    }
}

int main(void)
{
    int full;
    Piece oldp, newp;
    int s, h, l, st;
    int last_score = -1, last_high = -1, last_lines = -1, last_state = -1;
    static uint8_t shadow_board[20][10] = {{0}};

    SystemInit();
    LCD_Initialization();
    LCD_Clear(Black);
    
    ADC_Init(); 
    DAC_Init();
    
    UI_DrawStatic(); 
		UI_ShowLegend();
    UI_UpdateScore(0);
    UI_UpdateHigh(high);
    UI_UpdateLines(0);
    UI_UpdateState(STATE_PAUSE);

    LPC_SC->PCONP |= (1 << 15);            
    LPC_PINCON->PINSEL4 &= ~(0x3F << 20); 
    LPC_GPIO2->FIODIR   &= ~(0x7u << 10); 
    LPC_PINCON->PINSEL3 &= ~(0x3FF << 18); 
    LPC_GPIO1->FIODIR   &= ~(0x1F << 25);  

    reset_game(); 
    drawn_piece.type = 99;

    init_RIT(2); 
    /* Solo Timer 1 per evitare sovraccarico emulatore */
    init_timer(1, TIMER1_MATCH_TICKS); 
    
    NVIC_SetPriority(RIT_IRQn, 1);
    NVIC_SetPriority(TIMER1_IRQn, 2);
    
    enable_RIT();
    enable_timer(1);

    LPC_SC->PCON |= 0x1; 
    LPC_SC->PCON &= ~(0x2);
    
    while (1)   
    {
        if (ui_dirty) {
            physics_frozen = 1;
            NVIC_DisableIRQ(TIMER1_IRQn);

            full = ui_full_redraw; ui_full_redraw = 0; 
            ui_dirty = 0;

            oldp = drawn_piece;
            newp = cur;
            
            int do_static = flag_draw_static;
            Piece p_static = piece_to_draw_static;
            flag_draw_static = 0;

            int shifted = board_shifted;
            board_shifted = 0;

            s = score; h = high; l = lines; st = game_state;

            if (full) {
                render_all();
                { int r,c; for(r=0; r<20; r++) for(c=0; c<10; c++) shadow_board[r][c] = (uint8_t)boardgame[r][c]; }
                last_score = -1; last_high = -1; last_lines = -1; last_state = -1;
            } else {
                if (shifted) {
                    int r,c;
                    for (r=0; r<20; r++) {
                        int row_changed = 0;
                        for (c=0; c<10; c++) {
                            uint8_t nv = (uint8_t)boardgame[r][c];
                            if (nv != shadow_board[r][c]) { row_changed = 1; break; }
                        }
                        if (row_changed) {
                            redraw_row_from_board(r);
                            for (c=0; c<10; c++) shadow_board[r][c] = (uint8_t)boardgame[r][c];
                        }
                    }
                } else {
                    if (do_static && p_static.type <= 6) {
                        draw_piece_at(p_static.type, p_static.rot, p_static.r, p_static.c, 1);
                        shadow_apply_locked_piece(shadow_board, &p_static);
                    }
                }
                if (st != STATE_OVER && newp.type <= 6) {
                    if (oldp.type <= 6 && (oldp.r != newp.r || oldp.c != newp.c || oldp.rot != newp.rot)) {
                        erase_piece_restore(&oldp);
                    }
                    draw_piece_at(newp.type, newp.rot, newp.r, newp.c, 1);
                }
            }
            drawn_piece = newp;
            if (s != last_score) { UI_UpdateScore(s); last_score = s; }
            if (h != last_high)  { UI_UpdateHigh(h);  last_high = h; }
            if (l != last_lines) { UI_UpdateLines(l); last_lines = l; }
            if (st != last_state){ UI_UpdateState(st);last_state = st; }

            NVIC_EnableIRQ(TIMER1_IRQn);
            physics_frozen = 0;
        }
        __ASM("wfi");
    }
}