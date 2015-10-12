#include <Nordic\reg9e5.h>
#include "util.h"
#include "uart.h"
#include "radio.h"
#include "eeprom.h"
//#include "nrf9e5.h"

#define SLAVE 0

#if SLAVE

//void ChangeID(unsigned char, unsigned char);
//void DisplayID(void);



void main(void){
	
	unsigned char uniqueID = 6;
	unsigned char groupID = 3;
	
	InitPin(3,1);
	InitPin(5,1);
	InitUART();
	InitRF();
	InitTimer0(0x02);
	PutString("\r\nPlease press SW2 or SW3.");
	
	//SW2 or SW3
	while(1){
//		SlaveOp(groupID, uniqueID);
		if(P03 == 0){
			
			PutString("\r\nPlease enter new groupID: ");
			GetChar(&groupID);
			
			PutString("\r\nPlease enter new uniqueID: ");
			GetChar(&uniqueID);
			
			ChangeID(groupID, uniqueID);
			DisplayID();
			
		}else {
				
			//InitRF();
			SlaveOp(groupID, uniqueID);
		}
	}	
	
}

//void ChangeID(unsigned char groupID, unsigned char uniqueID){
//	EEWrite(3950, groupID);
//	EEWrite(3951, uniqueID);
//}

//void DisplayID(void){
//	unsigned char group, unique;
//	
//	group = EERead(3950);
//	unique = EERead(3951);
//	
//	PutString("\r\nCurrent groupID & uniqueID is: ");
//	PutChar(group);
//	PutChar(0x20);
//	PutChar(unique);
//	
//}

#else //MASTER

/*
**	Send traffic info & send heartbeat request
**
*/


void main(void){
	
	unsigned char uniqueID = 0;
	unsigned char groupID = 0;
	unsigned char c = 0;
	unsigned char i,j;
	unsigned char ret = 0;
	
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
			PutString("\r\nPlease enter groupID: ");
			GetChar(&groupID);
			groupID -= 0x30;
			
			PutString("\r\nPlease enter uniqueID: ");
			GetChar(&uniqueID);
			uniqueID -= 0x30;
			
			
			i = 0;
			j = 0;
			ret = 0;
			while(ret == 0 && i <= 20){
				RequestHeartbeat(groupID, uniqueID);
				while(ret == 0 && j <= 20){
					ret = WaitHeartbeat(groupID, uniqueID);
					j++;
				}
				i++;
			}
		
		}
	}	
	
}




#endif