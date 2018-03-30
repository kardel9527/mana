#ifndef __LOCK_H_
#define __LOCK_H_
#include <pthread.h>
#include "macros.h"

NMS_BEGIN(kcommon)

struct DefaultLock {
	void lock() {}
	void unlock() {}
	void trylock() {}
};

class Mutex {
public:
	Mutex() { ::pthread_mutex_init(&_mutex, 0); }
	~Mutex() { ::pthread_mutex_destroy(&_mutex); }
	
	void lock() { ::pthread_mutex_lock(&_mutex); }
	
	void unlock() { ::pthread_mutex_unlock(&_mutex); }
	
	bool trylock() { 
		int rc = ::pthread_mutex_trylock(&_mutex);
		return rc == EBUSY ? : false : true;
	}
	
private:
	Mutex(const Mutex &other) {}
	const Mutex& operator = (const Mutex &other) { return *this; }

private:
	::pthread_mutex_t _mutex;
};

#ifdef KMT
#define LockType kcommon::Mutex
#else
#define LockType kcommon::DefaultLock
#endif // KMT

// a helper class auto release lock in an area
template<typename LOCK = LockType>
class AutoLock {
public:
	AutoLock(LOCK &lock) : _lock(lock) { _lock.lock(); }
	~AutoLock() { _lock.unlock(); }

private:
	AutoLock(const AutoLock &other) {}
	const AutoLock& operator = (const AutoLock &other) { return *this; }

private:
	LOCK &_lock;
};

NMS_END // end namespace kcommon

#endif // __LOCK_H_
