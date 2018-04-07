/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef TIMER0_H
#define	TIMER0_H

#include "system.h" 

void TMR0_Initialize(void);
void TMR0_Reload(void);
void TMR0_SetInterruptHandler(void* interruptHandler);
void TMR0_ISR(void);

void (*TMR0_InterruptHandler)(void);

#endif	/* TIMER0_H */

