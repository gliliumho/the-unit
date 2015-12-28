/********************************************************************
** 	radio_slave.h
**
**	This file contains prototype for slave's radio functions.
**
*********************************************************************/

extern void SlaveOp_Buffer(unsigned char groupID, unsigned char uniqueID);
extern unsigned char SlaveReceive(unsigned char b[PACKET_SIZE]);
extern void CheckTraffic(unsigned char b[PACKET_SIZE], unsigned char groupID);
extern void SendHeartbeat(unsigned char groupID, unsigned char uniqueID);
extern void SlaveRelay(unsigned char b[PACKET_SIZE]);
extern unsigned char CheckBuffer(packetID a);
