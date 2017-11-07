#ifndef __SINGLETON_H_
#define __SINGLETON_H_
#include <assert.h>
#include "macros.h"

NMS_BEGIN(kcommon)

template<typename T>
class Singleton {
public:
	Singleton() { assert(!_instance); _instance = static_cast<T *>(this); }
	~Singleton() { }

	template<typename ... Args>
	static void create(const Args& ... args) { new T(args...); }

	static void destroy() { sdelete(_instance); }

	static T* instance() { return _instance; }

private:
	static T *_instance;
};

template<typename T> T *Singleton<T>::_instance = 0;

NMS_END // end namespace kcommon

#endif // __SINGLETON_H_

