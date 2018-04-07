#include "uart1.h"

/* Assumes FOSC = 64MHz */

/* override putch in order to use printf() to output to serial. see XC8 User Manual for details */
void putch(unsigned char data)
{
    while(!TXSTA1bits.TRMT);
    TXREG1 = data;
}

void UART1_Initialize(UART1_Baud_t baud)
{
    TRISCbits.TRISC7 = 1;           //RX1 -> RC7: input [1]
    
    TRISCbits.TRISC6 = 0;           //TX1 -> RC6: output [0]
    
    TXSTA1 = 0x24;                  //0b00100100;
    
    RCSTA1 = 0x90;                  //0b10010000;
    
    BAUDCON1bits.BRG16 = 1;         //16-bit baud configuration
    
    SPBRG1 = baud & 0x00FF;                  
    
    SPBRGH1 = (baud >> 8) & 0x00FF;
    
    UART1_FrameReceivedHandler = NULL;
    
    UART1_InterruptHandler = NULL;
}

void UART1_SetBuffer(uint8_t *buffer)
{
    UART1_BufferPtr = buffer;
    UART1_BufferBase = buffer;
}

void UART1_ClearBuffer(void)
{
    if(UART1_BufferPtr != UART1_BufferBase)
    {
        do
        {
            *UART1_BufferPtr-- = 0;
        }while(UART1_BufferPtr > UART1_BufferBase);
        *UART1_BufferPtr = 0;
    }
}

void UART1_SetInterruptHandler(void* interruptHandler)
{
    UART1_InterruptHandler = interruptHandler;
}

void UART1_SetFrameReceivedHandler(void* frameReceivedHandler)
{
    UART1_FrameReceivedHandler = frameReceivedHandler;
}

void UART1_ISR(void)
{
    /* read Byte received into byte buffer */
    RXDATA = RCREG1;
    /* clear the interrupt flag*/
    PIR1bits.RC1IF = 0;
    
    /* handle frame errors */
    if(RCSTA1bits.FERR)
        RXDATA = RCREG1;

    /* handle overflow error */
    if(RCSTA1bits.OERR)
    {
        RCSTA1bits.CREN = 0;
        RCSTA1bits.CREN = 1;
    }

    /* check if end of frame */
    if(RXDATA == 0x0D)
    {
        /* flag end of frame */
        *UART1_BufferPtr++ = 0x0D;
        if(UART1_FrameReceivedHandler != NULL)
        {
            UART1_FrameReceivedHandler();
        }
        //UART1_ClearBuffer();
        UART1_BufferPtr = UART1_BufferBase;
    }
    else
    {
        /* add to frame buffer */
        *UART1_BufferPtr++ = RXDATA;
    }
}