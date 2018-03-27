#ifndef __NET_MGR_BASE_H_
#define __NET_MGR_BASE_H_
#include <map>
#include "macros.h"
#include "udt.h"
#include "lock.h"
#include "fastqueue.h"
#include "netiohandler.h"

NMS_BEGIN(network)

class NetMgrBase {
	enum HandlerOperationType { HOT_NONE, HOT_CONNECT, HOT_DISCONNECT, HOT_CONNECT_FAIL };
	struct HandlerOperation {
		HandlerOperationType _type;
		NetIoHandler *_handler;

		HandlerOperation() : _type(HOT_NONE), _handler(0) {}
		HandlerOperation(HandlerOperationType type, NetIoHandler *handler) : _type(type), _handler(handler) {}
		~HandlerOperation() { _type = HOT_NONE; _handler = 0; }
	};

public:
	NetMgrBase() : _handler_operation_queue(128) {}
	virtual ~NetMgrBase();

	void handle_new_connect(NetIoHandler *handler);

	void handle_disconnect(NetIoHandler *handler);

	void handle_connect_failed(NetIoHandler *handler);

	void update();

	void kickall();

	void wait_all_kicked();

	// redefine these two functions if you want to alloc handler dynamic.
	// notice that default action is put/get handler into/from a list of each handler type.
	virtual void put_handler(NetIoHandler *handler);
	virtual NetIoHandler* get_handler(int32 type);

	// redefine this function if you want to do something with a connect failed handler
	// notice that default action is retry later.
	virtual void connect_fail(NetIoHandler *handler);

private:
	NetMgrBase(const NetMgrBase &other) {}
	NetMgrBase& operator = (const NetMgrBase &rh) { return *this; }

private:
	void update_handler_operation();

	void update_all_handler();

private:
	typedef std::map<int32, NetIoHandler *> HandlerMap;
	HandlerMap _handler_map;

	kcommon::FastQueue<NetIoHandler *> _handler_store[NHLT_MAX];
	kcommon::FastQueue<HandlerOperation> _handler_operation_queue;
};

NMS_END // end namespace network

#endif // __NET_MGR_BASE_H_

