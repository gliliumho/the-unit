/********************************************************************
** 	radio.h
**
**	This file is the header file for radio.c
**
*********************************************************************/

#define HFREQ 0		// 0=433MHz, 1=868/915MHz
#define POWER 3 	// 0=min power...3 = max power

extern void InitRF(void);
extern void TransmitPacket(unsigned char *b);
extern void ReceivePacket(unsigned char *b);
extern void SetAutoRetransmit(unsigned char setting);
extern void SetPayloadWidth(unsigned char w);
extern void SetTXPower(unsigned char powerlevel);
extern void SetFrequency(unsigned char freq);
extern void SetLowRXPower(unsigned char flag);
extern void SetRXAddress(unsigned char *addr, unsigned char addr_size);
extern void SetTXAddress(unsigned char *addr, unsigned char addr_size);
extern void RadioPowerUpDown(unsigned char val);
extern void SetCRCMode(unsigned char flag);
extern void CRCEnableDisable(unsigned char flag);
extern void SendTraffic(unsigned char *info);
extern unsigned char CheckTraffic(unsigned char *info, unsigned char groupID);
extern void RequestHeartbeat(unsigned char groupID, unsigned char uniqueID);
extern void SendHeartbeat(unsigned char groupID, unsigned char uniqueID);