
#include <Nordic\reg9e5.h>
#include <string.h>
#include "nrf9e5.h"

/*old Transmitter and Receiver
void Transmitter(void){
	
	unsigned char payload[0x20];
	//strcpy(payload, "Hello World!");	//copy "Hello world!" to string
	GetString(&payload[0]);
	TXEN = 1;													//turn radio to TX mode
	
	PutString("\r\n Packet content: ");
	PutString(&payload[0]);						//
	TransmitPacket(&payload[0]);
	PutString("\r\n Packet transmitted.");
	Delay5ms(10);
	
	while(1){
		GetString(&payload[0]);
		
		PutString("\r\n Packet content: ");
		PutString(&payload[0]);
		TransmitPacket(&payload[0]);
		PutString("\r\n Packet transmitted.");
		Delay5ms(10);
		
	}
}

void Receiver(void){
	unsigned char payload[0x20];
	TXEN = 0;
	
	PutString(" Receiver started. \r\n");
	
	while(1){
		
		
//		//clear array
//		unsigned char i;
//		for(i=0;i<0x20;i++){
//			payload[i] = 0x20;
//		}
		
		
		ReceivePacket(&payload[0]);
		
		PutString(" Payload: ");
		PutString(&payload[0]);
		PutString("\r\n");
		
	}
	
}
*/

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
	unsigned char e;
	unsigned char s;
	
	
	
	InitPin(0,0);	//Initialize P00 for LED1
	InitPin(4,0);
	InitPin(6,0);
	InitPin(3,1);	//Initialize SW2 as input
	InitPin(5,1);	//Initialize SW3 as input
	InitPin(7,1);	//Initialize SW4 as input
	
	P00 = 1; 		//Initialize with LED1,3,4 turned ON
	P04 = 1;
	P06 = 1;
	
	InitUART();
	InitRF();
	
	while(1){
		PutString("\r\nEnter a number: ");
		GetNumber(&number[0],5);
		num = Ascii2Int(&number[0]);
		
		//num = 1000/num;
		//Int2Byte(num, &number[0]);
		
		//num = Byte2Int(&number[0]);
		
		PutString("\r\nThe number you entered is: ");
		PrintInt(num);
		//num++;
		//PutString("\r\nType something: ");
		//ConsoleComment();
	}

	
	
	//MasterTransmitter();
	
//	if(P03 == 0){		//SW2 for Transmitter
//		Slave(0);
//	} else if (P05 == 0){		//SW3 for Receiver
//		Slave(1);
//	} else if (P07 == 0){
//		Slave(2);
//	}
	
}

