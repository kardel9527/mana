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
	Mutex() { pthread_mutex_init(&_mutex, 0); }
	~Mutex() { pthread_mutex_destroy(&_mutex); }
	
	void lock() { pthread_mutex_lock(&_mutex); }
	
	void unlock() { pthread_mutex_unlock(&_mutex); }
	
	void trylock() { pthread_mutex_trylock(&_mutex); }
	
private:
	Mutex(const Mutex &other) {}
	Mutex& operator = (const Mutex &other) { return *this; }

private:
	::pthread_mutex_t _mutex;
};

// a helper class auto release lock in an area
template<typename LOCK = DefaultLock>
class AutoLock : public LOCK {
public:
	AutoLock() { lock(); }
	~AutoLock() { unlock(); }

private:
	AutoLock(const AutoLock &other) {}
	AutoLock& operator = (const AutoLock &other) { return *this; }
};

NMS_END // end namespace kcommon

#endif // __LOCK_H_
