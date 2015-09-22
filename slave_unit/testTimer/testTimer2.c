#include <reg9e5.H>

void TOM1Delay(void);
sbit mybit=P1^4;
void main(void)
{
	while(1)
	{
		mybit=~mybit;
		TOM1Delay();
	}
}
void TOM1Delay(void)
{
	TMOD=0x01;	//Timer0, mode1 (16 bit counter)
	TL0=0xFD;
	TH0=0x4B;
	TR0=1;
	while(TF0==0);
	TR0=1;
	TF0=0;
}