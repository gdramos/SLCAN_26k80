/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef SLCAN_H
#define	SLCAN_H

#include "system.h"

#define FIRMWARE_VERSION        "1020"
#define DUMMY_SERIAL_NUMBER     "GR002"

bool SLCAN_TimestampEnable = false;
bool SLCAN_ExtTimestampEnable = false;

uint8_t atonx(uint8_t byte);
uint8_t atox_byte(uint8_t *b);
uint32_t atox_sid(uint8_t *id);
uint32_t atox_xid(uint8_t *id);
void SLCAN_Print(CAN_Message_t *inbox);
void SLCAN_SetCANBitrate(uint8_t* frame);
void SLCAN_SetCANBitrateRegisters(void);
void SLCAN_OpenCANChannel(void);
void SLCAN_CloseCANChannel(void);
void SLCAN_TxStandardRTR(uint8_t* frame);
void SLCAN_TxExtendedRTR(uint8_t* frame);
void SLCAN_TxStandardFrame(uint8_t* frame);
void SLCAN_TxExtendedFrame(uint8_t* frame);
void SLCAN_ReadFlags(void);
void SLCAN_SetFilterCode(uint8_t* frame);
void SLCAN_SetFilterMask(uint8_t* frame);
void SLCAN_PrintVersion(void);
void SLCAN_PrintSerialNumber(void);
void SLCAN_EnableRxTimestamp(uint8_t* frame);
void SLCAN_SetECANMask(uint8_t* frame);
void SLCAN_SetECANFilter(uint8_t* frame);
void SLCAN_ReadMask(uint8_t* frame);
void SLCAN_ReadFilter(uint8_t* frame);
void SLCAN_GetItem(uint8_t* frame);
void SLCAN_SetSerialBaudrate(uint8_t* frame);
#endif	/* SLCAN_H */

