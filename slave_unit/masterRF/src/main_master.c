/*********************************************************************
**
**	This main file is for master
**
**
*********************************************************************/


#include <Nordic/reg9e5.H>
#include "radio.h"
#include "uart.h"
#include "util.h"
#include "misc.h"


void main(void){

	packetID id_buffer[6] = {0};
 	unsigned char packet_count = 0;
 	unsigned char buffer_count = 0;
	unsigned char input[0x10];

	InitUART();
	InitRF();
	// PutString("\r\nMaster Started..");

	while(1){
		input = {0};
		GetString(input)

		if(input[0] == TRAFFIC_INFO_HEADER){
			//will receive all traffic info here

		} else if(input[0] == HEARTBEAT_REQUEST_HEADER){
			unsigned char i=0, ret=0;
			unsigned char groupID = 0;
			unsigned char uniqueID = 0;

			RequestHeartbeat(groupID, uniqueID);

			while(ret == 0 && i < 5){
				ret = WaitHeartbeat(groupID, uniqueID);
				i++;
			}

			if(!ret){
				PutString("Timeout\r\n");
			}

		} else if(input[0] == HEARTBEAT_REQUEST_FROM_ALL_HEADER){
			//removed since it's only EXPERIMENTAL

		} else if(input[0] == 0x04){
			//RequestHeartbeatLoop();
		}

	}
}
