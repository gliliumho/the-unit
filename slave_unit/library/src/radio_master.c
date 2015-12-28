/*******************************************************************************
** slave_master.c
** This file contains all the functions for radio operations & configurations.
**
*********************************************************************************/

#include <Nordic\reg9e5.h>
#include <stdlib.h>
#include "util.h"
#include "uart.h"
#include "radio.h"

/*
0x01 for first byte. Master only

*/
void SendTraffic( unsigned char b[] ){
	unsigned char i;

	b[0] = TRAFFIC_INFO_HEADER;		//MACRO
	b[1] = packet_count;			//global

	TXEN = 1;
	for(i=0; i<5; i++)
		TransmitPacket(b);
	packet_count++; 				//global
}


//0x02 for first byte. Master only
void RequestHeartbeat(unsigned char gid, unsigned char uid){
	unsigned char i;
	unsigned char b[PACKET_SIZE];

	b[0] = HEARTBEAT_REQUEST_HEADER;
	b[1] = gid;
	b[2] = uid;
	b[3] = packet_count;
	for(i=4; i<PACKET_SIZE; i++)
		b[i] = 0x00;

	TXEN = 1;
	for(i=0; i<5; i++)
		TransmitPacket(b);
	packet_count++;
}

//0x04 for first byte. Master only
void RequestHeartbeatFromAll(void){
	unsigned char i;
	unsigned char b[PACKET_SIZE];

	b[0] = HEARTBEAT_REQUEST_FROM_ALL_HEADER;
	b[1] = 0x00;
	b[2] = 0x00;
	b[3] = packet_count;
	for(i=4; i<PACKET_SIZE; i++)
		b[i] = 0x00;

	TXEN = 1;
	for(i=0; i<5; i++)
		TransmitPacket(b);
	packet_count++;
}

//called by WaitHeartbeat() and WaitMultiHeartbeat(). Basically ReceivePacket()
//with timeout
unsigned char ListenHeartbeat(unsigned char *gid, unsigned char *uid){
	unsigned char b[PACKET_SIZE];
	unsigned char i=0, j=0;

	TXEN = 0;
	TRX_CE = 1;

	while(DR == 0 && i < 50){
		Delay400us(10);
		i++;
	}

	if(DR != 1)
		return 0;

	RACSN = 0;
	SpiReadWrite(RRP);
	for(i=0; i<PACKET_SIZE; i++)
		b[i] = SpiReadWrite(0);
	RACSN = 1;
	TRX_CE = 0;

	if( b[0]==HEARTBEAT_REPLY_HEADER ){
		*gid = b[1];
		*uid = b[2];
		return 1;
	}
	return 0;
}


//called after RequestHeartbeat()
unsigned char WaitHeartbeat(unsigned char gid, unsigned char uid){
	unsigned char ret, a, b;

	ret = ListenHeartbeat(&a, &b);
	if( gid != a || uid != b )
		ret = 0;

	if(ret){
		// PutString("Received heartbeat from slave ");
		// PrintChar(gid);
		// PutChar(0x20);
		// PrintChar(uid);
		// PutString("\r\n");
		return 1;
	} else {
		return 0;
	}

}

//called after RequestHeartbeatFromAll
unsigned char WaitMultiHeartbeat( unsigned char id[][2], unsigned char id_len){
	unsigned char ret, i=0;
	unsigned char fails=0;
	while( i < id_len && fails < 50 ){
		unsigned char j, gid, uid;

		ret = ListenHeartbeat( &gid, &uid );
		for(j=0; j<=i; j++)
			if( gid == id[j][0] && uid == id[j][1] )
				ret = 0;

		if(ret){
			id[i][0] = gid;
			id[i][1] = uid;
			i++;
		} else
			fails++;
	}
	return i;
}

//request heartbeat from all slaves by looping
void RequestHeartbeatLoop(void){
	unsigned char i, ret;
	unsigned char groupID = 0;
	unsigned char uniqueID = 0;

	PutString("Scanning for slaves alive..\r\n");
	PutString("Found the following slaves: \r\n");

	for(groupID=1; groupID<5; groupID++){
		for(uniqueID=3; uniqueID<8; uniqueID++){
			RequestHeartbeat(groupID,uniqueID);
			i = 0;
			ret = 0;
			while(ret == 0 && i < 10){
				ret = WaitHeartbeat(groupID, uniqueID);
				i++;
			}

			PrintChar(groupID);
			PutChar(0x20);
			PrintChar(uniqueID);
			PutChar(0x20);

			if(ret){
				PutString("Alive!\r\n");
			} else
				PutString("Timeout\r\n");

			Delay5ms(10);

		}
	}
}
