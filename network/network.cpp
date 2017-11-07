#include "netmgrbase.h"
#include "acceptor.h"
#include "connector.h"
#include "reactor.h"
#include "netiohandler.h"
#include "timerlist.h"
#include "epollreactor.h"
#include "timerlist.h"
#include "logger.h"
#include "network.h"

NMS_BEGIN(network)

NetWork::NetWork(NetMgrBase *net_mgr) : _net_mgr(net_mgr) {
	_connector = new Connector();
	_main_reactor = new Reactor();
	_main_reactor->impl(new kevent::EpollReactor());
	_main_reactor->net_mgr(net_mgr);
	_connector->reactor(_main_reactor);
}

NetWork::~NetWork() {
	sdelete(_net_mgr);
	for (auto it = _acceptor.begin(); it != _acceptor.end(); ++it) { sdelete(*it); }
	sdelete(_connector);
	sdelete(_main_reactor);
}

int32 NetWork::register_acceptor(int32 type, const kcommon::InetAddr &addr) {
	Acceptor *acceptor = new Acceptor(type, addr);
	acceptor->reactor(_main_reactor);
	_acceptor.push_back(acceptor);
	return 0;
}

void NetWork::add_server_handler(NetIoHandler *handler) {
	_net_mgr->put_handler(handler);
}

void NetWork::add_client_handler(NetIoHandler *handler) {
	_net_mgr->put_handler(handler);
	_connector->add_conn_type(handler->type());
}

int32 NetWork::start() {
	// start reactor first.
	int32 ret = _main_reactor->impl()->open(new kevent::TimerList(), 0);
	if (ret) {
		LOG_ERROR("main reactor start failed.");
		return ret;
	}

	// start acceptor.
	for (auto it = _acceptor.begin(); it != _acceptor.end(); ++it) {
		Acceptor *acceptor = *it;
		ret = acceptor->open();
		if (ret) {
			stop();
			return ret;
		}
	}

	// start connector
	ret = _connector->open(10000);
	if (ret) {
		LOG_ERROR("connector start failed.");
		stop();
		return ret;
	}

	// run reactor.
	ret = _main_reactor->begin_event_loop();
	if (ret) {
		LOG_ERROR("reactor run failed.");
		stop();
		return ret;
	}

	LOG_INFO("network now running.");
	return 0;
}

void NetWork::stop() {
	// kick off all net handler
	_net_mgr->kickall();

	// stop all acceptor.
	for (auto it = _acceptor.begin(); it != _acceptor.end(); ++it) {
		(*it)->close();
	}

	// close connector.
	_connector->close();

	// stop reactor finally.
	_main_reactor->end_event_loop();
}

void NetWork::update() {
	_net_mgr->update();
}

NMS_END // end namepsace network

