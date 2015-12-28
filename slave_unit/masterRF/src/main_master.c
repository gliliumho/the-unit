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


void GetTrafficInfo( unsigned char b[PACKET_SIZE] );
void GetHeartbeat( unsigned char *groupID, unsigned char *uniqueID );


void main(void){

	packetID id_buffer[6] = {0};
 	unsigned char packet_count = 0;
 	unsigned char buffer_count = 0;
	unsigned char input;

	InitUART();
	InitRF();
	PutString("\r\nMaster Started..");

	while(1){
		input = 0x00;

		PutString("\r\n");
		PutString("1. Traffic Info \r\n");
		PutString("2. Heartbeat \r\n");
		PutString("3. Heartbeat from all(broadcast) \r\n");
		PutString("4. Heartbeat from all(loop)\r\n");
		PutString("Select: ");
		GetChar(&input);
		PutString("\r\n");

		if(input == 0x31){
			unsigned char b[PACKET_SIZE];
			PutString("::::Send Traffic Info:::::\r\n");

			GetTrafficInfo(b);
			SendTraffic(b);

		} else if(input == 0x32){
			unsigned char i=0, ret=0;
			unsigned char groupID = 0;
			unsigned char uniqueID = 0;

			GetHeartbeat(&groupID, &uniqueID);
			RequestHeartbeat(groupID, uniqueID);

			while(ret == 0 && i < 5){
				ret = WaitHeartbeat(groupID, uniqueID);
				i++;
			}

			if(!ret){
				PutString("Timeout\r\n");
			}

		} else if(input == 0x33){
			unsigned char b[6][2]={0};
			unsigned char i, n = 0;

			RequestHeartbeatFromAll();
			n = WaitMultiHeartbeat(b, 6);

			if(n == 0){
				PutString("No heartbeat received..\r\n");
			} else {
				PutString("Received heartbeat from..\r\n");
				PutString("Slave [gid] [uid]: \r\n");
				for(i=0; i<n; i++){
					PutChar(b[i][0]+0x30);
					PutChar(0x20);
					PutChar(b[i][1]+0x30);
					PutString("\r\n");
				}
			}

		}
		else if(input == 0x34){
			RequestHeartbeatLoop();
		}

	}
}

void GetTrafficInfo( unsigned char b[PACKET_SIZE] ){
	unsigned char i;
	unsigned char groupID = 0;
	unsigned char uniqueID = 0;

	for(i=0; i<PACKET_SIZE; i++)
		b[i] = 0x04;

	for(i=3; i<=5; i++){
		PutString("Traffic info for group ");
		PutChar( 0x2F+i );
		PutString(": ");
		GetChar(&b[i]);
		PutString("\r\n");
		b[i] -= 0x30;
	}

	// PutString("\r\nTraffic info is :");
	// for(i=0; i<PACKET_SIZE; i++)
	// 	PutChar( b[i]+0x30 );
	PutString("\r\n");

}


void GetHeartbeat( unsigned char *groupID, unsigned char *uniqueID ){
	PutString("Please enter groupID: ");
	GetChar(groupID);
	*groupID -= 0x30;
	PutString("\r\n");

	PutString("Please enter uniqueID: ");
	GetChar(uniqueID);
	*uniqueID -= 0x30;
	PutString("\r\n");

}

// void RequestHeartbeatLoop(void){
// 	unsigned char i, ret;
// 	unsigned char groupID = 0;
// 	unsigned char uniqueID = 0;
//
// 	PutString("Scanning for slaves alive..\r\n");
// 	PutString("Found the following slaves: \r\n");
//
// 	for(groupID=1; groupID<10; groupID++){
// 		for(uniqueID=1; uniqueID<10; uniqueID++){
// 			RequestHeartbeat(groupID,uniqueID);
// 			i = 0;
// 			ret = 0;
// 			while(ret == 0 && i < 5){
// 				ret = WaitHeartbeat(groupID, uniqueID);
// 				i++;
// 			}
//
// 			PrintChar(groupID);
// 			PutChar(0x20);
// 			PrintChar(uniqueID);
// 			PutChar(0x20);
//
// 			if(ret){
// 				PutString("Alive!\r\n");
// 			} else
// 				PutString("Timeout\r\n");
//
// 			Delay400us(10);
// 			//PutString("\r\n");
//
// 		}
// 	}
// }
