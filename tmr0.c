#include "tmr0.h"

void TMR0_Initialize(void)
{
    T0CON = 0x00;
    T0CONbits.PSA = 1;  /* No prescaler */
    
    /* 1ms interrupts */
    TMR0H = 0xC1;
    TMR0L = 0x7F;
    
    INTCONbits.TMR0IF = 0;
    INTCONbits.TMR0IE = 1;
    
    T0CONbits.TMR0ON = 1;
    
    TMR0_InterruptHandler = NULL;
}

void TMR0_Reload(void)
{
    TMR0H = 0xC1;
    TMR0L = 0x7F;
}

void TMR0_SetInterruptHandler(void* interruptHandler)
{
    TMR0_InterruptHandler = interruptHandler;
}

void TMR0_ISR(void)
{
    INTCONbits.TMR0IF = 0;
    TMR0_Reload();
    
    if(TMR0_InterruptHandler != NULL)
    {
        TMR0_InterruptHandler();
    }
}

