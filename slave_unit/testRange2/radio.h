/********************************************************************
** 	radio.h
**
**	This file is the header file for radio.c, radio_master.c and radio_slave.c
**
*********************************************************************/

#define HFREQ 0		// 0=433MHz, 1=868/915MHz
#define POWER 3 	// 0=min power...3 = max power
#define PACKET_SIZE 0x10


//declaration for radio.c
extern void InitRF(void);
extern void TransmitPacket(unsigned char b[PACKET_SIZE]);
extern unsigned char ReceivePacket(unsigned char b[PACKET_SIZE], unsigned char timeout);
