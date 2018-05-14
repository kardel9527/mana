#ifndef __LOCK_H_
#define __LOCK_H_
#include <pthread.h>
#include <errno.h>
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
		return ::pthread_mutex_trylock(&_mutex) == EBUSY ? false : true;
	}
	
private:
	Mutex(const Mutex &other) {}
	const Mutex& operator = (const Mutex &other) { return *this; }

private:
	::pthread_mutex_t _mutex;
};

class Spin {
public:
	Spin() { ::pthread_spin_init(&_spin, 0); }
	~Spin() { ::pthread_spin_destroy(&_spin); }

	void lock() { ::pthread_spin_lock(&_spin); }

	void unlock() { ::pthread_spin_unlock(&_spin); }

	bool trylock() {
		return ::pthread_spin_trylock(&_spin) == EBUSY ? false : true;
	}

private:
	pthread_spinlock_t _spin;
};

#ifdef KMT
#define LockType kcommon::Spin
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
