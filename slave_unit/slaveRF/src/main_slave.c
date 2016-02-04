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
	//Initialize global variables. FUCKING IMPORTANT. DON'T REMOVE
	packetID id_buffer[6] = {0};
	unsigned char packet_count = 0;
	unsigned char buffer_count = 0;
	unsigned char groupID = 4;
	unsigned char uniqueID = 5;

	// unsigned char i, b[PACKET_SIZE];

	//Init GPIO pins for LEDs
	InitPin(0,0);
	InitPin(4,0);
	InitPin(6,0);

	//Init GPIO pin for button
	InitPin(3,1);
	InitUART();

	//Read saved groupID and uniqueID from EEPROM
	InitEEPROM();
	groupID = EERead(3995);
	uniqueID = EERead(3996);

	//Init the RF transceiver
	InitRF();

	PutString("Hi! I'm slave from group ");
	PrintChar(groupID);
	PutString("\r\n");

	while(1){
		if(P03 == 0){				//if button is pressed
			InitEEPROM();

			//prompt for groupID
			PutString("Please enter new groupID: ");
			GetChar(&groupID);
			groupID -= 0x30;
			PutString("\r\n");

			//prompt for uniqueID
			PutString("Please enter new uniqueID: ");
			GetChar(&uniqueID);
			uniqueID -= 0x30;
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

			//change SPI and settings back to RF
			InitRF();
		} else {
			//do the intern things...coz interns are slaves. Geddit? Hehe.
			SlaveOp_Buffer(groupID, uniqueID);
		}
	}
}
