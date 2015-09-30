/*******************************************************************************
** uart.c
** This file contains all the functions for UART operations
** such as GetString, PutString, GetNumber & etc.
**
*********************************************************************************/

#include <Nordic\reg9e5.h>
#include "util.h"
#include "uart.h"

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
}

/*Sample usage:
unsigned char a;
PutChar(a);
*/
void PutChar(unsigned char c){
	while(!TI) 	//TI=Transmit Interupt. TI=0 when UART TXD is busy
		;					
	TI = 0;
	SBUF = c;		//SBUF will be transmitted through UART
}

/*Sample usage:
unsigned char a[n];
PutString("Hello world!");
PutString(&a[0]);
*/
void PutString(unsigned char *s){
	while(*s != 0)
		PutChar(*s++);
}

/*Sample usage:
unsigned char a;
GetChar(&a);
*/
void GetChar(unsigned char *c){
	while(!RI) 		//RI=Receive Interupt. RI=0 when UART RXD is busy
		;
	RI=0;
	*c = SBUF; 		//SBUF stores the byte received through UART
	PutChar(*c);	//for internal echo
}

/*Sample usage:
unsigned char a[n];
GetString(&a[0]);
*/
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

/*Sample usage: Ascii2Int
unsigned char a[n];
Ascii2int(&a[0]);
*/
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

/*Sample usage: GetNumber
unsigned char a[n];
GetNumber(&a[0], n-1);
*/
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

void ConsoleComment(void){
	unsigned char c = 0x00;
	
	GetChar(&c);

	while(c!=0x0D && c!=0x0A){
		GetChar(&c);
	}
	PutString("\r\nDone!\r\n");
}

