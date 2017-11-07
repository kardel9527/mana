#ifndef __ACCEPTOR_H_
#define __ACCEPTOR_H_
#include "udt.h"
#include "macros.h"
#include "inetaddr.h"
#include "ihandler.h"

NMS_BEGIN(network)

class Acceptor : public kevent::IHandler {
public:
	Acceptor() : _type(0) { }
	Acceptor(int32 type, const kcommon::InetAddr &addr) : _type(type), _addr(addr) { }
	virtual ~Acceptor() { close(); _type = 0; }

	int32 open();
	void close();

	virtual int32 handle_input();

	virtual int32 handle_close();

private:
	Acceptor(const Acceptor &rh) {}
	Acceptor& operator = (const Acceptor &rh) { return *this; }

private:
	int32 _type;
	kcommon::InetAddr _addr;
};

NMS_END // end namespace network

#endif // __ACCEPTOR_H_
