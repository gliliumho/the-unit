#include <Nordic\reg9e5.h>
#include "util.h"
#include "uart.h"
#include "radio.h"
#include "eeprom.h"
//#include "nrf9e5.h"

#define SLAVE 0

#if SLAVE

void ChangeID(unsigned char, unsigned char);
void DisplayID(void);

void main(void){
	
	unsigned char uniqueID = 7;
	unsigned char groupID = 2;
	
	InitPin(3,1);
	InitPin(5,1);
	InitUART();
	InitRF();
	InitTimer0(0x02);
	PutString("\r\nPlease press SW2 or SW3.");
	
	//SW2 or SW3
	while(1){
		SlaveOp(groupID, uniqueID);
//		if(P03 == 0){
//			
//			PutString("\r\nPlease enter new groupID: ");
//			GetChar(&groupID);
//			
//			PutString("\r\nPlease enter new uniqueID: ");
//			GetChar(&uniqueID);
//			
//			ChangeID(groupID, uniqueID);
//			DisplayID();
//			
//		}else if(P05 == 0){
//				
//			//InitRF();
//			SlaveOp(groupID, uniqueID);
//		}
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

#else //MASTER

/*
**	Send traffic info & send heartbeat request
**
*/


void main(void){
	
	unsigned char uniqueID = 0;
	unsigned char groupID = 0;
	unsigned char c = 0;
	
	InitPin(3,1);
	InitPin(5,1);
	InitUART();
	//PutString("\r\nPlease press SW2(Send Traffic Info) or SW3 (Request Heartbeat)");
	InitRF();
	//InitEEPROM();
	InitTimer0(0x02);
	
	//SW2 or SW3
	while(1){
		if(c == 0){
			PutString("\r\nPlease press 1(Send Traffic Info) or 2(Request Heartbeat): ");
			c = 1;
		}
		
		if(P03 == 0){
			unsigned char i; 
			unsigned char temp[16];
			
			c = 0;
			PutString("\r\n::::Send Traffic Info:::::");
			temp[0] = 0x01;
			for(i=1;i<14;i++){
				PutString("\r\nTraffic info for group ");
				PutChar((i+0x30));
				PutString(": ");
				GetChar(&temp[i]);
				temp[i] -= 0x30;
			}
			temp[15] = 0x00;
			
			for(i=1;i<10;i++){
				TransmitPacket(&temp[0]);
			}
			
			PutString("\r\nTraffic info transmitted.");
			
		}else if(P05 == 0){
			c = 0;
			PutString("\r\nPlease enter new groupID: ");
			GetChar(&groupID);
			groupID -= 0x30;
			
			PutString("\r\nPlease enter new uniqueID: ");
			GetChar(&uniqueID);
			uniqueID -= 0x30;
			
			RequestHeartbeat(groupID, uniqueID);
			WaitHeartbeat(groupID, uniqueID);
			
		}
	}	
	
}




#endif