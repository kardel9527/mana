#include <assert.h>
#include <time.h>
#include "ihandler.h"
#include "timerlist.h"

NMS_BEGIN(kevent)

TimerList::TimerList() : _head(0), _seed(0) {}

TimerList::~TimerList() {
	while (_head) {
		Node *tmp = _head;
		_head = _head->_next;
		sdelete(tmp);
	}
	
	_head = 0;
	_seed = 0;
}

int32 TimerList::add(timet delay, timet interval, IHandler *handler) {
	assert(handler);
	Node *node = new Node();
	node->_id = ++_seed;
	node->_handler = handler;
	node->_timeout = time(0) + delay + interval;
	node->_interval = interval;
	
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

int32 TimerList::expire_single() {
	Node *curr = _head;
	while (curr) {
		uint64 now = time(0);
		if (curr->_timeout <= now) {
			curr->_handler->handle_timeout();
			if (curr->_interval) {
				// TODO: put in head.
				curr->_timeout += curr->_interval;
			} else {
				remove(curr->_id);
			}
			break;
		}
		curr = curr->_next;
	}
	
	return 0;
}

int32 TimerList::expire_all() {
	Node *curr = _head;
	while (curr) {
		uint64 now = time(0);
		if (curr->_timeout >= now) {
			curr->_handler->handle_timeout();
			if (curr->_interval) {
				// TODO: put in head.
				curr->_timeout += curr->_interval;
			} else {
				remove(curr->_id);
			}
		}
		curr = curr->_next;
	}
	
	return 0;
} 

NMS_END // kcommon
