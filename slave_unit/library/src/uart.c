/*******************************************************************************
** uart.c
** This file contains all the functions for UART operations
** such as GetString, PutString, GetNumber & etc.
**
*********************************************************************************/

#include <Nordic\reg9e5.h>
#include <math.h>
#include "util.h"
#include "uart.h"
//#include "radio.h"

//Initialize UART/RS-232. Must call this function before any UART functions
void InitUART(void){
	unsigned char cklf;

	TH1 = 0xE6;					// 9600@16MHz (when T1M=1 and SMOD=1)
	CKCON |= 0x10;				// T1M=1 (/4 timer clock)
	PCON = 0x80;				// SMOD=1 (double baud rate)
	SCON = 0x52;				// Serial mode1, enable receiver
	TMOD = 0x20;				// Timer1 8bit auto reload
	TR1 = 1;					// Start timer1

	P0_ALT |= 0x06;				//select alternative function for P01 and P02
	P0_DIR |= 0x02;				//P01(RXD) is input

	SPICLK = 0;					//Max SPI clock
	SPI_CTRL = 0x02;			//Connect SPI to RF. Not sure if needed.

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

//Prints a character through UART
void PutChar(unsigned char c){
	while(!TI) 	//TI=Transmit Interupt. TI=0 when UART TXD is busy
		;
	TI = 0;
	SBUF = c;	//SBUF will be transmitted through UART
}

//Prints a string through UART
void PutString(unsigned char *s){
	while(*s != 0)
		PutChar(*s++);
}

//This function is used to print the value of char variable
void PrintChar(unsigned char n){
	unsigned char i, bits[3];
	unsigned char temp;

	for(i=2; i>0; i--){
		switch(i){
			case 2:
				temp = 100;
				break;
			case 1:
				temp = 10;
				break;
		}
		if( n >= temp){
			bits[i] = n/temp + 0x30;
			n %= temp;
		}else
			bits[i] = 0x30;
	}
	bits[0] = (n + 0x30);
	if(bits[2] >= 0x31)
		i=2;
	else if(bits[1] >= 0x31)
		i=1;
	while(i>=0 && i<3){
		PutChar(bits[i]);
		i--;
	}
}

//Input a character from UART (PuTTY to board)
void GetChar(unsigned char *c){
	while(!RI)
		;
	RI=0;
	*c = SBUF;
	PutChar(*c);	//for internal echo
}

//Input a string and save to *s
// input ends with '\n' while the string is 0x00/NULL-terminated
void GetString(unsigned char *s){
	GetChar(s);
	while(*s!= 0x0D && *s!= 0x0A){	//GetChar as long as not ENTER.
		s++;						//move pointer to next byte
		GetChar(s);
	}
	*s = 0x00; 						//0x00 to indicate end of string(EOS)
}

//Input n bytes of character from UART and save to *s
//Written to interface with master_unit's Python script
void GetFixedString(unsigned char *s, unsigned char n){
	unsigned char i;
	RI=0;
	for (i=0; i<n; i++){
		GetChar(s++);
		//s++;
	}
}

//Print n bytes of character from UART, starting from *s
//Written to interface with master_unit's Python script
void PutFixedString(unsigned char *s, unsigned char n){
	unsigned char i;
	for(i=0; i<n; i++)
		PutChar(*(s++));
}

unsigned char GetNumber(void){
	unsigned char i=0, input=0, value=0;

	while(i < 3){
		GetChar(&input);
		if(input >= 0x30 && input <= 0x39){
			value *= 10;
			value += input;
		} else if(input == 0x0D){
			break;
		} else {
			PutString("\r\nerror.");
		}
	}
	return value;
}
