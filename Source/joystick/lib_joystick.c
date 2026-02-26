#include "LPC17xx.h"
#include "joystick.h"

void joystick_init(void) {
    /* Joystick LandTiger: Port 1.25 -> 1.29 */
    
    // 1. PINSEL: Function 00 (GPIO) per P1.25 a P1.29
    // Bit 18-27 del PINSEL3 gestiscono P1.25-P1.29? No, PINSEL3 gestisce P1.
    // Verifica Manuale LPC1768: P1.25-29 default è GPIO (00).
    // Ma per sicurezza puliamo i bit relativi in PINSEL3.
    LPC_PINCON->PINSEL3 &= ~( (3u<<18) | (3u<<20) | (3u<<22) | (3u<<24) | (3u<<26) );

    // 2. FIODIR: 0 = Input
    LPC_GPIO1->FIODIR &= ~( (1u<<25) | (1u<<26) | (1u<<27) | (1u<<28) | (1u<<29) );
}