/*
 * File:   Eeprom.c
 * Author: Gerson Ramos
 *
 * Created on August 29, 2016, 9:07 PM
 */


#include "eeprom.h"

void Eeprom_Initialize(void)
{
    EECON1 = 0;
    Eeprom_ptr = 0;
    Eeprom_size = 0;
}

void Eeprom_write8(uint8_t byte, uint16_t addr)
{
    if(addr > 0x3FF)
        return;
    
    EEADRH = addr >> 8;
    EEADR = addr;
    EEDATA = byte;
    WREN = 1;
    INTCONbits.GIE = 0;
    EECON2 = 0x55;
    EECON2 = 0xAA;
    WR = 1;
    while(WR);
    INTCONbits.GIE = 1;
    WREN = 0;

}

void Eeprom_write16(uint16_t word, uint16_t addr)
{
    if(addr > 0x3FE)
        return;
    
    Eeprom_write8(word >> 8, addr);
    Eeprom_write8(word, addr+1);
}

uint8_t Eeprom_read8(uint16_t addr)
{
    if(addr > 0x3FF)
        return 0;
    
    EEADRH = addr >> 8;
    EEADR = addr;
    EEPGD = 0;
    CFGS = 0;
    RD = 1;
    NOP();
    return EEDATA;
}

uint16_t Eeprom_read16(uint16_t addr)
{
    if(addr > 0x3FE)
        return 0;
    
    uint16_t word;
    word = Eeprom_read8(addr);
    word = word << 8;
    word |= Eeprom_read8(addr+1);
    
    return word;
}
