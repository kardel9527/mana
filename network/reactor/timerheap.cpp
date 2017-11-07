#include "timerheap.h"

NMS_BEGIN(kevent)
TimerHeap::TimerHeap() : _data(0), _capacity(0), _size(0) {}

TimerHeap::~TimerHeap() {
}

int32 TimerHeap::add(timet delay, timet interval, IHandler *handler) {
	return invalid_handle;
}

void TimerHeap::remove(int32 id) {
}

int32 TimerHeap::expire_single() {
	return 0;
}

int32 TimerHeap::expire_all() {
	return 0;
}

NMS_END // end namespace kevent

