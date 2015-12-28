-----
# Files #

+ **nRF9E5-FW.zip** - Contains the sample codes from Nordic Semiconductor.
+ **Product_Specification_nRF9E5_v1.6_highlight.pdf** - Specification sheet for
NRF9E5. Descriptions on the 8051 MCU, RF transceivers and other components.
Reading this document will be useful for understanding of the hardware and
functions in general.

-----
# Function Reference #
-----
# Utilities - __util.c__ #

void __InitPin__ ( unsigned char __pinNum__, unsigned char __direction__ )

Initialize the P0 specified as GPIO for input or output.
>Parameter :
>
> pinNum - 0~7 for pin number to be GPIO
> direction - 0 for Output / 1 for Input
>
>Return :
> void


void __Delay400us__ ( volatile unsigned char __n__ )

Busy idle for 400us
>Parameter :
>
> n - number of times to loop
>
>Return :
> void


void __Delay5ms__ ( volatile unsigned char __n__ )

Busy idle for 5ms
>Parameter :
>
> n - number of times to loop
>
>Return :
> void


unsigned char __SpiReadWrite__ ( unsigned char __b__ )

Read from or write to SPI
>Parameter :
>
> b - byte to write to SPI
>
>Return :
> byte read from SPI


--------
## UART Functions - __uart.c__ ##

void __InitUART__ ( void )

Initialize P01 & P02 for UART
>Parameter :
> void
>
>Return :
> void


void __PutChar__ ( unsigned char __c__ )

Print character to console / PuTTy through UART
>Parameter :
>
> c - Character to print
>
>Return :
> void


void __PutString__ ( unsigned char __\*s__ )

Print string to console / PuTTy through UART
>Parameter :
>
> \*s - Pointer to string to print
>
>Return :
> void

void __GetChar__ ( unsigned char __\*c__ )

Get character from console through UART
>Parameter :
>
> \*c - Pointer to character variable to populate
>
>Return :
> void

void __GetString__ ( unsigned char __\*s__ )

Get string from console through UART
>Parameter :
>
> \*s - Pointer to array of char to populate
>
>Return :
> void


--------
## EEPROM Functions - __eeprom.c__ ##


void __InitEEPROM__ ( void )

Initialize EEPROM
>Parameter :
> void
>
>Return :
> void


unsigned char __EEStatus__ ( void )

Returns EEPROM status
>Parameter :
> void
>
>Return :
> EEPROM status


unsigned char __EERead__ ( unsigned int __addr__ )

Read the content of EEPROM at addr.
>Parameter :
>
> addr - EEPROM address to read from.
>
>Return :
> EEPROM content at the address, addr.


void __EEWrite__ ( unsigned int __addr__,  unsigned char __b__ )

Write byte to EEPROM at addr.
>Parameter :
>
> addr - EEPROM address to write to.
>
> b - Byte to write at addr
>
>Return :
> void


--------
## Radio Functions - __radio.c__ ##


void __InitRF__ ( void )

Initialize RF Transceiver. Called at beginning or program before any RF
operation.
>Parameter :
> void
>
>Return :
> void


void __TransmitPacket__ ( unsigned char __b[ ]__ )

Transmit packet containing the array passed.
>Parameter :
>
> b[] - array to be transmitted as packet
>
>Return :
> void


void __ReceivePacket__ ( unsigned char __b[ ]__ )

Blocking function that waits for valid packet. Once packet is received, it will
populate the array b[].
>Parameter :
>
> b[] - array to be populated with packet received.
>
>Return :
> void


--------
## Slave's Radio Functions - __radio_slave.c__ ##


void __SlaveOp\_Buffer__ ( unsigned char __groupID__, unsigned char __uniqueID__ )

Will receive packet and look into the contents of packet, then decide whether
to relay packet or not.
>Parameter :
>
> groupID - slave's own groupID
>
> uniqueID - slave's own uniqueID
>
>Return :
> void


unsigned char __SlaveReceive__ ( unsigned char __b[ ]__ )

Called by SlaveOp_Buffer().
This function is the same as ReceivePacket but this function is non blocking
>Parameter :
>
> groupID - slave's own groupID
>
> uniqueID - slave's own uniqueID
>
>Return :
> void


void __CheckTraffic__ ( unsigned char __b[ ]__, unsigned char __groupID__ )

Called by SlaveOp_Buffer() if the header byte is 0x01.
This function lights up the LED according to traffic info.
>Parameter :
>
> b[] - Array that stores the packet received
>
> groupID - slave's own groupID
>
>Return :
> void


void __SendHeartBeat__ ( unsigned char __groupID__, unsigned char __uniqueID__ )

Called by SlaveOp_Buffer() if header is 0x02 and groupID matches.
This function sends heartbeat back to master.
>Parameter :
>
> groupID - slave's own groupID
>
> uniqueID - slave's own uniqueID
>
>Return :
> void


void __SlaveRelay__ ( unsigned char __groupID__, unsigned char __uniqueID__ )

Called by SlaveOp_Buffer() after every radio operation.
This function will check the id_buffer[] and determine if packet should be
retransmitted or not.
>Parameter :
>
> groupID - slave's own groupID
>
> uniqueID - slave's own uniqueID
>
>Return :
> void


unsigned char __CheckBuffer__ ( packetID __a__ )

Called by SlaveRelay() and any function that checks id_buffer[].  
This function compares __a__ with existing packetID in id_buffer[]. Returns 0
if not in buffer.
>Parameter :
>
> a -packetID to be compared to elements in id_buffer
>
>Return :
> 0/1 to indicate whether a already exists in id_buffer


--------
## Master's Radio Functions - __radio_master.c__ ##


void __SendTraffic__ ( unsigned char b[] )

Sends traffic info for all groups. Each byte represents traffic info for
a group (for now). Each byte will represent 2 bytes in future to support more
groups.
>Parameter :
>
> b[] - 
>
> uniqueID - slave's own uniqueID
>
>Return :
> void





*to be continued....*
