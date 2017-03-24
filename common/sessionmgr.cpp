#include "netiohandler.h"
#include "session.h"
#include "sessionmgr.h"

void SessionMgr::handle_new_connect(kevent::NetIoHandler *handler) {
	AutoLock guard(_connected_list_lock);
	_connected_list.push_back(handler);
}

void SessionMgr::handle_disconnect(kevent::NetIoHandler *handler) {
	int32 hid = handler->get_handle();
	delete handler;
	AutoLock guard(_disconnect_list_lock);
	_disconnect_list.push_back(hid);
}

void SessionMgr::update()
{
	handle_connect_list();

	handle_disconnect_list();

	for (SessionMap::iterator it = _session.begin(); it != _session.end(); ++it) {
		it->second->update();
	}
}

void SessionMgr::handle_connect_list() {
	HandlerList tmp;
	_connected_list_lock.lock();
	tmp_list = _connected_list;
	_connected_list_lock.unlock();

	for (HandlerList::iterator it = tmp.begin(); it != tmp.end(); +++it) {
		kevent::NetIoHandler *handler = *it;
		Session *session = new Session();
		session->on_connect(handler);

		_session[handler->get_handle()] = session;
	}
}

void SessionMgr::handle_disconnect_list() {
	HandlerIdList tmp;
	_disconnect_list_lock.lock();
	tmp = _disconnect_list;
	_disconnect_list.clear();
	_disconnect_list_lock.unlock();

	for (HandlerIdList::iterator it = tmp.begin(); it != tmp.end(); ++it) {
		SessionMap::iterator its = _session.find(*it);
		if (it == _session.end()) continue;

		Session *s = its->second;
		s->on_disconnect();
		delete s;

		_session.erase(its);
	}
}

