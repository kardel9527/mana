#ifndef __FAST_QUEUE_H_
#define __FAST_QUEUE_H_
#include "ringbuffer.h"

NMS_BEGIN(kcommon)

template<typename T, typename Lock = LockType>
class FastQueue : public RingBuffer<T, Lock> {
public:
	FastQueue() {}
	FastQueue(uint32 capacity) : RingBuffer<T, Lock>(capacity) {}
	FastQueue(const FastQueue &other) { *this = other; }
	FastQueue& operator = (const FastQueue &rh) { RingBuffer<T, Lock>::operator = (rh); return *this; }
	~FastQueue() {}

	uint32 size() { return RingBuffer<T, Lock>::avail(); }

	bool empty() { return RingBuffer<T, Lock>::avail() == 0; }

	// todo
	T pop() { T ret; RingBuffer<T, Lock>::read(ret); return ret; }

	void push(const T &t) { RingBuffer<T, Lock>::write(t); }
};

NMS_END // end namespace kcommon

#endif // __FAST_QUEUE_H_
