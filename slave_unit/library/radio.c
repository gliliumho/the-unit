/*******************************************************************************
** radio.c
** This file contains all the functions for radio operations & configurations.
**
*********************************************************************************/

#include <Nordic\reg9e5.h>
#include "util.h"
#include "uart.h"
#include "radio.h"

void InitRF(void){
	unsigned char tmp, cklf;
	
	SPICLK = 0;						//Max SPI clock
	SPI_CTRL = 0x02;
	
	// Switch to 16MHz clock:
	RACSN = 0;
	SpiReadWrite(RRC | 0x09);
	cklf = SpiReadWrite(0) | 0x04;	//XO_DIRECT = 1, follow XO_Frequency
	RACSN = 1;
	RACSN = 0;
	SpiReadWrite(WRC | 0x09);
	SpiReadWrite(cklf);
	RACSN = 1;
	
	//Configure RF
	RACSN = 0;
	SpiReadWrite(WRC | 0x03);	   	// Write to RF config address 3 (RX payload)
	SpiReadWrite(0x10);			 	// 16 byte RX payload width
	SpiReadWrite(0x10);			 	// 16 byte TX payload width
	RACSN = 1;

	RACSN = 0;
	SpiReadWrite(RRC | 0x01);	   	// Read RF config address 1
	tmp = SpiReadWrite(0) & 0xf1;   // Clear the power and frequency setting bits
	RACSN = 1;

	RACSN = 0;
	SpiReadWrite(WRC | 0x01);	  // Write RF config address 1
	// Change power defined by POWER and to 433 or 868/915MHz defined by HFREQ above:
	SpiReadWrite(tmp | (POWER <<2) | (HFREQ << 1));
	RACSN = 1;	
}

void SetReceiveMode(void){
	TXEN = 0x00;
}

void SetTransmitMode(void){
	TXEN = 0x01;
}

void TransmitPacket(unsigned char *b){
	unsigned char i, width;
	
	RACSN = 0;
	SpiReadWrite(RRC | 0x04);	   		//Read byte 4 of RF config(TX payload width)
	width = SpiReadWrite(0) & 0x3F;		//save the TX payload width
	RACSN = 1;
	
	/* To transmit more than one byte, just change the TX_PW in RF config.
	**	Then use SpiReadWrite() for each byte. (You can just loop it).	*/
	
	RACSN = 0;
	SpiReadWrite(WTP);					//Write to TX payload
	for(i=0;i<width;i++){
		if(*b!=0x00){					//if not EOS
			SpiReadWrite(*b);			//then write byte to SPI
			b++;						//move pointer to next byte
		}else{
			SpiReadWrite(0x00);			//write 0x00 for remaining of the payload
		}
	}
	RACSN = 1;
	
	TRX_CE = 1;				//turn ON radio
	Delay400us(20);			//delay to wait for transmission to be completed
	TRX_CE = 0; 			//turn OFF radio
}

void ReceivePacket(unsigned char *b){
	unsigned char i, width;
	
	TRX_CE = 1;							//turn ON radio

	while(DR == 0)						//DR=Data Ready
		;								//Busy waiting until VALID packet is received
	RACSN = 0;
	SpiReadWrite(RRC | 0x03);	   		//Read byte 4 of RF config(RX payload width)
	width = SpiReadWrite(0) & 0x3F;		//save the RX payload width
	RACSN = 1;
	
	
	/* To receive more than one byte, just change the RX_PW in RF config.
	** Then use SpiReadWrite() for each byte. (You can just loop it).	*/
	RACSN = 0;
	SpiReadWrite(RRP);				//Read receive payload
	for(i=0;i<width;i++){
		*b = SpiReadWrite(0);		//populate *b with first byte of payload
		b++;						//move pointer to next byte
	}
	
	
	RACSN = 1;
	TRX_CE = 0;						//turn OFF radio
}

void SetAutoRetransmit(unsigned char setting){
	unsigned char tmp;
	
	RACSN = 0;
	SpiReadWrite(RRC | 0x01); 			//Read RF config address byte #1
	tmp = SpiReadWrite(0) & 0xDF;		//store current RF config and clear off AUTORETRAN bit
	RACSN = 1;
	
	RACSN = 0;
	SpiReadWrite(WRC | 0x01);			//Write RF config at addr byte #1
	SpiReadWrite(tmp | (setting <<5));	//change the AUTORETRAN setting
	RACSN = 1;
}

