#include <pthread.h>
#include "epollreactor.h"
#include "session.h"
#include "netiohandler.h"
#include "sessionmgr.h"
#include "service.h"

Service::Service() : _io_thread(0), _s_mgr(0) {}

Service::~Service() {
	sdelete(_s_mgr);
}

int32 Service::open(uint32 max_connection, kcommon::SessionMgr *mgr) {
	_s_mgr = mgr;
	kevent::IReactor *reactor = new kevent::EpollReactor();
	int32 ret = reactor->open(max_connection, 0, 0);
	if (ret == -1) return ret;

	_reactor.open(reactor, true);
	
	_connector.reactor(&_reactor);
	ret = _connector.open(15);
	
	return 0;
}

int32 Service::open(const kcommon::InetAddr &addr, uint32 max_connection, kcommon::SessionMgr *mgr) {
	int32 ret = open(max_connection, mgr);
	if (ret == -1) return ret;

	_acceptor.reactor(&_reactor);
	_acceptor.session_mgr(_s_mgr);
	ret = _acceptor.open(addr);
	if (ret == -1) return ret;

	_connector.reactor(&_reactor);
	ret = _connector.open(15);

	return ret;
}

void Service::update() {
	_s_mgr->update();
}

int32 Service::start() {
	// begin io thread.
	return ::pthread_create((pthread_t *)&_io_thread, 0, &(Service::io_routine), this) == 0 ? 0 : -1;
}

void Service::stop() {
	_reactor.end_event_loop();
	::pthread_join(_io_thread, 0);
}

void Service::connect(const kcommon::InetAddr &addr, int32 session_type) {
	kcommon::Session *s = _s_mgr->create(session_type);
	s->mgr(_s_mgr);
	s->io_handler()->redirect(-1, addr);
	_connector.connect(s);
}

void* Service::io_routine(void *arg) {
	Service *svc = (Service *)arg;
	svc->_reactor.begin_event_loop(20);
	return 0;
}

