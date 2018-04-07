/* 
 * File:                Serial.h
 * Author:              Gerson Ramos    
 * Comments:            Serial Communication Stuff
 * Revision history:    Who keeps track of these things
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef SERIAL_H
#define	SERIAL_H

#include "system.h"

//<editor-fold defaultstate="collapsed" desc="Variables">
uint8_t RXDATA;
uint8_t *UART1_BufferPtr;
uint8_t *UART1_BufferBase;
//</editor-fold>

typedef enum
{
    BAUD_300        =       0xD054,
    BAUD_1200       =       0x3114,
    BAUD_2400       =       0x1A0A,
    BAUD_9600       =       0x0682,
    BAUD_19200      =       0x0340,
    BAUD_38400      =       0x01A0,
    BAUD_57600      =       0x0115,
    BAUD_115200     =       0x008A,
    BAUD_230400     =       0x0044,
    BAUD_256000     =       0x003E,
    BAUD_460800     =       0x0022,
    BAUD_921600     =       0x0010,
    BAUD_1000000    =       0x000F,
    BAUD_2000000    =       0x0007,
    BAUD_3000000    =       0x0004,
    BAUD_4000000    =       0x0003
}UART1_Baud_t;

//<editor-fold defaultstate="collapsed" desc="Prototypes">
void UART1_Initialize(UART1_Baud_t baud);
void UART1_SetBuffer(uint8_t *buffer);
void UART1_ClearBuffer(void);
void UART1_ISR(void);
void UART1_SetInterruptHandler(void* interruptHandler);
void UART1_SetFrameReceivedHandler(void* frameReceivedHandler);
void UART1_CallBack(void);

void (*UART1_InterruptHandler)(void);
void (*UART1_FrameReceivedHandler)(void);
//</editor-fold>

#endif	/* SERIAL_H */

