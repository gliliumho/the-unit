/********************************************************************
** 	radio.h
**
**	This file is the header file for radio.c, radio_master.c and radio_slave.c
**
*********************************************************************/

#define HFREQ 0		// 0=433MHz, 1=868/915MHz
#define POWER 3 	// 0=min power...3 = max power
#define PACKET_SIZE 0x10
#define TRAFFIC_INFO_HEADER 0x01
#define HEARTBEAT_REQUEST_HEADER 0x02
#define HEARTBEAT_REPLY_HEADER 0x03
#define HEARTBEAT_REQUEST_FROM_ALL_HEADER 0x04

//definition of packetID struct
typedef struct packetID{
	unsigned char groupid;
	unsigned char uniqueid;
	unsigned char packet_no;
} packetID;

extern packetID id_buffer[6];
extern unsigned char packet_count;
extern unsigned char buffer_count;

extern unsigned char PacketIdEqual(packetID a, packetID b);
extern void PacketIdCpy(packetID *a, packetID b);

//declaration for radio.c
extern void InitRF(void);
extern void TransmitPacket(unsigned char b[PACKET_SIZE]);
extern void ReceivePacket(unsigned char b[PACKET_SIZE]);

//declaration for radio_master.c
extern void SendTraffic(unsigned char b[PACKET_SIZE]);
extern void RequestHeartbeat(unsigned char gid, unsigned char uid);
extern unsigned char ListenHeartbeat(unsigned char *gid, unsigned char *uid);
extern unsigned char WaitHeartbeat(unsigned char gid, unsigned char uid);
extern void RequestHeartbeatFromAll(void);
extern unsigned char WaitMultiHeartbeat( unsigned char id[][2], unsigned char id_len);
extern void RequestHeartbeatLoop(void);

//declaration for radio_slave.c
extern void SlaveOp_Buffer(unsigned char gid, unsigned char uid);
extern unsigned char SlaveReceive(unsigned char b[PACKET_SIZE]);
extern void CheckTraffic(unsigned char b[PACKET_SIZE], unsigned char gid);
extern void SendHeartbeat(unsigned char gid, unsigned char uid);
extern void SlaveRelay(unsigned char b[PACKET_SIZE]);
extern unsigned char CheckBuffer(packetID a);
