/*******************************************************************************
** eeprom.c
**
** This file contains all the functions for EEPROM operations
**
*********************************************************************************/

#include <Nordic\reg9e5.h>
#include "util.h"
#include "eeprom.h"

void InitEEPROM(void){
//  P1_DIR &= ~0x1b;
    SPICLK = 5;                             // CLK/32 SPI clock

    EECSN = 1;
    SPI_CTRL = 0x01;                        // Connect internal SPI to P1
}

unsigned char EEStatus(void){
    unsigned char b;

    EECSN = 0;
    SpiReadWrite(EE_RDSR);
    b = SpiReadWrite(0);
    EECSN = 1;
    return b;
}

unsigned char EERead(unsigned int addr){
    unsigned char b;

    while ((EEStatus() & 0x01) != 0x00)     // Wait if busy
        ;
    EECSN = 0;
    SpiReadWrite(EE_READ);
    SpiReadWrite(addr >> 8);
    SpiReadWrite(addr & 0xff);
    b = SpiReadWrite(0);
    EECSN = 1;
    return b;
}

void EEWrite(unsigned int addr, unsigned char b){
    while((EEStatus() & 0x01) != 0x00)      // Wait if busy
        ;
    EECSN = 0;
    SpiReadWrite(EE_WREN);
    EECSN = 1;
    EECSN = 0;
    SpiReadWrite(EE_WRITE);
    SpiReadWrite(addr >> 8);
    SpiReadWrite(addr & 0xff);
    SpiReadWrite(b);
    EECSN = 1;
}
