#ifndef __NETWORK_H_
#define __NETWORK_H_
#include <vector>
#include "udt.h"
#include "macros.h"
#include "inetaddr.h"

NMS_BEGIN(network)

class NetMgrBase;
class Acceptor;
class Connector;
class Reactor;
class NetIoHandler;

class NetWork {
public:
	NetWork(NetMgrBase *net_mgr);
	~NetWork();

	int32 register_acceptor(int32 type, const kcommon::InetAddr &addr);

	// pre alloc handler act as server/client.
	void add_server_handler(NetIoHandler *handler);
	void add_client_handler(NetIoHandler *handler);

	int32 start();
	void stop();

	void update();

private:
	NetWork(const NetWork &other) {}
	NetWork& operator = (const NetWork &other) { return *this; }

private:
	NetMgrBase *_net_mgr;
	std::vector<Acceptor *> _acceptor;
	Connector *_connector;
	Reactor *_main_reactor;
};

NMS_END // end namespace network

#endif // __NETWORK_H_

