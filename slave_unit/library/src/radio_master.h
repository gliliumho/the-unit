/********************************************************************
** 	radio_master.h (OUTDATED)
**
**	This file contains prototype for master's radio functions.
**
*********************************************************************/


extern void SendTraffic(unsigned char b[PACKET_SIZE]);
extern void RequestHeartbeat(unsigned char groupID, unsigned char uniqueID);
extern unsigned char WaitHeartbeat(unsigned char groupID, unsigned char uniqueID);
