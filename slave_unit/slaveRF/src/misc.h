/*
** misc.h
**
** Header file for the miscellaneous functions made during development.
** These functions are unlikely to be used in deployment.
**
*/

//Data type manipulation
extern unsigned int Char2Int(unsigned char b[2]);
extern void Int2Char(unsigned int n, unsigned char b[2]);

//Timer 0
extern void InitTimer0(unsigned char mode);
extern void ReloadTimer0(unsigned char high, unsigned char low);

//UART Functions
extern void GetStringLimit(unsigned char s[], unsigned char n);
extern void PrintInt(unsigned int n);
extern void PutString_Pointer(unsigned char s[0x10] );
extern void GetNumber(unsigned char *b, unsigned char n);
extern unsigned int GetIntNumber(unsigned char n);
extern unsigned int Ascii2Int (unsigned char s[], unsigned char n);
extern void ConsoleComment(void);
