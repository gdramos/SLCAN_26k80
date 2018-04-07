/* 
 * File:                Eeprom.h
 * Author:              Gerson Ramos
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef EEPROM_H
#define	EEPROM_H

#include "system.h"

//<editor-fold defaultstate="collapsed" desc="Definitions">
#define     BOOT_KEY        0x3FF
#define     APP_MODE        0xAA
//</editor-fold>

uint8_t Eeprom_ptr, Eeprom_size;

void Eeprom_write8(uint8_t byte, uint16_t addr);
void Eeprom_write16(uint16_t word, uint16_t addr);
uint8_t Eeprom_read8(uint16_t addr);
uint16_t Eeprom_read16(uint16_t addr);
void Eeprom_Initialize(void);

#endif	/* EEPROM_H */

