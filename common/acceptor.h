#ifndef __ACCEPTOR_H_
#define __ACCEPTOR_H_
#include "udt.h"
#include "macros.h"
#include "inetaddr.h"
#include "ihandler.h"

NMS_BEGIN(kcommon)

class SessionMgr;

class Acceptor : public kevent::IHandler {
public:
	Acceptor() : _sock(-1), _session_mgr(0) {}
	virtual ~Acceptor();

	void session_mgr(SessionMgr *mgr) { _session_mgr = mgr; }

	int32 open(const kcommon::InetAddr &addr);

	virtual int32 get_handle() { return _sock; }

	virtual int32 handle_input();

	virtual int32 handle_close();

private:
	Acceptor(const Acceptor &rh) {}
	Acceptor& operator = (const Acceptor &rh) { return *this; }

private:
	int32 _sock;
	SessionMgr *_session_mgr;
};

NMS_END // end namespace kcommon

#endif // __ACCEPTOR_H_
