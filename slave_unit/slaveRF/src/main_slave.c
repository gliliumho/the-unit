/*********************************************************************
**
**	This main file is used for unit testing of the functions
**
**
*********************************************************************/


#include <Nordic/reg9e5.H>
#include "radio.h"
#include "uart.h"
#include "util.h"
#include "misc.h"
#include "eeprom.h"



// unsigned char malloc_pool[0x10];

void main(void){
	packetID id_buffer[6] = {0};
	unsigned char packet_count = 0;
	unsigned char buffer_count = 0;
	unsigned char groupID = 4;
	unsigned char uniqueID = 5;

	// unsigned char i, b[PACKET_SIZE];

	InitPin(0,0);
	InitPin(4,0);
	InitPin(6,0);

	InitPin(3,1);
	InitUART();

	InitEEPROM();
	groupID = EERead(3995);
	uniqueID = EERead(3996);

	InitRF();

	PutString("Hi! I'm slave from group ");
	PrintChar(groupID);
	PutString("\r\n");


	// while(1){
	// 	ReceivePacket(b);
	// 	for(i=0; i<PACKET_SIZE;i++){
	// 		PrintChar(b[i]);
	// 		PutChar(0x20);
	// 	}
	// 	PutString("\r\n");
	// }

	while(1){
		if(P03 == 0){
			InitEEPROM();

			PutString("Please enter new groupID: ");
			GetChar(&groupID);
			groupID -= 0x30;
			PutString("\r\n");

			PutString("Please enter new uniqueID: ");
			GetChar(&uniqueID);
			uniqueID -= 0x30;
			PutString("\r\n");

			EEWrite(3995,groupID);
			EEWrite(3996,uniqueID);

			PutString("groupID: ");
			// PrintChar(groupID);
			PrintChar(EERead(3995));
			PutChar(0x20);
			PutString("uniqueID: ");
			// PrintChar(uniqueID);
			PrintChar(EERead(3996));
			PutString("\r\n");

			InitRF();
		} else {

			// PutString("groupID: ");
			// PrintChar(groupID);
			// PutChar(0x20);
			// PutString("uniqueID: ");
			// PrintChar(uniqueID);
			// PutString("\t");
			//
			// PutString("SlaveOp_Buffer\r\n");
			SlaveOp_Buffer(groupID, uniqueID);
		}
	}

}
