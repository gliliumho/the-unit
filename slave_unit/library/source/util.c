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
	EXIF &= ~0x20;
	SPI_DATA = b;
	while((EXIF & 0x20) == 0x00)
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
	temp |= (0x0F & mode);
	TMOD = temp;
}

void ReloadTimer0(unsigned char high, unsigned char low){
	TH0 = high;
	TL0 = low;
}




