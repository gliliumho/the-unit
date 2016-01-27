/*
*	testRFString.c
*	
*	Written to test sending multiple bytes via RF
*	
*	
*	Author: KY Ho 
*			*some of the codes are copied from Nordic's sample code
*	Dependencies : None
*	
*	
*/


#include <Nordic\reg9e5.h>
#include <string.h>


#define HFREQ 0		// 0=433MHz, 1=868/915MHz
#define POWER 3 	// 0=min power...3 = max power

/* Set pinNum as GPIO. direction=1 for input, direction=0 for output
** eg.InitPin(1,1) will set P01 as input GPIO. */
void InitPin(unsigned char pinNum, direction){
	switch(pinNum){
		case 0:
			P0_ALT &= 0xFE;
			if (direction == 1) P0_DIR |= 0x01;
			else P0_DIR &= 0xFE;
			break;
		
		case 1:
			P0_ALT &= 0xFD;
			if (direction == 1) P0_DIR |= 0x02;
			else P0_DIR &= 0xFD;
			break;
		
		case 2:
			P0_ALT &= 0xFB;
			if (direction == 1) P0_DIR |= 0x04;
			else P0_DIR &= 0xFB;
			break;
			
		case 3:
			P0_ALT &= 0xF7;
			if (direction == 1) P0_DIR |= 0x08;
			else P0_DIR &= 0xF7;
			break;
		
		case 4:
			P0_ALT &= 0xEF;
			if (direction == 1) P0_DIR |= 0x10;
			else P0_DIR &= 0xEF;
			break;
		
		case 5:
			P0_ALT &= 0xDF;
			if (direction == 1) P0_DIR |= 0x20;
			else P0_DIR &= 0xDF;
			break;
		
		case 6:
			P0_ALT &= 0xBF;
			if (direction == 1) P0_DIR |= 0x40;
			else P0_DIR &= 0xBF;
			break;
			
		case 7:
			P0_ALT &= 0x7F;
			if (direction == 1) P0_DIR |= 0x80;
			else P0_DIR &= 0x7F;
			break;	
			
		case 8:
			P0_ALT = 0x00;
			if (direction == 1) P0_DIR = 0xFF;
			else P0_DIR = 0x00;
			break;
			
		default:
			break;	
	}
}

/* ---SetTXPower() & SetFrequency()------------------------
---------------------------------------------
Currently not used and not working. 
---------------------------------------------
void SetTXPower(unsigned char powerlevel){
	switch(powerlevel){
		case 0:
			CC &= 0xF3FF;
			break;
		case 1:
			CC &= 0xF7FF;
			CC |= 0x0400;
			break;
		case 2:
			CC &= 0xFBFF;
			CC |= 0x0800;
			break;
		case 3:
			CC |= 0x0C00;
			break;
	}
}


------------------------------------------
Currently not used and not working. 
------------------------------------------
void SetFrequency(unsigned char freq){
	if(freq == 1)
		CC |= 0x0200;
	else
		CC &= 0xFDFF;
} 
*/


void Delay400us(volatile unsigned char n){
	unsigned char i;
	while(n--)
		for(i=0;i<35;i++)
			;
}

void Delay5ms(volatile unsigned char n){
	while(n--)
		Delay400us(50);
}

/*	SpiReadWrite() is used to read/write to register using SPI.
**	SpiReadWrite(REGISTER) will specify the register/byte you want to write to.
**	Then SpiReadWrite(VALUE) will write value to register. If you use it again, it'll write to next byte.
** 	c = SpiReadWrite() will read value from register.
*/
unsigned char SpiReadWrite(unsigned char b){
	EXIF &= ~0x20;				  // Clear SPI interrupt
	SPI_DATA = b;						// Move byte to send to SPI data register
	while((EXIF & 0x20) == 0x00)	// Wait until SPI has finished transmitting
		;
	return SPI_DATA;
}

void TransmitPacket(unsigned char *b){
	unsigned char i, width;
	
	RACSN = 0;
	SpiReadWrite(RRC | 0x04);	   //Read byte 4 of RF config(TX payload width)
	width = SpiReadWrite(0) & 0x3F;		//save the TX payload width
	RACSN = 1;
	
	/* To transmit more than one byte, just change the TX_PW in RF config.
	**	Then use SpiReadWrite() for each byte. (You can just loop it).	*/
	
	RACSN = 0;
	SpiReadWrite(WTP);					//Write to TX payload
	for(i=0;i<width;i++){
		if(*b!=0x00){							//if not EOS
			SpiReadWrite(*b);				//then write byte to SPI
			b++;										//move pointer to next byte
		}else{
			SpiReadWrite(0x00);			//write 0x00 for remaining of the payload
		}
	}
	
	RACSN = 1;
	TRX_CE = 1;				//turn ON radio
	Delay400us(20);		//delay to wait for transmission to be completed
	TRX_CE = 0; 			//turn OFF radio
}

