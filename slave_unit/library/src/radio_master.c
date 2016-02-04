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


// Sends traffic info to all slaves.
// Traffic info:
// 1 - free flow / little traffic
// 2 - little congestion / slow traffic
// 3 - very congested / very slow traffic
//
// b[2] to b[15] contains traffic info
// 0x01 for first byte. Master only
void SendTraffic( unsigned char b[] ){
	unsigned char i;

	//make sure the header is correct
	b[0] = TRAFFIC_INFO_HEADER;		//MACRO
	b[1] = packet_count;			//global

	TXEN = 1;				//make sure radio is set to TX
	//transmit 5 times
	for(i=0; i<5; i++)
		TransmitPacket(b);

	//track count of packets sent
	packet_count++; 				//global
}


//Transmits heartbeat request packet to get heartbeat from
// group=gid and uniqueID = uid
//0x02 for first byte. Master only
void RequestHeartbeat(unsigned char gid, unsigned char uid){
	unsigned char i;

	//Generate the heartbeat request packet
	unsigned char b[PACKET_SIZE];
	b[0] = HEARTBEAT_REQUEST_HEADER;
	b[1] = gid;
	b[2] = uid;
	b[3] = packet_count;
	for(i=4; i<PACKET_SIZE; i++)
		b[i] = 0x00;

	TXEN = 1;				//make sure radio is set to TX
	//transmit 5 times to make sure slaves receive it
	for(i=0; i<5; i++)
		TransmitPacket(b);
	//track count of packets sent
	packet_count++;
}

//EXPERIMENTAL feature (not really successful)
// Transmit heartbeat request packet but all slaves will
// reply to it.
//0x04 for first byte. Master only
void RequestHeartbeatFromAll(void){
	unsigned char i;

	//Generate heartbeat request packet
	unsigned char b[PACKET_SIZE];
	b[0] = HEARTBEAT_REQUEST_FROM_ALL_HEADER;
	b[1] = 0x00;
	b[2] = 0x00;
	b[3] = packet_count;
	for(i=4; i<PACKET_SIZE; i++)
		b[i] = 0x00;

	TXEN = 1; 				//make sure radio is set to TX
	for(i=0; i<5; i++)
		TransmitPacket(b);
	packet_count++;
}

//Basically ReceivePacket() with timeout.
//Called by WaitHeartbeat() and WaitMultiHeartbeat().
unsigned char ListenHeartbeat(unsigned char *gid, unsigned char *uid){
	unsigned char i=0, j=0;
	unsigned char b[PACKET_SIZE];

	TXEN = 0;			//set radio to RX
	TRX_CE = 1;			//enable RF transceiver

	//limits number of loops instead of infinite loop in ReceivePacket()
	while(DR == 0 && i < 50){
		Delay400us(10);
		i++;
	}

	if(DR != 1)		//exit function if timed out
		return 0;

	//Reads packet from SPI
	RACSN = 0;
	SpiReadWrite(RRP);
	for(i=0; i<PACKET_SIZE; i++)
		b[i] = SpiReadWrite(0);
	RACSN = 1;
	TRX_CE = 0;		//disable RF transceiver

	//return 1 if it's heartbeat reply
	if( b[0]==HEARTBEAT_REPLY_HEADER ){
		*gid = b[1];
		*uid = b[2];
		return 1;
	}
	//return 0 if it's not heartbeat reply
	return 0;
}


//Called after RequestHeartbeat() to wait for heartbeat reply
// Note: This function is fucking useless. gid and uid should be passed directly
// to ListenHeartbeat() and checked there.
unsigned char WaitHeartbeat(unsigned char gid, unsigned char uid){
	unsigned char ret, a, b;

	ret = ListenHeartbeat(&a, &b);
	if( gid != a || uid != b )
		ret = 0;

	if(ret)
		return 1;
	else
		return 0;

}

// EXPERIMENTAL feature
// Called after RequestHeartbeatFromAll
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

//Request heartbeat from all slaves by looping
//Calls RequestHeartbeat() repeated for certain range of groupID & uniqueID
//Not used anymore because currently, the looping is done at master_unit
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
