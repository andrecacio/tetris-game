#ifndef __RIT_H
#define __RIT_H
#include <stdint.h>

/* init_RIT.c */
/* Cambiato nome parametro da RITInterval a RIT_MS per chiarezza */
extern uint32_t init_RIT( uint32_t RIT_MS ); 

extern void enable_RIT( void );
extern void disable_RIT( void );
extern void reset_RIT( void );

/* IRQ_RIT.c */
extern void RIT_IRQHandler (void);

#endif /* end __RIT_H */