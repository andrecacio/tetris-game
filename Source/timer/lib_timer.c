#include "LPC17xx.h"
#include "timer.h"

void enable_timer( uint8_t timer_num )
{
  if ( timer_num == 0 ) LPC_TIM0->TCR = 1;
  else LPC_TIM1->TCR = 1;
}

void disable_timer( uint8_t timer_num )
{
  if ( timer_num == 0 ) LPC_TIM0->TCR = 0;
  else LPC_TIM1->TCR = 0;
}

void reset_timer( uint8_t timer_num )
{
  uint32_t regVal;
  if ( timer_num == 0 ) {
    regVal = LPC_TIM0->TCR;
    regVal |= 0x02;
    LPC_TIM0->TCR = regVal;
  } else {
    regVal = LPC_TIM1->TCR;
    regVal |= 0x02;
    LPC_TIM1->TCR = regVal;
  }
}

uint32_t init_timer ( uint8_t timer_num, uint32_t TimerInterval )
{
  if ( timer_num == 0 ) {
    LPC_TIM0->MR0 = TimerInterval;
    LPC_TIM0->MCR = 3; // Interrupt + Reset on Match
    NVIC_EnableIRQ(TIMER0_IRQn);
    // NVIC_SetPriority impostata nel main
    return (1);
  }
  else if ( timer_num == 1 ) {
    LPC_TIM1->MR0 = TimerInterval;
    LPC_TIM1->MCR = 3;
    NVIC_EnableIRQ(TIMER1_IRQn);
    return (1);
  }
  return (0);
}