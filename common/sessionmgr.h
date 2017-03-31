#ifndef __SESSION_MGR_H_
#define __SESSION_MGR_H_
#include <map>
#include "macros.h"
#include "udt.h"
#include "lock.h"
#include "FastQueue.h"

NMS_BEGIN(kcommon)

class Session;

class SessionMgr {
public:
	SessionMgr();
	~SessionMgr();

	void handle_new_connect(Session *s);

	void handle_disconnect(Session *s);

	void update();

	virtual Session* create(int32 session_type);

	virtual void destroy(Session *s);

private:
	SessionMgr(const SessionMgr &other) {}
	SessionMgr& operator = (const SessionMgr &rh) { return *this; }

private:
	void handle_connect_list();

	void handle_disconnect_list();

	void update_session();

private:
	typedef std::map<int32, Session *> SessionMap;
	SessionMap _session;

	kcommon::FastQueue<Session *> _disconnected;
	kcommon::FastQueue<Session *> _connected;
};

NMS_END // end namespace kcommon

#endif // __SESSION_MGR_H_

