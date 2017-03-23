#include "netiohandler.h"
#include "session.h"
#include "sessionmgr.h"

void SessionMgr::handle_new_connect(kevent::NetIoHandler *handler) {
	AutoLock guard(_connected_list_lock);
	_connected_list.push_back(handler);
}

void SessionMgr::handle_disconnect(kevent::NetIoHandler *handler) {

}


