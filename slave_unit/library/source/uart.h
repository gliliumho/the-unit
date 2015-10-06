/**********************************************************************
** uart.h
** 
** This file is the header file for uart.c
**
************************************************************************/

extern void InitUART(void);
extern void PutChar(unsigned char c);
extern void PutString(unsigned char *s);
extern void GetChar(unsigned char *c);
extern void GetString(unsigned char *s);
extern void PrintInt(unsigned int n);
extern unsigned int Ascii2Int (unsigned char *n);
extern void GetNumber(unsigned char *b, unsigned char n);
extern void ConsoleComment(void);