
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
		b++;						//move pointer to next byte
	}
	
	
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

void PrintInt(unsigned int n){
	unsigned char bit4, bit3, bit2, bit1, bit0;
	
	if(n >= 10000){
		bit4 = n/10000;
		n -= bit4*10000;
	} else{
		bit4 = 0;
	}
	
	if(n >= 1000){
		bit3 = n/1000;
		n -= bit3*1000;
	} else{
		bit3 = 0;
	}
	
	if(n >= 100){
		bit2 = n/100;
		n -= bit2*100;
	} else{
		bit2 = 0;
	}
	
	if(n >= 10){
		bit1 = n/10;
		n -= bit1*10;
	} else{
		bit1 = 0;
	}
	
	bit0 = n;
	
	
	bit4 += 0x30;
	bit3 += 0x30;
	bit2 += 0x30;
	bit1 += 0x30;
	bit0 += 0x30;
	
	if(bit4 >= 0x31){
		PutChar(bit4);
		PutChar(bit3);
		PutChar(bit2);
		PutChar(bit1);
	}else if(bit3 >= 0x31){
		PutChar(bit3);
		PutChar(bit2);
		PutChar(bit1);
	}else if(bit2 >= 0x31){
		PutChar(bit2);
		PutChar(bit1);
	}else if(bit1>= 0x31)
		PutChar(bit1);
	
	PutChar(bit0);

}
//converts a string of ascii chars to 1 integer
unsigned int Ascii2Int (unsigned char *n){
	unsigned int value;
	
	if(*n >= 0x30 && *n <=0x39){
			value += (*n - 0x30);
			n++;
	}
	
	while(*n!=0x00){
		if(*n >= 0x30 && *n <=0x39){
			value = value * 10;
			value += (*n - 0x30);
			n++;
		}
	}
	
	return value;
}

void GetNumber(unsigned char *b, unsigned char n){
	unsigned char i=0;
	while(i<n){
		GetChar(b);
		if(*b>=0x30 && *b<=0x39){
			b++;
			i++;
		}else if(*b==0x0D){
			*b = 0x00;
			break;
		}else{
			PutString("\r\nerror.");
			//break;
		}
	}
	*b = 0x00;

}
//converts 4 unsigned char into 1 integer(4bytes)
unsigned int Byte2Int(unsigned char b[]){
	unsigned int value = 0;
	value = (b[0]<<24)&0xFF000000|
					(b[1]<<16)&0x00FF0000|
					(b[2]<< 8)&0x0000FF00|
					(b[3]<< 0)&0x000000FF;
	
	return value;
}

//splits 4-byte integer to 4 unsigned char by populating *b
void Int2Byte(unsigned int n, unsigned char *b){
	*b = (n >>24) & 0xFF;
	b++;
	*b = (n >>16) & 0xFF;
	b++;
	*b = (n >> 8) & 0xFF;
	b++;
	*b = (n >> 0) & 0xFF;
	b++;
}


void ConsoleComment(void){
	unsigned char c = 0x00;
	
	GetChar(&c);
	while(c!=0x0D && c!=0x0A){
		GetChar(&c);
	}
	PutString("\r\nDone!\r\n");
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
	SpiReadWrite(0x04);			 // 3 byte RX payload width
	SpiReadWrite(0x04);			 // 3 byte TX payload width
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

