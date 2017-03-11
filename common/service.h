#ifndef __SERVICE_H_
#define __SERVICE_H_
#include "macros.h"
#include "udt.h"
#include "acceptor.h"
#include "connector.h"
#include "reactor.h"
#include "singleton.h"

// : TODO: add a send thread
class Service : public kcommon::Singleton<Service> {
public:
	Service();
	~Service();

	int32 open(uint32 max_connection, kcommon::SessionMgr *mgr);

	int32 open(const kcommon::InetAddr &addr, uint32 max_connection, kcommon::SessionMgr *mgr);

	void update();

	int32 start();

	void stop();

	void connect(const kcommon::InetAddr &addr, int32 session_type);

	static void* io_routine(void *arg);

private:
	Service(const Service &other) {}
	Service& operator = (const Service &rh) { return *this; }

private:
	unsigned long _io_thread;
	kevent::Reactor _reactor;
	kcommon::Acceptor _acceptor;
	kcommon::Connector _connector;
	kcommon::SessionMgr *_s_mgr;
};

#endif // __SERVICE_H_