void SetPayloadWidth(unsigned char w){
	//Configure RF
	RACSN = 0;
	SpiReadWrite(WRC | 0x03);	 // Write to RF config address 3 (RX payload)
	SpiReadWrite(w);			 // 3 byte RX payload width
	SpiReadWrite(w);			 // 3 byte TX payload width
	RACSN = 1;
}

void SetTXPower(unsigned char powerlevel){
	unsigned char tmp;
	
	RACSN = 0;
	SpiReadWrite(RRC | 0x01); 			//Read RF config address byte #1
	tmp = SpiReadWrite(0) & 0xF3;		//store current RF config and clear off PA_PWR bit
	RACSN = 1;
	
	RACSN = 0;
	SpiReadWrite(WRC | 0x01);				//Write RF config at addr byte #1
	SpiReadWrite(tmp | (powerlevel <<2));	//change the PA_PWR setting
	RACSN = 1;
}

void SetFrequency(unsigned char freq){
	unsigned char tmp;
	
	RACSN = 0;
	SpiReadWrite(RRC | 0x01); 			//Read RF config address byte #1
	tmp = SpiReadWrite(0) & 0xFD;		//store current RF config and clear off HFREQ_PLL bit
	RACSN = 1;
	
	RACSN = 0;
	SpiReadWrite(WRC | 0x01);			//Write RF config at addr byte #1
	SpiReadWrite(tmp | (freq <<1));		//change the HFREQ_PLL setting
	RACSN = 1;
} 

void SetLowRXPower(unsigned char flag){
	unsigned char tmp;
	
	RACSN = 0;
	SpiReadWrite(RRC | 0x01); 			//Read RF config address byte #1
	tmp = SpiReadWrite(0) & 0xEF;		//store current RF config and clear off RX_RED_PWR bit
	RACSN = 1;
	
	RACSN = 0;
	SpiReadWrite(WRC | 0x01);			//Write RF config at addr byte #1
	SpiReadWrite(tmp | (flag <<4));		//change the RX_RED_PWR setting
	RACSN = 1;
}

//*addr is the pointer to RX address, addr_size must be 1-4
void SetRXAddress(unsigned char *addr, unsigned char addr_size){
	unsigned char tmp, i;
	
	RACSN = 0;
	SpiReadWrite(RRC | 0x02);			//Read RF config at addr byte #2
	tmp = SpiReadWrite(0) & 0xF8;		//store current address size and clear RXAddress size
	RACSN = 1;
	
	RACSN = 0;
	SpiReadWrite(WRC | 0x02);			//Write RF config at addr byte #2
	SpiReadWrite(tmp | (addr_size));	//change the RX address size
	RACSN = 1;
	
	
	RACSN = 0;
	SpiReadWrite(WRC | 0x05);			//Write RF config at addr byte #5
	for(i=0;i<addr_size;i++){
		SpiReadWrite(*addr);			//change the TX address
		addr++;
	}
	RACSN = 1;
}

//*addr is the pointer to TX address, addr_size must be 1-4
void SetTXAddress(unsigned char *addr, unsigned char addr_size){
	unsigned char tmp, i;
	
	RACSN = 0;
	SpiReadWrite(RRC | 0x02);			//Read RF config at addr byte #2
	tmp = SpiReadWrite(0) & 0xF8;		//store current address size and clear TXAddress size
	RACSN = 1;
	
	RACSN = 0;
	SpiReadWrite(WRC | 0x02);			//Write RF config at addr byte #2
	SpiReadWrite(tmp | (addr_size<<4));	//change the TX address size
	RACSN = 1;
	
	RACSN = 0;
	SpiReadWrite(WTA);			//Write TX Address
	for(i=0;i<addr_size;i++){
		SpiReadWrite(*addr);	//change the TX address
		addr++;
	}
	RACSN = 1;
}

void RadioPowerUpDown(unsigned char val){
	if(val == 0)
		TRX_CE = 0;
	else if(val == 1)
		TRX_CE = 1;
}

