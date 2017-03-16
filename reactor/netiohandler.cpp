#include <sys/types.h>
#include <sys/socket.h>
#include "stringutil.h"
#include "netiohandler.h"

NMS_BEGIN(kevent)

void NetIoHandler::set_addr(const char *addr, uint16 port) {
	_port = port;
	kcommon::stringutil::smemcpy(_addr, addr, sizeof(_addr));
}

int32 NetIoHandler::get_handle() { return _fd; }

void NetIoHandler::set_handle(int32 hid) { _fd = hid; }

int32 NetIoHandler::handle_input();

int32 NetIoHandler::handle_output();

int32 NetIoHandler::send(const char *data, uint32 len, bool immediately/* = true*/);

int32 NetIoHandler::disconnect();

int32 NetIoHandler::reconnect();

bool NetIoHandler::is_active() {

}

NMS_END // end namespace kevent

