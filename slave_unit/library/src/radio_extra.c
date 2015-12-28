/*******************************************************************************
** radio_extra.c
**
** This file contains all the functions for radio operations & configurations.
** Usually these functions aren't needed. I'm just writing them for completion.
**
*********************************************************************************/

#include <Nordic\reg9e5.h>
#include "util.h"
#include "uart.h"
#include "radio.h"
#include "radio_extra.h"

void SetReceiveMode(void){
	TXEN = 0x00;
}

void SetTransmitMode(void){
	TXEN = 0x01;
}

void SetAutoRetransmit(unsigned char setting){
	unsigned char tmp;

	RACSN = 0;
	SpiReadWrite(RRC | 0x01);
	tmp = SpiReadWrite(0) & 0xDF;
	RACSN = 1;

	RACSN = 0;
	SpiReadWrite(WRC | 0x01);
	SpiReadWrite(tmp | (setting <<5));
	RACSN = 1;
}

void SetPayloadWidth(unsigned char w){
	//Configure RF
	RACSN = 0;
	SpiReadWrite(WRC | 0x03);
	SpiReadWrite(w);
	SpiReadWrite(w);
	RACSN = 1;
}

void SetTXPower(unsigned char powerlevel){
	unsigned char tmp;

	RACSN = 0;
	SpiReadWrite(RRC | 0x01);
	tmp = SpiReadWrite(0) & 0xF3;
	RACSN = 1;

	RACSN = 0;
	SpiReadWrite(WRC | 0x01);
	SpiReadWrite(tmp | (powerlevel <<2));
	RACSN = 1;
}

void SetFrequency(unsigned char freq){
	unsigned char tmp;

	RACSN = 0;
	SpiReadWrite(RRC | 0x01);
	tmp = SpiReadWrite(0) & 0xFD;
	RACSN = 1;

	RACSN = 0;
	SpiReadWrite(WRC | 0x01);
	SpiReadWrite(tmp | (freq <<1));
	RACSN = 1;
}

void SetLowRXPower(unsigned char flag){
	unsigned char tmp;

	RACSN = 0;
	SpiReadWrite(RRC | 0x01);
	tmp = SpiReadWrite(0) & 0xEF;
	RACSN = 1;

	RACSN = 0;
	SpiReadWrite(WRC | 0x01);
	SpiReadWrite(tmp | (flag <<4));
	RACSN = 1;
}

//*addr is the pointer to RX address, addr_size must be 1-4
void SetRXAddress(unsigned char addr[], unsigned char addr_size){
	unsigned char tmp, i;

	RACSN = 0;
	SpiReadWrite(RRC | 0x02);
	tmp = SpiReadWrite(0) & 0xF8;
	RACSN = 1;

	RACSN = 0;
	SpiReadWrite(WRC | 0x02);
	SpiReadWrite(tmp | (addr_size));
	RACSN = 1;


	RACSN = 0;
	SpiReadWrite(WRC | 0x05);
	for(i=0;i<addr_size;i++){
		SpiReadWrite(*addr);
		addr++;
	}
	RACSN = 1;
}

//*addr is the pointer to TX address, addr_size must be 1-4
void SetTXAddress(unsigned char addr[], unsigned char addr_size){
	unsigned char tmp, i;

	RACSN = 0;
	SpiReadWrite(RRC | 0x02);
	tmp = SpiReadWrite(0) & 0xF8;
	RACSN = 1;

	RACSN = 0;
	SpiReadWrite(WRC | 0x02);
	SpiReadWrite(tmp | (addr_size<<4));
	RACSN = 1;

	RACSN = 0;
	SpiReadWrite(WTA);
	for(i=0;i<addr_size;i++){
		SpiReadWrite(*addr);
		addr++;
	}
	RACSN = 1;
}

void RadioPowerUpDown(unsigned char val){
	if(val == 0)
		TRX_CE = 0;
	else if(val == 1)
		TRX_CE = 1;
}

void SetCRCMode(unsigned char flag){
	unsigned char tmp;

	RACSN = 0;
	SpiReadWrite(RRC | 0x09);
	tmp = SpiReadWrite(0) & 0x7F;
	RACSN = 1;

	RACSN = 0;
	SpiReadWrite(WRC | 0x09);
	SpiReadWrite(tmp | (flag <<7));
	RACSN = 1;
}

void CRCEnableDisable(unsigned char flag){
	unsigned char tmp;

	RACSN = 0;
	SpiReadWrite(RRC | 0x09);
	tmp = SpiReadWrite(0) & 0x7F;
	RACSN = 1;

	RACSN = 0;
	SpiReadWrite(WRC | 0x09);
	SpiReadWrite(tmp | (flag <<6));
	RACSN = 1;
}
