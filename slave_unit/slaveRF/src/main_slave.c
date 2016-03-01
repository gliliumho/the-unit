/*********************************************************************
**
**	This main file is used for unit testing of the functions
**
**
*********************************************************************/

/*
+ timeout to wait for slave heartbeat
+ enable/disable blinking
+ blinking interval
+ relay to higher groupID or not
+ sleep interval between SlaveOp_Buffer


*/



#include <Nordic/reg9e5.H>
#include "radio.h"
#include "uart.h"
#include "util.h"
#include "misc.h"
#include "eeprom.h"



void main(void){
	//Initialize global variables. FUCKING IMPORTANT. DON'T REMOVE
	packetID id_buffer[6] = {0};
	unsigned char packet_count = 0;
	unsigned char buffer_count = 0;
	unsigned char groupID = 4;
	unsigned char uniqueID = 5;

	// unsigned char i, b[PACKET_SIZE];

	//Init GPIO pins for LEDs
	InitPin(3,0);
	InitPin(4,0);
	InitPin(5,0);

	P03 = 0;
	P04 = 0;
	P05 = 0;

	//Init GPIO pin for button
	InitPin(7,1);
	//P07 = 1;
	if(P07 == 0){
		InitUART();
		InitEEPROM();

		//prompt for groupID
		PutString("Please enter new groupID: ");
		groupID = GetNumber();
		// groupID -= 0x30;
		PutString("\r\n");

		//prompt for uniqueID
		PutString("Please enter new uniqueID: ");
		uniqueID = GetNumber();
		// uniqueID -= 0x30;
		PutString("\r\n");

		//save entered groupID & uniqueID into EEPROM
		EEWrite(3995,groupID);
		EEWrite(3996,uniqueID);

		PutString("groupID: ");
		PrintChar(EERead(3995));
		PutChar(0x20);
		PutString("uniqueID: ");
		PrintChar(EERead(3996));
		PutString("\r\n");
	} else {
		InitUART();
		InitEEPROM();
		groupID = EERead(3995);
		uniqueID = EERead(3996);
	}

	InitRF();
	//Read saved groupID and uniqueID from EEPROM
	PutString("Hi! I'm slave..");
	PutString("groupID: ");
	PrintChar(groupID);
	PutChar(0x20);
	PutString("uniqueID: ");
	PrintChar(uniqueID);
	PutString("\r\n");

	while(1){
		SlaveOp_Buffer(groupID, uniqueID);
	}
}
