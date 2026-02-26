#ifndef GAME_CONFIG_H
#define GAME_CONFIG_H
#include <stdint.h>

#define TIMER0_MATCH_TICKS   0x4B0u   
#define TIMER1_MATCH_TICKS   0x249F0u   

#define INPUT_TICK_MS        1u
/* METTI QUESTO A 50u PER L'EMULATORE, NON 6u */
#define GAME_TICK_MS         50u  

#define MIN_SPEED_MS         1000u 
#define MAX_SPEED_MS         200u  

#define PWR_CLEAR_HALF       10    
#define PWR_SLOW             11    

/* Note */
#define NOTE_C4  262
#define NOTE_D4  294
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_D5  587
#define NOTE_E5  659

#define DB_SAMPLES         10u
#define EVT_CAP             8

#endif