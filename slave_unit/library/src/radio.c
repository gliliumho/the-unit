/*******************************************************************************
** radio.c
** This file contains all the functions for radio operations & configurations.
**
*********************************************************************************/

#include <Nordic\reg9e5.h>
#include <stdlib.h>
#include "util.h"
#include "uart.h"
#include "radio.h"

packetID id_buffer[6] = {0};
unsigned char packet_count = 0;
unsigned char buffer_count = 0;

//returns 1 if same. Returns 0 if not same.
unsigned char PacketIdEqual(packetID a, packetID b){
	if((a.groupid==b.groupid) && (a.uniqueid==b.uniqueid) && (a.packet_no==b.packet_no))
		return 1;
	else
		return 0;
}

void PacketIdCpy(packetID *dest, packetID src){
	dest->groupid = src.groupid;
	dest->uniqueid = src.uniqueid;
	dest->packet_no = src.packet_no;
}

void InitRF(void){
	unsigned char tmp, cklf;

	SPICLK = 0;						//Max SPI clock
	SPI_CTRL = 0x02;

	// Switch to 16MHz clock:
	RACSN = 0;
	SpiReadWrite(RRC | 0x09);
	cklf = SpiReadWrite(0) | 0x04;	//XO_DIRECT = 1, follow XO_Frequency
	RACSN = 1;
	RACSN = 0;
	SpiReadWrite(WRC | 0x09);
	SpiReadWrite(cklf);
	RACSN = 1;

	//Configure RF
	RACSN = 0;
	SpiReadWrite(WRC | 0x03);	   	// Write to RF config address 3 (RX payload)
	SpiReadWrite(PACKET_SIZE);		// 16 byte RX payload width
	SpiReadWrite(PACKET_SIZE);		// 16 byte TX payload width
	RACSN = 1;

	RACSN = 0;
	SpiReadWrite(RRC | 0x01);	   	// Read RF config address 1
	tmp = SpiReadWrite(0) & 0xf1;   // Clear the power and frequency setting bits
	RACSN = 1;

	RACSN = 0;
	SpiReadWrite(WRC | 0x01);	  // Write RF config address 1
	// Change power defined by POWER and to 433 or 868/915MHz defined by HFREQ above:
	SpiReadWrite(tmp | (POWER << 2) | (HFREQ << 1));
	RACSN = 1;
}


void TransmitPacket(unsigned char b[PACKET_SIZE] ){
	unsigned char i;

	RACSN = 0;
	SpiReadWrite(WTP);					//Write to TX payload
	for(i=0; i<PACKET_SIZE; i++)
		SpiReadWrite(b[i]);
	RACSN = 1;

	TRX_CE = 1;
	Delay400us(PACKET_SIZE);
	TRX_CE = 0;
}

void ReceivePacket(unsigned char b[PACKET_SIZE]){
	unsigned char i;

	TRX_CE = 1;
	while(DR == 0)
		;

	RACSN = 0;
	SpiReadWrite(RRP);				//Read receive payload
	for(i=0; i<PACKET_SIZE; i++)
		b[i] = SpiReadWrite(0);
	RACSN = 1;
	TRX_CE = 0;
}
