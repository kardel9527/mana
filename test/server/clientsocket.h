#ifndef __CLIENT_SCOKET_H_
#define __CLIENT_SOCKET_H_
#include <signal.h>
#include "logger.h"
#include "netiohandler.h"

using namespace kcommon;
using namespace network;

class Room;

class TestClientHandler : public NetIoHandler {
public:
	TestClientHandler() : NetIoHandler(NHLT_CLIENT), _r(0) {}

	virtual void handle_connect();

	virtual void handle_disconnect();

	virtual void handle_packet(const char *data);

	void room(Room *r) { _r = r; }

private:
	Room *_r;
};

#endif // __CLIENT_SOCKET_H_

