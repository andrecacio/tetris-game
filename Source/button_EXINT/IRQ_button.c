#include "button.h"
#include "LPC17xx.h"

/* FILE SVUOTATO INTENZIONALMENTE 
   La gestione dei bottoni è fatta in IRQ_timer.c tramite Polling.
   Questi interrupt sono disabilitati.
*/

void EINT0_IRQHandler (void) {}
void EINT1_IRQHandler (void) {}
void EINT2_IRQHandler (void) {}