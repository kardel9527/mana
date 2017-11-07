#include <pthread.h>
#include <unistd.h>
#include <assert.h>
#include "commandhandler.h"
#include "reactor.h"

#define ITV 5 // default 5 ms each round.

NMS_BEGIN(network)
Reactor::Reactor() : _thread_id(0), _impl(0), _net_mgr(0) {
	_cmd_handler = new CommandHandler();
	_cmd_handler->reactor(this);
}

Reactor::Reactor(kevent::IReactor *reactor) : _thread_id(0), _impl(reactor), _net_mgr(0) {
	_cmd_handler = new CommandHandler();
	_cmd_handler->reactor(this);
}

Reactor::~Reactor() {
	sdelete(_cmd_handler);
	sdelete(_impl);
	_net_mgr = 0;
}

int32 Reactor::begin_event_loop() {
	impl()->active(true);

	int32 ret = _cmd_handler->open(1000);
	if (ret) {
		sdelete(_cmd_handler);
		return ret;
	}

	ret = ::pthread_create((pthread_t *)&_thread_id, 0, &Reactor::io_routin, this);
	if (ret) {
		_thread_id = 0;
		sdelete(_cmd_handler);
		return ret;
	}

	return ret;
}

void Reactor::end_event_loop() {
	impl()->active(false);

	// wait child thread exit.
	::pthread_join((pthread_t)_thread_id, 0);

	// close reactor impl
	impl()->close();
}

void Reactor::add_cmd(int32 type, NetIoHandler *handler) {
	_cmd_handler->add(CommandHandler::Command((CommandHandler::CommandType)type, handler));
}

void* Reactor::io_routin(void *arg) {
	Reactor *reactor = (Reactor *)arg;
	assert(reactor && "null pointer!");

	while (reactor->impl()->active()) {
		reactor->impl()->handle_events(ITV);
	}

	// handle the rest cmd.
	while (reactor->_cmd_handler->handle_timeout()) ;

	return 0;
}

NMS_END // end namespacke network.

