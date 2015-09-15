#ifndef __SINGLETON_H_
#define __SINGLETON_H_
#include "macros.h"

NMS_BEGIN(kcommon)

template<typename T>
class Singlegon {
	static void create() { if (!_instance) _instance = new T(); }

	static void destroy() { sdelete(_instance); }

	T* instance() { return _instance; }

private:
	static T *_instance;
};

template<typename T> T *Singlegon<T>::_instance = 0;

NMS_END // end namespace kcommon

#endif // __SINGLETON_H_

