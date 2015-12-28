/********************************************************************
** 	radio_extra.h
**
**	This file is the header file for functions in radio_extra.c
**
*********************************************************************/

extern void SetReceiveMode(void);
extern void SetTransmitMode(void);
extern void SetAutoRetransmit(unsigned char setting);
extern void SetPayloadWidth(unsigned char w);
extern void SetTXPower(unsigned char powerlevel);
extern void SetFrequency(unsigned char freq);
extern void SetLowRXPower(unsigned char flag);
extern void SetRXAddress(unsigned char addr[], unsigned char addr_size);
extern void SetTXAddress(unsigned char addr[], unsigned char addr_size);
extern void RadioPowerUpDown(unsigned char val);
extern void SetCRCMode(unsigned char flag);
extern void CRCEnableDisable(unsigned char flag);
