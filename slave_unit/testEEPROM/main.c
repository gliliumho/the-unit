
#include <Nordic\reg9e5.h>
#include "util.h"
#include "uart.h"
#include "radio.h"
#include "eeprom.h"
//#include "nrf9e5.h"

void ChangeID(unsigned char, unsigned char);
void DisplayID(void);

void main(void){
	
	unsigned char uniqueID;
	unsigned char groupID;
	
	InitPin(3,1);
	InitPin(5,1);
	InitUART();
	InitRF();
	InitEEPROM();
	PutString("\r\nPlease press SW2 or SW3.");
	
	//SW2 or SW3
	while(1){
		if(P03 == 0){
			
			PutString("\r\nPlease enter new groupID: ");
			GetChar(&groupID);
			//groupID -= 0x30;
			
			PutString("\r\nPlease enter new uniqueID: ");
			GetChar(&uniqueID);
			//uniqueID -= 0x30;
			
			ChangeID(groupID, uniqueID);
			DisplayID();
			
		}else if(P05 == 0){
			DisplayID();
			
		}
	}	
}



void ChangeID(unsigned char groupID, unsigned char uniqueID){
	EEWrite(3900, groupID);
	EEWrite(3901, uniqueID);
}

void DisplayID(void){
	unsigned char group, unique;
	
	group = EERead(3900);
	unique = EERead(3901);
	
	PutString("\r\nCurrent groupID & uniqueID is: ");
	PutChar(group);
	PutChar(0x20);
	PutChar(unique);
	
}
