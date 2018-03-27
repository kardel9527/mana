#include <assert.h>
#include <string.h>
#include "reactor.h"
#include "stringutil.h"
#include "netiohandler.h"
#include "netmgrbase.h"
#include "connector.h"

NMS_BEGIN(network)

int32 Connector::open(int32 itv) {
	return register_timer_event(itv);
}

void Connector::close() {
	if (handle() == invalid_handle) return ;
	unregister_timer_event();
}

int32 Connector::handle_timeout() {
	for (ConnectionTypeMap::iterator it = _conn_type_map.begin(); it != _conn_type_map.end(); ++it) {
		while (NetIoHandler *handler = reactor()->net_mgr()->get_handler(it->first)) {
			handler->reactor(reactor());
			int32 ret = handler->reconnect();
			if (ret) {
				reactor()->net_mgr()->handle_connect_failed(handler);
			} else {
				handler->active(true);
				handler->id(handler->handle());
				reactor()->net_mgr()->handle_new_connect(handler);
			}
		}
	}

	return 0;
}

NMS_END // end namespace network

