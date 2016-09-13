#include <sys/types.h>
#include <sys/socket.h>
#include "stringutil.h"
#include "netiohandler.h"

NMS_BEGIN(kevent)

void NetIoHandler::set_addr(const char *addr, uint16 port) {
	_port = port;
	kcommon::stringutil::smemcpy(_addr, addr, sizeof(_addr));
}

int32 NetIoHandler::handle_input() {
	return 0;
}

int32 NetIoHandler::handle_output() {
	return 0;
}

int32 NetIoHandler::send(const char *data, uint32 len, bool immediately/* = true*/) {
	return 0;
}

int32 NetIoHandler::disconnect() {
	return 0;
}

int32 NetIoHandler::reconnect() {
	return 0;
}

bool NetIoHandler::is_active() {
	return false;
}

NMS_END // end namespace kevent

