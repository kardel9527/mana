#ifndef __ACCEPTOR_H_
#define __ACCEPTOR_H_
#include "udt.h"
#include "macros.h"
#include "ihandler.h"

NMS_BEGIN(kevent)

class Acceptor : public IHandler {
public:
	Acceptor() : _sock(-1) {}
	virtual ~Acceptor();

	int32 open(const char *addr, uint16 port);

	virtual int32 get_handle() { return _sock; }

	virtual int32 handle_input();

	virtual int32 handle_close();

private:
	Acceptor(const Acceptor &rh) {}
	Acceptor& operator = (const Acceptor &rh) { return *this; }

private:
	int32 _sock;
};

NMS_END // end namespace kevent

#endif // __ACCEPTOR_H_
