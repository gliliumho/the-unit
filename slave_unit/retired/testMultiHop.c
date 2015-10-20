/*
*	testMultiHop.c
*	
*	This is the test code that was used as PoC for the multihop architecture.
*	
*	
*	Author: KY Ho 
*			*some of the codes are copied from Nordic's sample code
*	Dependencies : nrf9e5.h
*	
*	
*/


#include <Nordic\reg9e5.h>
#include <string.h>
#include "nrf9e5.h"


void MasterTransmitter(void){
	unsigned char payload[4];
	TXEN = 1;
	
	while(1){
		unsigned char i;
		
		PutString("\r\n [Slave1 LED][Slave2 LED](no space): ");
		GetString(&payload[0]);
		
		PutString("\r\n Transmitting packet: ");
		PutString(&payload[0]);
		for(i=0;i<50;i++){
			TransmitPacket(&payload[0]);
		}
	}
}


void Slave(unsigned char groupID){
	unsigned char payload[3];
	unsigned char id = groupID;
	unsigned char slaveID = 0;
	
	while(1){
		unsigned char i;
		
		TXEN = 0;
		ReceivePacket(&payload[0]);
		
		if(payload[id] == 0x31){
			P00 = 0;
			P04 = 1;
			P06 = 1;
		} else if(payload[id] == 0x32){
			P00 = 1;
			P04 = 0;
			P06 = 1;
		} else if(payload[id] == 0x33){
			P00 = 1;
			P04 = 1;
			P06 = 0;
		}
		
		TXEN = 1;
		for(i=0;i<10;i++){
			TransmitPacket(&payload[0]);
		}
		Delay5ms(50);
	}
}

void main(){
	

	unsigned char number[6];
	unsigned int num;

	
	
	
	InitPin(0,0);	//Initialize P00 for LED1
	InitPin(4,0);
	InitPin(6,0);
	InitPin(3,1);	//Initialize SW2 as input
	InitPin(5,1);	//Initialize SW3 as input
	InitPin(7,1);	//Initialize SW4 as input
	
	P00 = 0; 		//Initialize with LED1,3,4 turned ON
	P04 = 0;
	P06 = 0;
	
	InitUART();
	InitRF();
	
	
//	MasterTransmitter();
	
	if(P03 == 0){		//SW2 for Transmitter
		Slave(0);
	} else if (P05 == 0){		//SW3 for Receiver
		Slave(1);
	} else if (P07 == 0){
		Slave(2);
	}
	
}

