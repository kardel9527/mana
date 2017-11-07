#ifndef __TIMER_HEAP_H_
#define __TIMER_HEAP_H_
#include "macros.h"
#include "udt.h"
#include "itimermgr.h"

NMS_BEGIN(kevent)

class TimerHeap : public ITimerMgr {
	struct Node {
		uint32 _id;
		IHandler *_handler;
		timet _timeout;
		timet _interval;

		Node() : _id(0), _handler(0), _timeout(0), _interval(0) {}
	};

public:
	TimerHeap();
	~TimerHeap();

	virtual int32 add(timet delay, uint64 itv, IHandler *handler);

	virtual void remove(int32 id);

	virtual int32 expire_single();

	virtual int32 expire_all();

private:
	TimerHeap(const TimerHeap &other) {}
	TimerHeap& operator = (const TimerHeap &other) { return *this; }

private:
	Node *_data;
	uint32 _capacity;
	uint32 _size;
};

NMS_END // kevent

#endif // __TIMER_HEAP_H_

