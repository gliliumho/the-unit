# NRF9E5 Library #

_Last update: 20/10/2015_

## Intro ##

This repo contains the source codes to create the library of functions for __Nordic Semiconductor NRF9E5__.
Reason I'm making this to a library is because the compiler will include the unneeded functions into the HEX 
file. 

Eg.

```
void Int2Char(unsigned int n, unsigned char (*b)[2]){
	unsigned char i;
	for(i=0; i<2; i++)
		*b[i] = (n>>(8-(i*8)))&0xFF;
}

void InitTimer0(unsigned char mode){
	TMOD =	(TMOD & 0xF0)|
			(mode & 0x0F);
}

void main(void){
	unsigned char (*two_char)[2] = malloc(sizeof *two_char);
	unsigned int one_int = 1439;
	
	Int2Char(one_int, two_char);
}
```

As you can see, my main() doesn't only use Int2Char() and InitTimer0 is not used. But uVision will
still include InitTimer0() into the firmware because it is treated as [potental interrupt routines][1].

I figured it would be troublesome to add and remove each .c prior to compiling so I figured I'll just make them
into a library so only the necessary functions will be linked during compile time. 


