#ifndef __PACKET_H_
#define __PACKET_H_
#include "udt.h"

#pragma pack(1)
struct PacketHeader {
	uint32 _len;
	byte _compress;
	byte _encrypt;
	uint16 _opcode;
};

#pragma push

#endif // __PACKET_H_

