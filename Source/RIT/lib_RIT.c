#include "LPC17xx.h"
#include "RIT.h"

void enable_RIT( void )
{
	LPC_RIT->RICTRL |= (1<<3);	
}

void disable_RIT( void )
{
	LPC_RIT->RICTRL &= ~(1<<3);	
}

void reset_RIT( void )
{
	LPC_RIT->RICTRL |= (1<<4); /* Reset counter flag */
	LPC_RIT->RICOUNTER = 0;
}

uint32_t init_RIT ( uint32_t RIT_MS )
{

	LPC_SC->PCLKSEL1 &= ~(3<<26);
	LPC_SC->PCLKSEL1 |=  (1<<26); 
	
	LPC_SC->PCONP    |=  (1<<16);  
	
	

	LPC_RIT->RICOMPVAL = (uint32_t)(100000 * RIT_MS);


	LPC_RIT->RICTRL = (1<<1) | (1<<2);	
	LPC_RIT->RICOUNTER = 0;
	
	
	NVIC_EnableIRQ(RIT_IRQn);
	/* Priorità RIT (1) deve essere più alta (numero minore) del Timer Gioco (2) */
	NVIC_SetPriority(RIT_IRQn, 1);
	
	return (0);
}