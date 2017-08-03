#ifndef __CORE_H_
#define __CORE_H_
#include "macros.h"
#include "udt.h"
#include "acceptor.h"
#include "connector.h"
#include "commandhandler.h"
#include "./reactor/reactor.h"
#include "singleton.h"

NMS_BEGIN(kcore)
class SessionMgr;

class Core : public kcommon::Singleton<Core> {
public:
	Core();
	~Core();

	int32 open(uint32 max_connection, SessionMgr *mgr);

	int32 open(const kcommon::InetAddr &addr, uint32 max_connection, SessionMgr *mgr);

	void update();

	int32 start();

	void stop();

	void connect(const kcommon::InetAddr &addr, int32 session_type);

	static void* io_routine(void *arg);

private:
	Core(const Core &other) {}
	Core& operator = (const Core &rh) { return *this; }

private:
	uint64 _io_thread;
	kevent::Reactor _reactor;
	Acceptor _acceptor;
	Connector _connector;
	CommandHandler _cmd_handler;
	SessionMgr *_s_mgr;
};

NMS_END // end namespace kcore

#endif // __CORE_H_

