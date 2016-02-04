/*******************************************************************************
** eeprom.c
**
** ALL FUNCTIONS COPIED FROM NORDIC'S SAMPLE CODES
**
*********************************************************************************/

#include <Nordic\reg9e5.h>
#include "util.h"
#include "eeprom.h"

//Initialize EEPROM
//Note: SPI will be connected to port 1. This disables the RF until SPI_CTRL=2;
void InitEEPROM(void){
//  P1_DIR &= ~0x1b;
    SPICLK = 5;                             // CLK/32 SPI clock
    EECSN = 1;
    SPI_CTRL = 0x01;                        // Connect internal SPI to P1
}

//Returns EEPROM status. Not used in main code.
unsigned char EEStatus(void){
    unsigned char b;
    EECSN = 0;
    SpiReadWrite(EE_RDSR);
    b = SpiReadWrite(0);
    EECSN = 1;
    return b;
}

// Returns the byte content at addr of EEPROM
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

//Writes byte to addr at EEPROM
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
