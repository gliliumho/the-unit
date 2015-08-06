
#include <Nordic\reg9e5.h>

#define HFREQ 0		// 0=433MHz, 1=868/915MHz
#define POWER 3 	// 0=min power...3 = max power

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

unsigned char SpiReadWrite(unsigned char b){
	EXIF &= ~0x20;				  // Clear SPI interrupt
	SPI_DATA = b;						// Move byte to send to SPI data register
	while((EXIF & 0x20) == 0x00)	// Wait until SPI has finished transmitting
		;
	return SPI_DATA;
}

void TransmitPacket(unsigned char b){
	RACSN = 0;
	SpiReadWrite(WTP);
	SpiReadWrite(b);
	RACSN = 1;
	TRX_CE = 1;
	Delay400us(1);
	TRX_CE = 0;
}

unsigned char ReceivePacket(void){
	unsigned char b;

	TRX_CE = 1;

	while(DR == 0)
		;
	RACSN = 0;
	SpiReadWrite(RRP);
	b = SpiReadWrite(0);
	RACSN = 1;
	TRX_CE = 0;
	return b;
}

void InitUART(void){
	
	unsigned char cklf;
	
	TH1 = 0xE6;					  // 9600@16MHz (when T1M=1 and SMOD=1)
	CKCON |= 0x10;				  // T1M=1 (/4 timer clock)
	PCON = 0x80;					// SMOD=1 (double baud rate)
	SCON = 0x52;					// Serial mode1, enable receiver
	TMOD = 0x20;					// Timer1 8bit auto reload 
	TR1 = 1;						// Start timer1
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
	while(!TI)
		;
	TI = 0;
	SBUF = c;
}

void PutString(unsigned char *s){
	while(*s != 0)
		PutChar(*s++);
}

void GetChar(unsigned char *c){
	while(!RI)
		;
	RI=0;
	*c = SBUF;
}

void GetString(unsigned char *s){
	GetChar(s);
	while(*s!= 0x0D && *s!= 0x0A){	//GetChar as long as not ENTER.
		s++;
		GetChar(s);
	}
	*s = 0;
}

void PrintNumber(unsigned int n){
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
void ConsoleComment(void){
	unsigned char c = 0x00;
	
	GetChar(&c);
	while(c!=0x0D && c!=0x0A){
		PutChar(c);
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
	SpiReadWrite(0x01);			 // One byte RX payload width
	SpiReadWrite(0x01);			 // One byte TX payload width
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
	unsigned char i,j,n = 0x00;
	TXEN = 1;
	
	while(1){
		if(P03 == 0){
			
			for(i=0;i<50;i++){
				TransmitPacket(0x01);
				Delay400us(5);
				TransmitPacket(n);
				P00 ^= 1;
			}
			
			Delay5ms(10);
			P00 = 0;
			
			for(i=0x61; i<=0x74; i++){
				TransmitPacket(i);
				P00 ^= 1;
				Delay5ms(10);		//delay 0.10s
				for(j=0x31; j<=0x39; j++){
					//PutString("Transmitting letter: ");
					//PutChar(letter);
					//PutString("\r\n");
					TransmitPacket(j);
					P00 ^= 1;
					Delay5ms(10);		//delay 0.15s
				}
			}
			
			for(i=0;i<100;i++){
				TransmitPacket(0x04);
				P00 ^= 1;
				Delay400us(5);
			}
			
			n++;
			if (n > 0x09)
				n = 0x00;
			P00 = 1;
		}
					
	}
}

void Receiver(void){
	unsigned char letter = 0x00;
	unsigned char headerFlag = 0;
	unsigned char endFlag = 1;
	unsigned int packetCount = 0;
	TXEN = 0;
	
	PutString("\r\n \r\nReceiver started.");
	
	while(1){	
		letter = ReceivePacket(); 
		
		if(((letter >= 0x30 && letter <= 0x39)||(letter>=0x61 && letter<=0x7A)) && endFlag==0){	
			//if letter is 0-9 or a-z
			headerFlag = 0;
			P00 = 0;	//Turn on LED1
			
			//if letter is a-z, print new line
			if(letter>=0x61 && letter<=0x7A)
				PutString("\r\n");
			
			packetCount++;
			PutChar(letter);
			letter = 0x00;
		} else if(letter == 0x01 && headerFlag == 0){	//to indicate transmitter just started
			letter = ReceivePacket();
			PutString("\r\n \r\nNew Transmission:");
			letter += 0x30;
			PutChar(letter);
			headerFlag = 1;
			endFlag = 0;
			packetCount = 0;
		} else if(letter == 0x04 && endFlag == 0){	//to indicate end of transmission
			headerFlag = 0;
			endFlag = 1;
			PutString("\r\nEnd of Transmission. Received ");
			PrintNumber(packetCount);
			PutString("/200 packets sent. Packet loss: ");
			packetCount = (200-packetCount)*100/200;
			PrintNumber(packetCount);
			PutChar(0x25);
			packetCount = 0;
			PutString("\r\nEnter Comment: ");
			ConsoleComment();
		}
	}
	
}

void main(){
	
	InitPin(0,0);	//Initialize P00 for LED1
	InitPin(3,1);
	InitPin(5,1);
	
	P00 = 1; 		//Initialize with LED1 turned OFF
	
	InitUART();
	InitRF();
	
	if(P03 == 0){
		Transmitter();
	} else if (P05 == 0){
		Receiver();
	}
	
}

