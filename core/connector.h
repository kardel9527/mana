#ifndef __CONNECTOR_H_
#define __CONNECTOR_H_
#include "macros.h"
#include "udt.h"
#include "inetaddr.h"
#include "./reactor/ihandler.h"
#include "fastqueue.h"

NMS_BEGIN(kcore)

class Session;
class CommandHandler;

class Connector : public kevent::IHandler {
public:
	Connector() : _timer_id(0), _interval(15000), _cmd_handler(0) {}
	~Connector();

	int32 open(uint32 timer_interval, CommandHandler *cmd_handler);

	int32 connect(Session *s);

	virtual int32 handle_timeout();

private:
	int32 _timer_id;
	uint32 _interval;
	CommandHandler *_cmd_handler;
	kcommon::FastQueue<Session *, kcommon::DefaultLock> _conn_list;
};

NMS_END // end namespace kcore

#endif // __CONNECTOR_H_

