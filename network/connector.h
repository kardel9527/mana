#ifndef __CONNECTOR_H_
#define __CONNECTOR_H_
#include <map>
#include "macros.h"
#include "udt.h"
#include "inetaddr.h"
#include "ihandler.h"

NMS_BEGIN(network)

class NetIoHandler;

class Connector : public kevent::IHandler {
public:
	Connector() { }
	~Connector() { close(); }

	int32 open(int32 itv);
	void close();

	void add_conn_type(int32 type) { _conn_type_map[type] = true; }

	virtual int32 handle_timeout();

private:
	typedef std::map<int32, bool> ConnectionTypeMap;

	ConnectionTypeMap _conn_type_map;
};

NMS_END // end namespace network

#endif // __CONNECTOR_H_

