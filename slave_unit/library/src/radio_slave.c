/*******************************************************************************
** slave.c
** This file contains all the functions for radio operations & configurations.
**
*********************************************************************************/

#include <Nordic\reg9e5.h>
// #include <stdlib.h>
#include "util.h"
#include "uart.h"
#include "radio.h"


void SlaveOp_Buffer(unsigned char gid, unsigned char uid){
	unsigned char b[PACKET_SIZE];

	if( SlaveReceive(b) ){
		if(b[0] == TRAFFIC_INFO_HEADER){
			PutString("Received Traffic Info!\r\n");
			CheckTraffic(b, gid);
			SlaveRelay(b);
		} else if(b[0] == HEARTBEAT_REQUEST_HEADER){
			// PutChar(b[1]+0x30);
			// PutChar(b[2]+0x30);
			PutString("Received HB Request!\r\n");
			if(b[1]==gid && b[2]==uid){
				SendHeartbeat(gid, uid);
			} else if(b[1] >= gid){
				SlaveRelay(b);
				PutString("Relaying HB Request!\r\n");
			}
		} else if(b[0] == HEARTBEAT_REPLY_HEADER && b[1] >= gid){
			SlaveRelay(b);
			PutString("Relaying HB!\r\n");
		} else if(b[0] == HEARTBEAT_REQUEST_FROM_ALL_HEADER){
			PutString("Everyone send heartbeat!!\r\n");
			// for(i=0; i<20; i++)
			SendHeartbeat(gid, uid);
			SlaveRelay(b);
		}
	}

}


//Basically a non-blocking ReceivePacket. Will timeout after not receiving for a while
unsigned char SlaveReceive(unsigned char b[PACKET_SIZE]){
	unsigned char i=0;

	TXEN = 0;				//RX mode
	TRX_CE = 1;				//enable radio

	while(DR == 0 && i < 50){
		Delay400us(10);
		i++;
	}

	if ( DR != 1 ){
		TRX_CE = 0;			//disable radio
		//PutString("timeout\r\n");
		return 0;
	} else {
		RACSN = 0;
		SpiReadWrite(RRP);
		for(i=0;i<PACKET_SIZE;i++)
			b[i] = SpiReadWrite(0);
		RACSN = 1;
		TRX_CE = 0;			//disable radio
		PutString("Valid packet\r\n");
		return 1;
	}
}

//Slave only
void CheckTraffic(unsigned char b[PACKET_SIZE], unsigned char gid){
	switch(b[gid+1]){
		case 1:
			P00 = 0;
			P04 = 1;
			P06 = 1;
			break;
		case 2:
			P00 = 1;
			P04 = 0;
			P06 = 1;
			break;
		case 3:
			P00 = 1;
			P04 = 1;
			P06 = 0;
			break;
		default:
			PutString("Unidentified groupID..\r\n");
			P00 = 0;
			P04 = 0;
			P06 = 0;
			break;
	}
}

//0x03 for first byte. Slave only
void SendHeartbeat(unsigned char gid, unsigned char uid){
	unsigned char b[PACKET_SIZE];
	unsigned char i;

	b[0] = HEARTBEAT_REPLY_HEADER;	//MACRO
	b[1] = gid;
	b[2] = uid;
	b[3] = packet_count;			//global variable
	for(i=4;i<PACKET_SIZE;i++)
		b[i] = 0x00;

	TXEN = 1;
	for(i=0; i<5; i++){
		TransmitPacket(b);
		Delay400us(i+1);
	}

	packet_count++;					//global variable
	PutString("Heartbeat Sent!\r\n");
}



/* void SlaveOp_Delay(unsigned char groupID, unsigned char uniqueID){
	unsigned char i;
	unsigned char (*b)[PACKET_SIZE] = malloc(sizeof *b);

	if( !SlaveReceive(b) ){
		if(*b[0]==0x01){
			//PutString("\r\nReceived traffic info!");
			CheckTraffic(b, groupID);


			////////////////////////////////////////////////////////
			TXEN = 1;
			//Can be swapped to SlaveRelay() once that is done
			for(i=0;i<10;i++)
				TransmitPacket(b);
			Delay5ms(10);
			////////////////////////////////////////////////////////

		}else if(*b[0]==0x02){
			if(*b[1]==groupID && *b[2]==uniqueID){
				for(i=0;i<50;i++)
					SendHeartbeat(groupID, uniqueID);
			} else if(*b[1] <= groupID){

				/////////////////////////////////////////////////////
				TXEN = 1;
				//Can be swapped to SlaveRelay() once that is done
				for(i=0;i<10;i++)
					TransmitPacket(b);
				//PutString("\r\nHB Request Relayed..");
				Delay5ms(3);
				/////////////////////////////////////////////////////

			}
		}else if(*b[0]==0x03 && *b[1] <= groupID){

			////////////////////////////////////////////////////////
			TXEN = 1;
			//Can be swapped to SlaveRelay() once that is done
			for(i=0;i<10;i++)
				TransmitPacket(b);
			PutString("\r\nHB Relayed..");
			Delay5ms(5);
			////////////////////////////////////////////////////////

		}
	}
	free(b);
} */



//will decide if retransmit the payload or not. TX. Slave only
void SlaveRelay(unsigned char b[PACKET_SIZE]){
	// unsigned char i;
	packetID temp_packet;

	if(b[0] == 0x01 || b[0] == 0x04){
		temp_packet.groupid = 0x00;
		temp_packet.uniqueid = 0x00;
		temp_packet.packet_no = b[1];
	} else if(b[0] == 0x02 || b[0] == 0x03){
		temp_packet.groupid = b[1];
		temp_packet.uniqueid = b[2];
		temp_packet.packet_no = b[3];
	}

	if( !CheckBuffer(temp_packet) ){
		PacketIdCpy(&id_buffer[buffer_count], temp_packet); //id_buffer is global

		TXEN =1;
		// for(i=0;i<2;i++)
		TransmitPacket(b);

		buffer_count++;					//global
		if(buffer_count > 5)			//global
			buffer_count = 0;			//global

	}

}

//returns 0 if not in buffer. returns 1 if found inside buffer.
unsigned char CheckBuffer(packetID a){
	unsigned char i;
	for(i=0; i<6; i++)
		if( PacketIdEqual(a, id_buffer[i]) )
			return 1;

	return 0;
}
