#ifndef __CONNECTOR_H_
#define __CONNECTOR_H_
#include "macros.h"
#include "udt.h"
#include "inetaddr.h"
#include "ihandler.h"
#include "ringbuffer.h"

NMS_BEGIN(kcommon)

class Connector : public kevent::IHandler {
public:
	Connector() : _timer_id(0), _interval(15){}
	~Connector();

	int32 open(uint32 timer_interval);

	int32 connect(Session *s);

	virtual int32 handle_timeout();

private:
	int32 _timer_id;
	uint32 _interval;
	kcommon::RingBuffer<Session *> _conn_list;
};

NMS_END // end namespace kcommon

#endif // __CONNECTOR_H_

