/*
*	testTimer.c
*	This is a program to test and get familiar with the timer on 8051 MCU.
*	Some of the codes are copied from online tutorials.
*	
*	
*	Author: KH Teo
*	Dependencies: None
*	
*/

#include <Nordic\reg9e5.h>
#include <string.h>

void T0Delay(void);
void TOM1Delay(void);
sbit mybit=P0^4;

#define HFREQ 0		// 0=433MHz, 1=868/915MHz
#define POWER 3 	// 0=min power...3 = max power

/* Set pinNum as GPIO. direction=1 for input, direction=0 for output
** eg.InitPin(1,1) will set P01 as input GPIO. */
void InitPin(unsigned char pinNum, direction){
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
			
		case 8:
			P0_ALT = 0x00;
			if (direction == 1) P0_DIR = 0xFF;
			else P0_DIR = 0x00;
			break;
			
		default:
			break;	
	}
}

unsigned char SpiReadWrite(unsigned char b){
	EXIF &= ~0x20;				  // Clear SPI interrupt
	SPI_DATA = b;						// Move byte to send to SPI data register
	while((EXIF & 0x20) == 0x00)	// Wait until SPI has finished transmitting
		;
	return SPI_DATA;
}

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
	
	SPICLK = 0;			//Max SPI clock
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

void InitRF(void){
	
	unsigned char tmp;
	
	//Divider factor from CPU clock to SPI clock
	//0000: 1/2 of CPU clock frequency
	SPICLK = 0;						//Max SPI clock
	SPI_CTRL = 0x02;			//10->SPI connected to the nRF905 transceiver
	
	//Configure RF
	RACSN = 0;
	SpiReadWrite(WRC | 0x03);	   // Write to RF config address 3 (RX payload)
	SpiReadWrite(0x04);			 // 4 byte RX payload width
	SpiReadWrite(0x04);			 // 4 byte TX payload width
	RACSN = 1;

	RACSN = 0;
	SpiReadWrite(RRC | 0x01);	   // Read RF config address 1
	tmp = SpiReadWrite(0) & 0xf1;   // Clear the power and frequency setting bits (1111 0001 clears the bit [1][2]and[3])
	//RRC | 0x01
	//[7] [6] [5:Auto Retransmit] [4:Rx Red Pwr] [3:PA Power] [2:PA Power] [1:HFREQ PLL] [0:Channel Num[8]]
	RACSN = 1;

	RACSN = 0;
	SpiReadWrite(WRC | 0x01);	  // Write RF config address 1
	// Change power defined by POWER and to 433 or 868/915MHz defined by HFREQ above:
	SpiReadWrite(tmp | (POWER <<2) | (HFREQ << 1));
	RACSN = 1;
	
}


void main(){
	InitUART();
	
	/*
	InitPin(0,0);
	InitPin(2,0);
	
	while(1)
	{
		//OFF all LEDs
		P00=0;
		P02=0;
		T0Delay();
		//ON all LEDs
		P00=1;
		P02=1;
		T0Delay();
	}
	
	while(1)
	{
		mybit=~mybit;
		TOM1Delay();
	}*/
	
	
}

void T0Delay(){
	
	//Timer 0, Mode 1, 16-bit counter
	TMOD=0x01;
	//Initial counter value
	TL0=0x00;
	//Reload value (TL0 rolls over to TH0 value when the count increments from 0xFFFF)
	TH0=0x35;
	//Timer enable bit (TR0=1 to turn on T0)
	TR0=1;
	//Timer 0 overflow flag (when TF0=1 means roll over)
	while(TF0==0);
	//Timer enable bit (TR0=0 to turn off T0)
	TR0=0;
	//Clear Timer 0 overflow flag
	TF0=0;
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
