/**********************************************************************************************************
** 	util.c
**	
**	This file contains all the utilities such as GPIO initialization, 
**	SpiReadWrite for SPI operations, etc
**
***********************************************************************************************************/

#include <Nordic\reg9e5.h>
#include "util.h"

/* Set pinNum as GPIO. direction=1 for input, direction=0 for output
** eg.InitPin(1,1) will set P01 as input GPIO. */
void InitPin(unsigned char pinNum, unsigned char direction){
	P0_ALT &= (~(0x01 << pinNum));
	if (direction == 1)
		P0_DIR |= (0x01 << pinNum);
	else 
		P0_DIR &= (~(0x01 << pinNum));
	
	/*
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
			
		default:
			break;	
	}
	*/
}

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

void InitTimer0(unsigned char mode){
	unsigned char temp;
	
	temp = (TMOD & 0xF0);
	temp &= (0x0F & mode);
	TMOD = temp;
}

void ReloadTimer0(unsigned char high, unsigned char low){
	TH0 = high;
	TL0 = low;
}




