
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



/* void SetTXPower(unsigned char powerlevel){
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
} */


//void SetFrequency(unsigned char freq){
//	if(freq == 1)
//		CC |= 0x0200;
//	else
//		CC &= 0xFDFF;
//}


void Delay400us(volatile unsigned char n){
    unsigned char i;
    while(n--)
        for(i=0;i<35;i++)
            ;
}

unsigned char SpiReadWrite(unsigned char b){
    EXIF &= ~0x20;                  // Clear SPI interrupt
    SPI_DATA = b;                   // Move byte to send to SPI data register
    while((EXIF & 0x20) == 0x00)    // Wait until SPI hs finished transmitting
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




//not tested. Mostly copy from ex3c.c
void InitUART(void){
	
	unsigned char cklf;
	
	TH1 = 0xE6;                      // 9600@16MHz (when T1M=1 and SMOD=1)
	CKCON |= 0x10;                  // T1M=1 (/4 timer clock)
	PCON = 0x80;                    // SMOD=1 (double baud rate)
	SCON = 0x52;                    // Serial mode1, enable receiver
	TMOD = 0x20;                    // Timer1 8bit auto reload 
	TR1 = 1;                        // Start timer1
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
	//PutString(" Type 1 char: ");
	while(!RI)
		;
	RI=0;
	*c = SBUF;
}

void GetString(unsigned char *s){
	PutString("Enter 2 letters then ENTER: \r\n");
	GetChar(s);
	PutString("1a");
	while(*s!= 0x70){	//GetChar as long as not ENTER.
		GetChar(s++);
		PutString(s);
		PutString("2");
	}
	PutString("3a");
	*s = 0;
	PutString("3b");
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
	
	SPICLK = 0;			//Max SPI clock
	SPI_CTRL = 0x02;
	
	//Configure RF
	RACSN = 0;
	SpiReadWrite(WRC | 0x03);       // Write to RF config address 3 (RX payload)
    SpiReadWrite(0x02);             // Two byte RX payload width
    SpiReadWrite(0x02);             // Two byte TX payload width
    RACSN = 1;

    RACSN = 0;
    SpiReadWrite(RRC | 0x01);       // Read RF config address 1
    tmp = SpiReadWrite(0) & 0xf1;   // Clear the power and frequency setting bits
    RACSN = 1;

    RACSN = 0;
    SpiReadWrite(WRC | 0x01);      // Write RF config address 1
    // Change power defined by POWER and to 433 or 868/915MHz defined by HFREQ above:
    SpiReadWrite(tmp | (POWER <<2) | (HFREQ << 1));
    RACSN = 1;
	
}


void main(){
	//insert stuff
	unsigned char letter[10];
	
	InitPin(0,0);
	InitPin(2,0);
	InitPin(4,0);
	InitPin(6,0);
	
	P00 = 0;
	P02 = 0;
	P04 = 0;
	P06 = 0;
	
	InitUART();
	//PutString("Testing. \n");
	while(1){
		GetString(&letter[0]);
		PutString(&letter[0]);
		
//		if (letter[0] == '1'){
//			PutString("Word starts with '1'.\r\n");
//			P00 ^= 1;
//		}
//		if (letter[1] == '2'){
//			PutString("2nd letter is '2'.\r\n");
//			P04 ^= 1;
//		} 
	
	}
	
}