void SetCRCMode(unsigned char flag){
	unsigned char tmp;
	
	RACSN = 0;
	SpiReadWrite(RRC | 0x09); 			//Read RF config address byte #9
	tmp = SpiReadWrite(0) & 0x7F;		//store current RF config and clear off CRC_MODE bit
	RACSN = 1;
	
	RACSN = 0;
	SpiReadWrite(WRC | 0x09);			//Write RF config at addr byte #9
	SpiReadWrite(tmp | (flag <<7));		//change the CRC_MODE setting
	RACSN = 1;
}

void CRCEnableDisable(unsigned char flag){
	unsigned char tmp;
	
	RACSN = 0;
	SpiReadWrite(RRC | 0x09); 			//Read RF config address byte #9
	tmp = SpiReadWrite(0) & 0x7F;		//store current RF config and clear off CRC_EN bit
	RACSN = 1;
	
	RACSN = 0;
	SpiReadWrite(WRC | 0x09);			//Write RF config at addr byte #9
	SpiReadWrite(tmp | (flag <<6));		//change the CRC_EN setting
	RACSN = 1;
}

//0x01 for first byte. Master only
void SendTraffic(unsigned char *info){
	unsigned char i, width;
	
	RACSN = 0;
	SpiReadWrite(RRC | 0x04);	   		//Read byte 4 of RF config(TX payload width)
	width = SpiReadWrite(0) & 0x3F;		//save the TX payload width
	RACSN = 1;
	
	RACSN = 0;
	SpiReadWrite(WTP);					//Write to TX payload
	SpiReadWrite(0x01);					//Write 1 to first byte to indicate it's traffic info
	for(i=1;i<width;i++){
		if(*info!=0x00){				//if not EOS
			SpiReadWrite(*info);		//then write byte to SPI
			info++;						//move pointer to next byte
		}else{
			SpiReadWrite(0x00);			//write 0x00 for remaining of the payload
		}
	}
	RACSN = 1;
	
	TRX_CE = 1;			//turn ON radio
	Delay400us(20);		//delay to wait for transmission to be completed
	TRX_CE = 0; 		//turn OFF radio
}

unsigned char CheckTraffic(unsigned char *info, unsigned char groupID){
	info += (groupID);
	return *info;
}

//0x02 for first byte. Master only
void RequestHeartbeat(unsigned char groupID, unsigned char uniqueID){
	unsigned char i, width;
	TXEN = 1;
			
	RACSN = 0;
	SpiReadWrite(RRC | 0x04);	  	 //Read byte 4 of RF config(TX payload width)
	width = SpiReadWrite(0) & 0x3F;	//save the TX payload width
	RACSN = 1;
	
	RACSN = 0;
	SpiReadWrite(WTP);				//Write to TX payload
	SpiReadWrite(0x02);				//Write 1 to first byte to indicate it's traffic info
	SpiReadWrite(groupID);
	SpiReadWrite(uniqueID);
	for(i=3;i<width;i++){
		SpiReadWrite(0x00);	
	}
	RACSN = 1;
	
	TRX_CE = 1;			//turn ON radio
	Delay400us(20);		//delay to wait for transmission to be completed
	TRX_CE = 0; 		//turn OFF radio
}

//0x03 for first byte. Slave only
void SendHeartbeat(unsigned char groupID, unsigned char uniqueID){
	unsigned char i, width;
	TXEN = 1;
			
	RACSN = 0;
	SpiReadWrite(RRC | 0x04);	   	//Read byte 4 of RF config(TX payload width)
	width = SpiReadWrite(0) & 0x3F;	//save the TX payload width
	RACSN = 1;
	
	RACSN = 0;
	SpiReadWrite(WTP);				//Write to TX payload
	SpiReadWrite(0x03);				//Write 1 to first byte to indicate it's traffic info
	SpiReadWrite(groupID);
	SpiReadWrite(uniqueID);	
	for(i=3;i<width;i++){
		SpiReadWrite(0x00);
	}
	RACSN = 1;
	
	TRX_CE = 1;			//turn ON radio
	Delay400us(20);		//delay to wait for transmission to be completed
	TRX_CE = 0; 		//turn OFF radio
	
	PutString("\r\nHeartbeat Sent!");
}

