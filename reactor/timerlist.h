#ifndef __TIMER_LIST_H_
#define __TIMER_LIST_H_
#include "itimermgr.h"

NMS_BEGIN(kcommon)

class TimerList : public ITimerMgr {
	struct Node {
		uint32 _id;
		IHandler *_handler;
		timeval _timeout;
		timeval _interval;
		Node *_next;

		Node() : _id(0), _handler(0), _timeout(0), _interval(0), _next(0) {}
	};

public:
	TimerList();
	~TimerList();

	virtual int32 add(timeval delay, timeval interval, IHandler *handler);

	virtual void remove(int32 id);

	virtual int32 expire_single();

	virtual int32 expire_all();

private:
	TimerList(const TimerList &other) {}
	TimerList& operator = (const TimerList &other) { return *this; }

	uint32 gen_id() { return ++_seed; }

private:
	Node *_head;
	uint32 _seed;
};

NMS_END // kcommon

#endif // __TIMER_LIST_H_