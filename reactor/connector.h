#ifndef __CONNECTOR_H_
#define __CONNECTOR_H_
#include <vector>
#include "macros.h"
#include "udt.h"
#include "inetaddr.h"
#include "ihandler.h"

NMS_BEGIN(kevent)

class Connector : public IHandler {
	struct ConnectInfo {
		int32 _fd;
		InetAddr _inet_addr;

		ConnectInfo() : _fd(-1) {}
		ConnectInfo(int32 fd, const InetAddr &addr) : _fd(fd), _inet_addr(addr) {}
	};

	typedef std::vector<ConnectInfo> ToBeConnectList;

public:
	Connector() : _timer_id(0), _interval(15){}
	~Connector();

	int32 open(uint32 timer_interval);

	int32 connect(const InetAddr &addr);

	virtual int32 handle_timeout();

private:
	int32 connect(const ConnectInfo &conn);

private:
	int32 _timer_id;
	uint32 _interval;
	ToBeConnectList _conn_list;
};

NMS_END // end namespace kevent

#endif // __CONNECTOR_H_

