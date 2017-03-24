#ifndef __SESSION_MGR_H_
#define __SESSION_MGR_H_
#include <map>
#include "macros.h"
#include "udt.h"
#include "lock.h"
#include "isessionmgr.h"

class Session;

class SessionMgr : public ISessionMgr {
public:
	SessionMgr();
	~SessionMgr();

	virtual void handle_new_connect(kevent::NetIoHandler *handler);

	virtual void handle_disconnect(kevent::NetIoHandler *handler);

	void update();

private:
	void handle_connect_list();

	void handle_disconnect_list();

private:
	typedef std::vector<int32> HandlerIdList;
	typedef std::vector<kevent::NetIoHandler *> HandlerList;
	typedef std::map<int32, Session *> SessionMap;
	SessionMap _session;
	LOCK_DEF(_connected_list_lock);
	HandlerList _connected_list;
	LOCK_DEF(_disconnect_list_lock);
	HandlerIdList _disconnect_list;
};

#endif // __SESSION_MGR_H_
