#include "netiohandler.h"
#include "session.h"
#include "sessionmgr.h"

NMS_BEGIN(kcommon)

SessionMgr::SessionMgr() {}

SessionMgr::~SessionMgr() {}

void SessionMgr::handle_new_connect(Session *s) {
	AutoLock<LockType> guard(_connected);
	_connected.write(s);
}

void SessionMgr::handle_disconnect(Session *s) {
	AutoLock<LockType> guard(_disconnected);
	_disconnected.write(s);
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
	uint32 size = _connected.avail();
	for (uint32 i = 0; i < size; ++i) {
		Session *session = 0;
		_connected.read(session);

		session->handle_connect();

		_session[session->id()] = session;
	}
}

void SessionMgr::handle_disconnect_list() {
	AutoLock<LockType> guard(_disconnected);
	uint32 size = _disconnected.avail();
	for (uint32 i = 0; i < size; ++i) {
		Session *session = 0;
		_connected.read(session);

		SessionMap::iterator it = _session.find(session->id());
		if (it != _session.end()) _session.erase(it);

		session->handle_disconnect();

		destroy(session);
	}
}

void SessionMgr::update_session() {
	for (SessionMap::iterator it = _session.begin(); it != _session.end(); ++it) {
		it->second->update();
	}
}

NMS_END // end namespace kcommon

