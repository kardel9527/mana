#ifndef __CONNECTOR_H_
#define __CONNECTOR_H_
#include "macros.h"
#include "udt.h"
#include "ihandler.h"

NMS_BEGIN(kevent)

class Connector : public IHandler {
public:
	Connector() : _timer_id(0), _interval(15), _port(0) {}
	Connector(uint32 interval) : _timer_id(0), _interval(interval), _port(0) {}
	~Connector() { _timer_id = 0; _interval = 0; _port = 0; }

	int32 open();

	void set_addr(const char *addr, uint16 port);

	virtual int32 handle_timeout();

private:
	int32 connect();

private:
	int32 _timer_id;
	uint32 _interval;
	uint16 _port;
	char _addr[64];
	int32 _sock;
};

NMS_END // end namespace kevent

#endif // __CONNECTOR_H_

