#ifndef __TIMER_HEAP_H_
#define __TIMER_HEAP_H_
#include <vector>
#include "macros.h"
#include "udt.h"
#include "itimermgr.h"

NMS_BEGIN(kevent)

class TimerHeap : public ITimerMgr {
	struct Node {
		int32 _id;
		IHandler *_handler;
		uint64 _timeout;
		uint64 _interval;

		bool operator > (const Node &other) const { return this->_timeout > other._timeout; }

		Node() : _id(0), _handler(0), _timeout(0), _interval(0) {}
	};

public:
	TimerHeap();
	~TimerHeap();

	virtual int32 add(uint64 start, uint32 itv, IHandler *handler);

	virtual void remove(int32 id);

	virtual int32 expire_single(uint64 now);

	virtual int32 expire_all(uint64 now);

	virtual uint32 expire_time(uint64 now, uint32 itv);

private:
	void filter_up(uint32 idx);

	void filter_down(uint32 idx);

private:
	TimerHeap(const TimerHeap &other) {}
	TimerHeap& operator = (const TimerHeap &other) { return *this; }

private:
	std::vector<Node> _data;
};

NMS_END // kevent

#endif // __TIMER_HEAP_H_

