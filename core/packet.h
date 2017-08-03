#ifndef __PACKET_H_
#define __PACKET_H_
#include <arpa/inet.h>
#include "macros.h"
#include "udt.h"

NMS_BEGIN(kcore)

#pragma pack(push)
#pragma pack(1)
struct PacketHeader {
	uint32 _size;
	uint16 _opcode;
	byte _data[0];

	PacketHeader() : _size(0), _opcode(0) {}
	~PacketHeader() {}

	uint32 size() const { return (_size) & 0x00FFFFFF; }

	uint16 opcode() const { return (_opcode); }
};
#pragma pack(pop)

#define COMPRESS_BIT 0x40000000
#define ENCRIPT_BIT 0x20000000

#define PACKET_SIZE(SZ) (SZ & 0x00FFFFFF)

NMS_END // ens namespace kcore

#endif // __PACKET_H_

