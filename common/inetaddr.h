#ifndef __INET_ADDR_H_
#define __INET_ADDR_H_
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include "udt.h"

NMS_BEGIN(kcommon)

class InetAddr {
public:
	InetAddr() { reset(); }
	InetAddr(const char *ip, uint16 port) { reset(); set(ip, port); }
	InetAddr(const InetAddr &other) { ::memcpy(&_addr, &(other._addr), sizeof(_addr)); }
	InetAddr& operator = (const InetAddr &rh) { ::memcpy(&_addr, &(rh._addr), sizeof(_addr)); return *this; }
	~InetAddr() { reset(); }

	void reset() { ::memset(&_addr, 0, sizeof(_addr)); _addr.sin_family = AF_INET; }

	void set(uint32 ip, uint16 port) { _addr.sin_port = ::htons(port); _addr.sin_addr.s_addr = ::htonl(ip); }

	void set(const char *ip, uint16 port) { _addr.sin_port = ::htons(port); _addr.sin_addr.s_addr = ::inet_addr(ip); }

	char* ip() { return ::inet_ntoa(_addr.sin_addr); }

	uint32 ip_addr() { return _addr.sin_addr.s_addr; }

	uint16 port() { return _addr.sin_port; }

	void* addr() { return (void *)&_addr; }
	
	void* addr() const { return (void *)&_addr; }

private:
	sockaddr_in _addr;
};

NMS_END // end namespace kcommon

#endif // __INET_ADDR_H_

