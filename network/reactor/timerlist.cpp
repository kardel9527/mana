#include <assert.h>
#include <time.h>
#include "ihandler.h"
#include "timeutil.h"
#include "timerlist.h"

NMS_BEGIN(kevent)

TimerList::TimerList() : _seed(0), _head(0) { }

TimerList::~TimerList() {
	while (_head) {
		Node *tmp = _head;
		_head = _head->_next;
		sdelete(tmp);
	}

	_head = 0;
}

int32 TimerList::add(uint64 start, uint32 itv, IHandler *handler) {
	assert(handler);
	Node *node = new Node();
	node->_id = ++_seed;
	node->_handler = handler;
	node->_timeout = start ? start : ktimeutil::get_current_time();
	node->_itv = itv;
	
	node->_next = _head;
	_head = node;

	return node->_id;
}

void TimerList::remove(int32 id) {
	Node *prev = _head;
	Node *curr = _head;
	while (curr) {
		if (curr->_id == (uint32)id) {
			if (curr == _head)
				_head = curr->_next;
			else
				prev->_next = curr->_next;
			
			curr->_handler->handle_close();
			delete curr;
			break;
		}

		if (curr != _head)
			prev = curr;
		curr = curr->_next;
	}
}

int32 TimerList::expire_single(uint64 now) {
	now = now ? now : ktimeutil::get_current_time();
	Node *curr = _head;
	Node *prev = _head;
	while (curr) {
		if (curr->_timeout <= now) {
			curr->_handler->handle_timeout();
			if (curr->_itv) {
				curr->_timeout += curr->_itv;
				// put in tail
				Node *tail = curr;
				while (tail->_next) tail = tail->_next;
				if (curr == tail) break;
				if (curr == _head) {
					_head = curr->_next;
				}
				prev->_next = curr->_next;
				tail->_next = curr;
				curr->_next = 0;
			} else {
				remove(curr->_id);
			}
			break;
		}
		if (curr != _head) {
			prev = prev->_next;
		}
		curr = curr->_next;
	}

	return 0;
}

int32 TimerList::expire_all(uint64 now) {
	now = now ? now : ktimeutil::get_current_time();
	Node *curr = _head;
	while (curr) {
		if (curr->_timeout >= now) {
			curr->_handler->handle_timeout();
			if (curr->_itv) {
				// TODO: put in head.
				curr->_timeout += curr->_itv;
			} else {
				remove(curr->_id);
			}
		}
		curr = curr->_next;
	}
	
	return 0;
}

NMS_END // kevent

