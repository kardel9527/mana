#ifndef __TIMER_LIST_H_
#define __TIMER_LIST_H_
#include "macros.h"
#include "udt.h"
#include "itimermgr.h"

NMS_BEGIN(kevent)

class TimerList : public ITimerMgr {
	struct Node {
		uint32 _id;
		IHandler *_handler;
		uint64 _timeout;
		uint64 _itv;
		Node *_next;

		Node() : _id(0), _handler(0), _timeout(0), _itv(0), _next(0) {}
	};

public:
	TimerList();
	~TimerList();

	virtual int32 add(uint64 start, uint32 itv, IHandler *handler);

	virtual void remove(int32 id);

	virtual int32 expire_single(uint64 now);

	virtual int32 expire_all(uint64 now);

private:
	TimerList(const TimerList &other) {}
	TimerList& operator = (const TimerList &other) { return *this; }

private:
	int32 _seed;
	Node *_head;
};

NMS_END // end namespace kevent

#endif // __TIMER_LIST_H_

