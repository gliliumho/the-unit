/**********************************************************************
** eeprom.h
** 
** This file is the header file for eeprom.c
**
************************************************************************/

#define EE_WRSR     0x01
#define EE_WRITE    0x02
#define EE_READ     0x03
#define EE_WRDI     0x04
#define EE_RDSR     0x05
#define EE_WREN     0x06

extern void InitEEPROM(void);
extern unsigned char EEStatus(void);
extern unsigned char EERead(unsigned int addr);
extern void EEWrite(unsigned int addr, unsigned char b);