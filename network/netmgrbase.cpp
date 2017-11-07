#include <assert.h>
#include "netiohandler.h"
#include "netmgrbase.h"

NMS_BEGIN(network)

NetMgrBase::~NetMgrBase() {
	assert(_handler_map.empty() && "logic error, please kickall first.");

	// delete all handler of each type.
	for (int i = 0; i < NHLT_MAX; ++i) {
		NetIoHandler *handler = 0;
		while (_handler_store[i].read(&handler, 1)) {
			sdelete(handler);
		}
	}
}

void NetMgrBase::handle_new_connect(NetIoHandler *handler) {
	assert(handler && "empty handler!");
	_handler_operation_queue.lock();
	_handler_operation_queue.push(HandlerOperation(HOT_CONNECT, handler));
	_handler_operation_queue.unlock();
}

void NetMgrBase::handle_disconnect(NetIoHandler *handler) {
	assert(handler && "empty handler!");
	_handler_operation_queue.lock();
	_handler_operation_queue.push(HandlerOperation(HOT_DISCONNECT, handler));
	_handler_operation_queue.unlock();
}

void NetMgrBase::handle_connect_failed(NetIoHandler *handler) {
	assert(handler && "empty handler!");
	_handler_operation_queue.lock();
	_handler_operation_queue.push(HandlerOperation(HOT_CONNECT_FAIL, handler));
	_handler_operation_queue.unlock();
}

void NetMgrBase::update() {
	update_handler_operation();

	update_all_handler();
}

void NetMgrBase::kickall() {
	// process all left msg.
	update_all_handler();

	// send kickoff request
	for (HandlerMap::iterator it = _handler_map.begin(); it != _handler_map.end(); ++it) {
		it->second->kickoff();
	}

	// wait all handler kicked.
	while (!_handler_map.empty()) {
		update_handler_operation();
	}
}

void NetMgrBase::put_handler(NetIoHandler *handler) {
	assert(handler->type() >= NHLT_CLIENT && handler->type() < NHLT_MAX && "error handler type.");
	kcommon::FastQueue<NetIoHandler *> &handler_store = _handler_store[handler->type()];
	handler_store.lock();
	handler_store.push(handler);
	handler_store.unlock();
}

NetIoHandler* NetMgrBase::get_handler(int32 type) {
	assert(type >= NHLT_CLIENT && type < NHLT_MAX && "error handler type.");
	NetIoHandler *ret = 0;
	kcommon::FastQueue<NetIoHandler *> &handler_store = _handler_store[type];
	handler_store.lock();
	if (!handler_store.empty()) {
		ret = handler_store.pop();
	}
	handler_store.unlock();

	return ret;
}

void NetMgrBase::connect_fail(NetIoHandler *handler) {
	put_handler(handler);
}

void NetMgrBase::update_handler_operation() {
	while (true) {
		HandlerOperation operation;
		_handler_operation_queue.lock();
		if (_handler_operation_queue.empty()) {
			_handler_operation_queue.unlock();
			break;
		}
		operation = _handler_operation_queue.pop();
		_handler_operation_queue.unlock();

		assert(operation._handler && "empty handler!");
		switch (operation._type) {
		case HOT_CONNECT:
			assert(_handler_map.find(operation._handler->id()) == _handler_map.end() && "handler already existed.");
			_handler_map[operation._handler->id()] = operation._handler;
			operation._handler->handle_connect();
			break;
		case HOT_DISCONNECT:
			assert(_handler_map.find(operation._handler->id()) != _handler_map.end() && "handler not exist.");
			_handler_map.erase(operation._handler->id());
			operation._handler->handle_disconnect();
			// TODO: how to handler the left packet and to be send data when handler closed ?
			put_handler(operation._handler);
			break;
		case HOT_CONNECT_FAIL:
			connect_fail(operation._handler);
			break;
		default:
			assert(false && "unknown handler operation type.");
			break;
		}
	}
}

void NetMgrBase::update_all_handler() {
	for (HandlerMap::iterator it = _handler_map.begin(); it != _handler_map.end(); ++it) {
		it->second->update_base();
	}
}

NMS_END // end namespace network

