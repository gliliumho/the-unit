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
#include <string.h>


void main(void){

	packetID id_buffer[6] = {0};
 	unsigned char packet_count = 0;
 	unsigned char buffer_count = 0;
	unsigned char input[0x09];

	InitUART();
	InitRF();

	while(1){
		//memset(input, 0, sizeof(input));
		GetString(&input);

		if(input[0] == TRAFFIC_INFO_HEADER){
			//will receive all traffic info here

		} else if(input[0] == HEARTBEAT_REQUEST_HEADER){
			unsigned char i=0, ret=0;
			unsigned char groupID = input[1];
			unsigned char uniqueID = input[2];

			RequestHeartbeat(groupID, uniqueID);

			while(ret == 0 && i < 5){
				ret = WaitHeartbeat(groupID, uniqueID);
				i++;
			}

			input[3] = ret;
			input[4] = 0x00;

			PutString(&input);

			// if(!ret){
			// 	PutString("Timeout\r\n");
			// }

		}
		// else if(input[0] == HEARTBEAT_REQUEST_FROM_ALL_HEADER){
		// 	//removed since it's only EXPERIMENTAL
		//
		// } else if(input[0] == 0x04){
		// 	//RequestHeartbeatLoop();
		// }

	}
}