//this function is currently a hugeeeee mess. Please don't use it. 
unsigned char WaitHeartbeat(unsigned char groupID, unsigned char uniqueID){
	unsigned char buf[16];
	unsigned char i, width;
	
	ReloadTimer0(0x00, 0x00);
	TXEN = 0;
			
	TRX_CE = 1;
	
	// TF0 = 0;
	// TR0 = 0;
	
	while(DR == 0)
		PutString("\r\nwaiting..");
	// TR0 = 0;
	
	if(DR != 1){
		PutString("\n\rHeartbeat request timeout.");
		return 0;
	}
	
	RACSN = 0;
	SpiReadWrite(RRC | 0x03);
	width = SpiReadWrite(0) & 0x3F;		//save the RX payload width
	RACSN = 1;
	
	RACSN = 0;
	SpiReadWrite(RRP);
	for(i=0;i<width;i++){
		buf[i] = SpiReadWrite(0);
	}
	
	RACSN = 1;
	TRX_CE = 0;
	
	
	if(buf[0]==0x03 && buf[1]==groupID && buf[2]==uniqueID){
		PutString("\r\nHeartbeat from slave received!!!!");
		return 1;
	}else{
		PutString("\r\nNot heartbeat from requested slave.");
		return 0;
	}
	
}

//Basically a non-blocking ReceivePacket. Will timeout after not receiving for a while
unsigned char SlaveReceive(unsigned char *b){
	unsigned char i, width;
	
	ReloadTimer0(0x00, 0x00);
	TXEN = 0;
	TRX_CE = 1;
	// TF0 = 0;
	// TR0 = 0;
	
	while(DR == 0)
		;
	// TR0 = 0;
	
	if (DR != 1){
		TRX_CE = 0;
		PutString("\r\nTimeout..");
		return 1;
		
	}else {
		RACSN = 0;
		SpiReadWrite(RRC | 0x03);
		width = SpiReadWrite(0) & 0x3F;		//save the RX payload width
		RACSN = 1;
		
		RACSN = 0;
		SpiReadWrite(RRP);
		for(i=0;i<width;i++){
			*b = SpiReadWrite(0);
			b++;
		}
		
		RACSN = 1;
		TRX_CE = 0;
		
		return 0;
	}
}

void SlaveOp(unsigned char groupID, unsigned char uniqueID){
	unsigned char i, temp;
	unsigned char b[16];
	
	if(!SlaveReceive(&b[0])){
		if(b[0]==0x01){
			temp = CheckTraffic(&b[0], groupID);
			switch(temp){
				case 0x01:
					P00 = 0;
					P04 = 1;
					P06 = 1;
					break;
				case 0x02:
					P00 = 1;
					P04 = 0;
					P06 = 1;
					break;
				case 0x03:
					P00 = 1;
					P04 = 1;
					P06 = 0;
					break;
				default:
					break;
			}
			
			TXEN = 1;
			for(i=0;i<10;i++)
				TransmitPacket(&b[0]);
			Delay5ms(5);
			
		}else if(b[0]==0x02){
			if(b[1]==groupID && b[2]==uniqueID){
				SendHeartbeat(groupID, uniqueID);
			} else {
				TXEN = 1;
				for(i=0;i<10;i++)
					TransmitPacket(&b[0]);
				Delay5ms(5);
			}
		}else if(b[0]==0x03){
			TXEN = 1;
			for(i=0;i<10;i++)
				TransmitPacket(&b[0]);
			Delay5ms(5);
		}
		
	}
}

//will decide if retransmit the payload or not. TX. Slave only
//NOT COMPLETE
void SlaveRelay(unsigned char *b){
	unsigned char i, width;
	
	//insert code to check buffer if packet was already received or not.
	
	SetAutoRetransmit(1);
	
	RACSN = 0;
	SpiReadWrite(RRC | 0x04);
	width = SpiReadWrite(0) & 0x3F;
	RACSN = 1;
	
	RACSN = 0;
	SpiReadWrite(WTP);
	for(i=0;i<width;i++){
		if(*b!=0x00){
			SpiReadWrite(*b);
			b++;
		}else{
			SpiReadWrite(0x00);
		}
	}
	RACSN = 1;
	
	TRX_CE = 1;
	Delay400us(200);
	TRX_CE = 0;
}