#include "LPC17xx.h"
#include "timer/timer.h"
#include "../utils.h"
#include "../game_config.h" 

extern volatile int joy_left_event, joy_right_event, joy_up_event, joy_click_event, joy_down_held;
extern volatile int key1_event, key2_event, int0_event;

volatile uint16_t debug_adc_val = 0;
volatile uint32_t debug_speed_ms = 1000;

void TIMER1_IRQHandler(void)
{
    if (physics_frozen) { LPC_TIM1->IR = 1; return; }

    static int acc_ms = 0;
    int do_left, do_right, do_rot, do_pause, do_down, do_hard, do_reset, cleared;
    uint8_t nr; Piece before;
    
    uint32_t current_grav_ms = MIN_SPEED_MS;

    NVIC_DisableIRQ(RIT_IRQn);
    do_left=joy_left_event; joy_left_event=0; do_right=joy_right_event; joy_right_event=0;
    do_rot=joy_up_event; joy_up_event=0; do_pause=key1_event; key1_event=0;
    do_hard=key2_event; key2_event=0; do_reset=int0_event; int0_event=0; do_down=joy_down_held;
    NVIC_EnableIRQ(RIT_IRQn);

    if(do_reset) { reset_game(); acc_ms=0; LPC_TIM1->IR = 1; return; }
    if(do_pause) {
        if (game_state == STATE_OVER) { reset_game(); game_state = STATE_PLAY; spawn_piece(); acc_ms=0; physics_frozen=1; ui_dirty=1; }
        else if (game_state == STATE_PLAY) { game_state = STATE_PAUSE; ui_dirty=1; } 
        else { game_state = STATE_PLAY; if(cur.type>6){spawn_piece(); acc_ms=0; physics_frozen=1;} ui_dirty=1; }
        LPC_TIM1->IR = 1; return;
    }
    if(game_state != STATE_PLAY || cur.type > 6) { LPC_TIM1->IR = 1; return; }

    before = cur;
    if(do_hard) {
        while(can_place(cur.type, cur.rot, cur.r+1, cur.c)) cur.r++;
        piece_to_draw_static = cur; flag_draw_static = 1; lock_piece(); cleared = clear_lines();
        if (cleared > 0) board_shifted = 1; spawn_piece(); acc_ms = 0; physics_frozen = 1; ui_dirty = 1;
        LPC_TIM1->IR = 1; return;
    }

    while(do_left--)  if(can_place(cur.type, cur.rot, cur.r, cur.c-1)) cur.c--;
    while(do_right--) if(can_place(cur.type, cur.rot, cur.r, cur.c+1)) cur.c++;
    if(do_rot) { nr = (uint8_t)((cur.rot + 1) % 4); if(can_place(cur.type, nr, cur.r, cur.c)) cur.rot = nr; }

    /* Velocità */
    if (powerup_slow_timer > 0) {
        powerup_slow_timer--;
        current_grav_ms = MIN_SPEED_MS; 
    } else {
        debug_adc_val = ADC_Read(); 
        if(debug_adc_val > 0xFFF) debug_adc_val = 0xFFF;
        uint32_t speed_reduction = ((uint32_t)debug_adc_val * 800) / 4095;
        current_grav_ms = MIN_SPEED_MS - speed_reduction;
    }
    debug_speed_ms = current_grav_ms;
    
    if (do_down) {
        current_grav_ms = current_grav_ms / 2;
        if(current_grav_ms < 50) current_grav_ms = 50;
    }

    acc_ms += GAME_TICK_MS; 
    
    if(acc_ms >= (int)current_grav_ms) {
        acc_ms = 0; 
        if(can_place(cur.type, cur.rot, cur.r+1, cur.c)) {
            cur.r++;
        } else {
            piece_to_draw_static = cur; flag_draw_static = 1; lock_piece(); cleared = clear_lines();
            if (cleared > 0) board_shifted = 1; spawn_piece(); physics_frozen = 1; ui_dirty = 1;
        }
    }

    if(cur.r!=before.r || cur.c!=before.c || cur.rot!=before.rot) ui_dirty = 1;
    LPC_TIM1->IR = 1;
}