void ReceivePacket(unsigned char *b){
	unsigned char i, width;
	
	TRX_CE = 1;							//turn ON radio

	while(DR == 0)					//DR=Data Ready
		;											//Busy waiting until VALID packet is received
	RACSN = 0;
	SpiReadWrite(RRC | 0x03);	   //Read byte 4 of RF config(TX payload width)
	width = SpiReadWrite(0) & 0x3F;		//save the TX payload width
	RACSN = 1;
	
	
	/* To receive more than one byte, just change the RX_PW in RF config.
	** Then use SpiReadWrite() for each byte. (You can just loop it).	*/
	RACSN = 0;
	SpiReadWrite(RRP);			//Read receive payload
	for(i=0;i<width;i++){
		*b = SpiReadWrite(0);		//populate *b with first byte of payload
		b++;									//move pointer to next byte
	}
	
	
//	*b = SpiReadWrite(0);
//	while(*b!=0x00){				//if end of message (NOTE: I use 0x00 to indicate EOS or end of packet.)
//		b++;									//move pointer to next byte
//		*b = SpiReadWrite(0);	
//	}
	
	RACSN = 1;
	TRX_CE = 0;							//turn OFF radio
}

void InitUART(void){
	
	unsigned char cklf;
	
	TH1 = 0xE6;					  // 9600@16MHz (when T1M=1 and SMOD=1)
	CKCON |= 0x10;				  // T1M=1 (/4 timer clock)
	PCON = 0x80;					// SMOD=1 (double baud rate)
	SCON = 0x52;					// Serial mode1, enable receiver
	TMOD = 0x20;					// Timer1 8bit auto reload 
	TR1 = 1;						// Start timer1
	
	/*	UART and RS-232 are using the same pin(P01 & P02) from SoC.
	**	For RS-232, the signal passes through another microcontroller
	**	that converts the UART TTL signal to serial RS-232 signals.	*/
	P0_ALT |= 0x06;	//select alternative function for P01 and P02
	P0_DIR &= 0x02; //P01(RXD) is input
	
	SPICLK = 0;			//Max SPI clock
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
}

void PutChar(unsigned char c){
	while(!TI) 	//TI=Transmit Interupt. TI=0 when UART TXD is busy
		;					
	TI = 0;
	SBUF = c;		//SBUF will be transmitted through UART
}

void PutString(unsigned char *s){
	while(*s != 0)
		PutChar(*s++);
}

void GetChar(unsigned char *c){
	while(!RI) 		//RI=Receive Interupt. RI=0 when UART RXD is busy
		;
	RI=0;
	*c = SBUF; 		//SBUF stores the byte received through UART
	PutChar(*c);	//for internal echo
}

void GetString(unsigned char *s){
	GetChar(s);
	while(*s!= 0x0D && *s!= 0x0A){	//GetChar as long as not ENTER.
		s++;													//move pointer to next byte
		GetChar(s);
	}
	*s = 0x00; 											//0x00 to indicate end of string(EOS)
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

void InitRF(void){
	
	unsigned char tmp;
	
	SPICLK = 0;						//Max SPI clock
	SPI_CTRL = 0x02;
	
	//Configure RF
	RACSN = 0;
	SpiReadWrite(WRC | 0x03);	   // Write to RF config address 3 (RX payload)
	SpiReadWrite(0x20);			 // 20 byte RX payload width
	SpiReadWrite(0x20);			 // 20 byte TX payload width
	RACSN = 1;

	RACSN = 0;
	SpiReadWrite(RRC | 0x01);	   // Read RF config address 1
	tmp = SpiReadWrite(0) & 0xf1;   // Clear the power and frequency setting bits
	RACSN = 1;

	RACSN = 0;
	SpiReadWrite(WRC | 0x01);	  // Write RF config address 1
	// Change power defined by POWER and to 433 or 868/915MHz defined by HFREQ above:
	SpiReadWrite(tmp | (POWER <<2) | (HFREQ << 1));
	RACSN = 1;
	
}


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
		
		//clear array
		unsigned char i;
		for(i=0;i<0x20;i++){
			payload[i] = 0x20;
		}
		
		ReceivePacket(&payload[0]);
		
		PutString(" Payload: ");
		PutString(&payload[0]);
		PutString("\r\n");
		
	}
	
}

void main(){
	
	InitPin(0,0);	//Initialize P00 for LED1
	InitPin(3,1);	//Initialize SW2 as input
	InitPin(5,1);	//Initialize SW3 as output
	
	P00 = 1; 		//Initialize with LED1 turned OFF
	
	InitUART();
	InitRF();
	
	if(P03 == 0){		//SW2 for Transmitter
		Transmitter();
	} else if (P05 == 0){		//SW3 for Receiver
		Receiver();
	}
	
}

