/********************************************************************
** 	util.h
**
**	This file is the header file for util.c
**
*********************************************************************/


extern void InitPin(unsigned char pinNum, unsigned char direction);
extern void Delay400us(volatile unsigned char n);
extern void Delay5us(volatile unsigned char n);
extern unsigned char SpiReadWrite(unsigned char b);
extern unsigned int Byte2Int(unsigned char b[]);
extern void Int2Byte(unsigned int n, unsigned char *b);