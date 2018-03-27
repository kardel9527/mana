#include <assert.h>
#include "ihandler.h"
#include "petty.h"
#include "timeutil.h"
#include "timerheap.h"

NMS_BEGIN(kevent)
TimerHeap::TimerHeap() {}

TimerHeap::~TimerHeap() {
}

int32 TimerHeap::add(uint64 start, uint32 itv, IHandler *handler) {
	_data.push_back(Node());

	Node &node = _data[_data.size() - 1];
	node._id = (int32)_data.size();
	node._handler = handler;
	node._timeout = start ? start : ktimeutil::get_current_time();
	node._interval = itv;

	filter_up(_data.size() - 1);

	return node._id;
}

void TimerHeap::remove(int32 id) {
	uint32 size = _data.size();

	for (uint32 i = 0; i < size; ++i) {
		if (_data[i]._id != id) continue;

		_data[i] = _data[_data.size() - 1];
		_data.pop_back();
		filter_down(i);
		break;
	}
}

int32 TimerHeap::expire_single(uint64 now) {
	now = now ? now : ktimeutil::get_current_time();
	if (_data.empty()) return 0;

	Node &node = _data[0];
	if (node._timeout > now) return 0;

	node._handler->handle_timeout();
	if (!node._interval) {
		remove(node._id);
	} else {
		node._timeout += node._interval;
		filter_down(0);
	}


	return 1;
}

int32 TimerHeap::expire_all(uint64 now) {
	int ret = 0;
	now = now ? now : ktimeutil::get_current_time();

	while (expire_single(now)) ret ++;

	return ret;
}

uint32 TimerHeap::expire_time(uint64 now, uint32 itv) {
	if (_data.empty()) return itv;

	uint32 ret = kcommon::max((int64)0, (int64)(now - _data[0]._timeout));
	return kcommon::min(ret, itv);	
}

void TimerHeap::filter_up(uint32 idx) {
	assert(idx < _data.size());

	Node node = _data[idx];
	while (idx > 0) {
		uint32 parent = (idx - 1) >> 1;
		if (!(_data[parent] > node)) break;

		_data[idx] = _data[parent];
		idx = parent;
	}

	assert(idx >= 0);
	_data[idx] = node;
}

void TimerHeap::filter_down(uint32 idx) {
	assert(idx < _data.size());

	Node node = _data[idx];
	while (true) {
		uint32 lchild = (idx << 1) + 1;
		uint32 rchild = lchild + 1;
		uint32 child = 0;

		if (rchild < _data.size()) {
			child = (_data[lchild] > _data[rchild]) ? lchild : rchild;
		} else if (lchild < _data.size()) {
			child = lchild;
		} else {
			break;
		}

		assert(child < _data.size() && "logic error.");

		if (!(_data[idx] > _data[child])) break;

		_data[idx] = _data[child];
		idx = child;
	}

	_data[idx] = node;
}

NMS_END // end namespace kevent

