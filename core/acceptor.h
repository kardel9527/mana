#ifndef __ACCEPTOR_H_
#define __ACCEPTOR_H_
#include "udt.h"
#include "macros.h"
#include "inetaddr.h"
#include "./reactor/ihandler.h"

NMS_BEGIN(kcore)

class SessionMgr;
class CommandHandler;

class Acceptor : public kevent::IHandler {
public:
	Acceptor() : _listen_fd(-1), _cmd_handler(0), _session_mgr(0) {}
	virtual ~Acceptor();

	void session_mgr(SessionMgr *mgr) { _session_mgr = mgr; }

	int32 open(const kcommon::InetAddr &addr, CommandHandler *cmd_handler);

	virtual int32 get_handle() { return _listen_fd; }

	virtual int32 handle_input();

	virtual int32 handle_close();

private:
	Acceptor(const Acceptor &rh) {}
	Acceptor& operator = (const Acceptor &rh) { return *this; }

private:
	int32 _listen_fd;
	CommandHandler *_cmd_handler;
	SessionMgr *_session_mgr;
};

NMS_END // end namespace kcore

#endif // __ACCEPTOR_H_
