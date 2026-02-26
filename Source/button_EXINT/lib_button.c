#include "button.h"
#include "LPC17xx.h"

void BUTTON_init(void) {
    /* RESET PIN P2.10 (Reset), P2.11 (Pause), P2.12 (Hard Drop) */
    
    /* 1. PINSEL4: Bits 20-25 a 0 -> Funzione GPIO */
    LPC_PINCON->PINSEL4 &= ~(0x03F00000); 

    /* 2. PINMODE4: Bits 20-25 a 0 -> Pull-up Resistor */
    LPC_PINCON->PINMODE4 &= ~(0x03F00000);

    /* 3. FIODIR2: Bits 10-12 a 0 -> Input */
    LPC_GPIO2->FIODIR &= ~(0x00001C00);
    
    /* Disabilita Interrupt Hardware per evitare conflitti */
    NVIC_DisableIRQ(EINT0_IRQn);
    NVIC_DisableIRQ(EINT1_IRQn);
    NVIC_DisableIRQ(EINT2_IRQn);
}