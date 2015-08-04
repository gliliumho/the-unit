
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
	
	unsigned char i,j,n = 0x30;
	TXEN = 1;
	
	
	//transmit 0x2B to indicate start of transmitter
	
	
	while(1){
		if(P03 == 0){
			TransmitPacket(0x2B);
			Delay5ms(30);		//delay 0.25s
			TransmitPacket(n);
			Delay5ms(10);
			n++;
			if (n > 0x39)
				n = 0x30;
			P00 = 0;
			
			for(i=0x61; i<=0x69; i++){
				TransmitPacket(i);
				Delay5ms(10);		//delay 0.10s
				
				for(j=0x30; j<=0x39; j++){

					//PutString("Transmitting letter: ");
					//PutChar(letter);
					//PutString("\r\n");
					TransmitPacket(j);

					Delay5ms(10);		//delay 0.15s
				}
			}
			
			P00 = 1;
		}
					
	}
}

void Receiver(void){
	unsigned char letter = 0x00;
	TXEN = 0;
	
	while(1){
		letter = ReceivePacket(); 
		
		//if letter is 0-9 or a-z
		if((letter >= 0x30 && letter <= 0x39)||(letter>=0x61 && letter<=0x7A)){
			P00 = 0;	//Turn on LED1
			
			//if letter is a-z, print new line
			if(letter>=0x61 && letter<=0x7A)
				PutString("\r\n");
			
			PutChar(letter);
			letter = 0x00;
		} 
		else if(letter == 0x2B){
			//to indicate transmitter just started
			PutString("\r\n \r\nNew Transmission:");
			letter = ReceivePacket();
			PutChar(letter);
			
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

