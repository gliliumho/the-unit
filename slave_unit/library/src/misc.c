/*******************************************************************************
** misc.c
**
** This file contains all the functions I developed for testing but will not be
** used in the final product. I'm separating them out to reduce size of program
** code.
**
*********************************************************************************/

#include <reg9e5.H>
#include <stdlib.h>
#include "util.h"
#include "uart.h"
#include "radio.h"
#include "misc.h"

/* ===============Timer 0=============== */

// initialize Timer 0 according to the parameter mode
void InitTimer0(unsigned char mode){
	TMOD =	(TMOD & 0xF0)|
			(mode & 0x0F);
}

// reload the values of Timer 0 after overflow
void ReloadTimer0(unsigned char high, unsigned char low){
	TH0 = high;
	TL0 = low;
}


/* ===============Data Type Manipulation=============== */

// converts 2 unsigned char into 1 integer(4bytes)
// This is written in case a large number is needed to be transmitted through RF
unsigned int Char2Int(unsigned char b[2]){
	unsigned int value = 0;
	value = (b[0] << 8) & 0xFF00|
			(b[1] << 0) & 0x00FF;
	return value;
}

//splits 2-byte integer to 2 unsigned char by populating *b
//This is written in case a large number is needed to be transmitted through RF
void Int2Char(unsigned int n, unsigned char b[2]){
	unsigned char i;
	for(i=0; i<2; i++)
		b[i] = (n >> 8-(i*8) ) & 0xFF;
}



/* ===============UART Communications=============== */

//BE CAREFUL WHEN CALLING THIS FUNCTION!!!
//This does not insert 0x00 at end of string so PutString doesn't know where is end of array.
void GetStringLimit(unsigned char *s, unsigned char n){
	unsigned char i;
	while(i<n){
		GetChar(&s[i]);
		i++;
	}
}


//Print fixed size array/string
void PutString_Pointer(unsigned char s[PACKET_SIZE] ){
	unsigned char i;

	for(i=0; i<PACKET_SIZE; i++)
	PutChar(s[i]);
}


//This function is used to print the value of int variable
void PrintInt(unsigned int n){
	unsigned char i, bits[5];
	unsigned int temp;
	for(i=4; i>0; i--){
		switch(i){
			case 4:
				temp = 10000; break;
			case 3:
				temp = 1000; break;
			case 2:
				temp = 100; break;
			case 1:
				temp = 10; break;
		}
		if( n >= temp){
			bits[i] = n/temp + 0x30;
			n %= temp;
		}else
			bits[i] = 0x30;
	}
	bits[0] = (n + 0x30);
	i = 4;
	while(!(bits[i] >= 0x31) && 0<i<5)
		i--;
	while(i>=0 && i<5){
		PutChar(bits[i]);
		i--;
	}
}

//Prompt user for (n or less) digits from UART then populate array *b
void GetNumber(unsigned char b[], unsigned char n){
	unsigned char i=0;
	while(i<=n){
		GetChar(&b[i]);
		if(b[i] >= 0x30 && b[i] <= 0x39){
			i++;
		} else if(b[i] == 0x0D){
			b[i] = 0x00;
			break;
		} else{
			PutString("\r\nerror.");
		}
	}
}

//Prompt user for (n or less) digits then return the value in unsigned int
unsigned int GetIntNumber(unsigned char n){
	unsigned char b[5];
	unsigned char i=0;
	while(i<n){
		GetChar(&b[i]);
		if(b[i] >= 0x30 && b[i] <= 0x39){
			i++;
		} else if(b[i] == 0x0D){
			b[i] = 0x00;
			break;
		}
	}
	return Ascii2Int(&b, i);
}

//Used to convert string ASCII characters, *s, to unsigned int
unsigned int Ascii2Int (unsigned char s[], unsigned char n){
	unsigned int value = 0;
	unsigned char i;
	for(i=0; i<n; i++){
		if(s[i] >= 0x30 && s[i] <= 0x39){
			value *= 10;
			value += (s[i] - 0x30);
		}
	}
	return value;
}

//Allow comment on PuTTy for testRange.
void ConsoleComment(void){
	unsigned char c = 0x00;
	GetChar(&c);
	while(c != 0x0D && c != 0x0A)
		GetChar(&c);
	PutString("\r\nDone!\r\n");
}
