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

unsigned char SpiReadWrite(unsigned char b){
	EXIF &= ~0x20;
	SPI_DATA = b;
	while((EXIF & 0x20) == 0x00)
		;
	return SPI_DATA;
}



/*	SpiReadWrite() is used to read/write to register using SPI.
**	SpiReadWrite(REGISTER) will specify the register/byte you want to write to.
**	Then SpiReadWrite(VALUE) will write value to register. If you use it again, it'll write to next byte.
** 	c = SpiReadWrite() will read value from register.
**
**	Chances are, you still don't get what SpiReadWrite is and scratching your head by now.
**	Here's a pic of NyanCat.
**
**	░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
**	░░░░░░░░░░▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄░░░░░░░░░
**	░░░░░░░░▄▀░░░░░░░░░░░░▄░░░░░░░▀▄░░░░░░░
**	░░░░░░░░█░░▄░░░░▄░░░░░░░░░░░░░░█░░░░░░░
**	░░░░░░░░█░░░░░░░░░░░░▄█▄▄░░▄░░░█░▄▄▄░░░
**	░▄▄▄▄▄░░█░░░░░░▀░░░░▀█░░▀▄░░░░░█▀▀░██░░
**	░██▄▀██▄█░░░▄░░░░░░░██░░░░▀▀▀▀▀░░░░██░░
**	░░▀██▄▀██░░░░░░░░▀░██▀░░░░░░░░░░░░░▀██░
**	░░░░▀████░▀░░░░▄░░░██░░░▄█░░░░▄░▄█░░██░
**	░░░░░░░▀█░░░░▄░░░░░██░░░░▄░░░▄░░▄░░░██░
**	░░░░░░░▄█▄░░░░░░░░░░░▀▄░░▀▀▀▀▀▀▀▀░░▄▀░░
**	░░░░░░█▀▀█████████▀▀▀▀████████████▀░░░░
**	░░░░░░████▀░░███▀░░░░░░▀███░░▀██▀░░░░░░
**	░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
**
*/
