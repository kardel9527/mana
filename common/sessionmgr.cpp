#include "netiohandler.h"
#include "session.h"
#include "sessionmgr.h"

NMS_BEGIN(kcommon)

SessionMgr::SessionMgr() {}

SessionMgr::~SessionMgr() {}

void SessionMgr::handle_new_connect(Session *s) {
	AutoLock<LockType> guard(_connected);
	_connected.push(s);
}

void SessionMgr::handle_disconnect(int32 hid) {
	AutoLock<LockType> guard(_disconnected);
	_disconnected.push(hid);
}

void SessionMgr::update()
{
	handle_connect_list();

	handle_disconnect_list();

	update_session();
}

Session* SessionMgr::create(int32 type) {
	return new Session();
}

void SessionMgr::destroy(Session *s) {
	delete s;
}

// TODO: when handle connect, the io handler was already closed
void SessionMgr::handle_connect_list() {
	AutoLock<LockType> guard(_connected);
	while (!_connected.empty()) {
		Session *session = _connected.pop();

		session->handle_connect();
		_session[session->id()] = session;
	}
}

void SessionMgr::handle_disconnect_list() {
	AutoLock<LockType> guard(_disconnected);
	while (!_disconnected.empty()) {
		int32 hid = _disconnected.pop();

		SessionMap::iterator it = _session.find(hid);
		if (it != _session.end()) {
			Session *session = it->second;
			_session.erase(it);

			session->handle_disconnect();

			destroy(session);
		} else {
			// TODO: error ocurred.
		}
	}
}

void SessionMgr::update_session() {
	for (SessionMap::iterator it = _session.begin(); it != _session.end(); ++it) {
		it->second->update();
	}
}

NMS_END // end namespace kcommon

