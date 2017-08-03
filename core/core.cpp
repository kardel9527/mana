#include <pthread.h>
#include "./reactor/epollreactor.h"
#include "session.h"
#include "netiohandler.h"
#include "sessionmgr.h"
#include "core.h"

#define EVENT_TIMEOUT 10 // default event timeout in ms.

NMS_BEGIN(kcore)

Core::Core() : _io_thread(0), _s_mgr(0) {}

Core::~Core() {
	sdelete(_s_mgr);
}

int32 Core::open(uint32 max_connection, SessionMgr *mgr) {
	_s_mgr = mgr;
	kevent::IReactor *reactor = new kevent::EpollReactor();
	int32 ret = reactor->open(max_connection, 0, 0);
	if (ret == -1) return ret;

	_reactor.open(reactor, true);
	
	_connector.reactor(&_reactor);
	_connector.open(15000, &_cmd_handler);

	_cmd_handler.reactor(&_reactor);
	_cmd_handler.open(&_connector, EVENT_TIMEOUT);
	
	return 0;
}

int32 Core::open(const kcommon::InetAddr &addr, uint32 max_connection, SessionMgr *mgr) {
	int32 ret = open(max_connection, mgr);
	if (ret == -1) return ret;

	_acceptor.reactor(&_reactor);
	_acceptor.session_mgr(_s_mgr);
	return _acceptor.open(addr, &_cmd_handler);
}

void Core::update() {
	_s_mgr->update();
}

int32 Core::start() {
	// begin io thread.
	if (::pthread_create((pthread_t *)&_io_thread, 0, &(Core::io_routine), this) == -1)
		return -1;

	return 0;
}

void Core::stop() {
	_reactor.end_event_loop();
	::pthread_join(_io_thread, 0);
}

void Core::connect(const kcommon::InetAddr &addr, int32 session_type) {
	Session *s = _s_mgr->create(session_type);
	s->mgr(_s_mgr);
	s->io_handler()->redirect(-1, addr);

	_cmd_handler.add(CommandHandler::Command(CommandHandler::CT_CONNECT, s));
}

void* Core::io_routine(void *arg) {
	Core *svc = (Core *)arg;
	svc->_reactor.begin_event_loop(EVENT_TIMEOUT);
	return 0;
}

NMS_END // end namespace kcore

