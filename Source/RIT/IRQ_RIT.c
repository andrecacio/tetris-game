#include "LPC17xx.h"
#include "RIT.h"
#include "../utils.h"

/* Variabili Input Esterne */
extern volatile int joy_left_event, joy_right_event, joy_up_event, joy_click_event, joy_down_held;
extern volatile int key1_event, key2_event, int0_event;

/* MACRO LETTURA PIN */
#define READ_KEY(port, bit) (((port->FIOPIN & (1u<<(bit))) == 0u) ? 1 : 0)

void RIT_IRQHandler(void)
{
    static int left_cnt=0, right_cnt=0;
    
    /* Latch flags per evitare ripetizioni */
    static int k1_locked = 0; 
    static int k2_locked = 0;
    static int i0_locked = 0;
    static int up_locked = 0; 
    static int clk_locked = 0;
    
    /* JOYSTICK LEFT  */
    if(READ_KEY(LPC_GPIO1, 27)) {
        left_cnt++;
        if(left_cnt == 1 || (left_cnt > 30 && (left_cnt % 5) == 0)) { 
            if(joy_left_event < 8) joy_left_event++;
        }
    } else left_cnt = 0;
    
    /* JOYSTICK RIGHT */
    if(READ_KEY(LPC_GPIO1, 28)) {
        right_cnt++;
        if(right_cnt == 1 || (right_cnt > 30 && (right_cnt % 5) == 0)) {
            if(joy_right_event < 8) joy_right_event++;
        }
    } else right_cnt = 0;
    
    /* JOYSTICK UP  */
    if(READ_KEY(LPC_GPIO1, 29)) {
        if(!up_locked) {
            if(joy_up_event < 8) joy_up_event++;
            up_locked = 1; 
        }
    } else up_locked = 0;
    
    /* JOYSTICK DOWN */
    joy_down_held = READ_KEY(LPC_GPIO1, 26);
    
    /* JOYSTICK CLICK */
    if(READ_KEY(LPC_GPIO1, 25)) {
        if(!clk_locked) {
            if(joy_click_event < 8) joy_click_event++;
            clk_locked = 1;
        }
    } else clk_locked = 0;
    
    /* KEY1 (Pause) */
    if(READ_KEY(LPC_GPIO2, 11)) {
        if(!k1_locked) {       
            key1_event = 1;    
            k1_locked = 1;     
        }
    } else k1_locked = 0;
    
    /* KEY2 (Hard Drop)*/
    if(READ_KEY(LPC_GPIO2, 12)) {
        if(!k2_locked) {
            key2_event = 1;    
            k2_locked = 1;     
        }
    } else k2_locked = 0; 
    
    /* KEY0 (Reset)*/
    if(READ_KEY(LPC_GPIO2, 10)) {
        if(!i0_locked) {
            int0_event = 1;
            i0_locked = 1;
        }
    } else i0_locked = 0; 
    
    /* CLEAR RIT FLAG  */
    LPC_RIT->RICTRL |= 1; 
}