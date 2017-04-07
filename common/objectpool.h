#ifndef __OBJECT_POOL_H_
#define __OBJECT_POOL_H_
#include <string.h>
#include <asset.h>
#include "macros.h"
#include "udt.h"
#include "petty.h"
#include "singleton.h"
#include "lock.h"

NMS_BEGIN(kmem)

template<typename Lock = LockType>
class ObjectBuffer : public Lock {
	union Block { Block *_next; char *_data; Block() : _next(0), _data(0); };
public:
	ObjectBuffer() : _size(0), _free(0) {}
	ObjectBuffer(uint32 size) : _size(size), _free(0) {}
	~ObjectBuffer() { while (_free) { Block *tmp = _free; _free = _free->_next; sfree(tmp); } }

	void set_block_size(uint32 size) { _size = size; }

	void* alloc() {
		Lock::lock();
		if (Block *ret = _free) {
			_free = _free->_next;
			Lock::unlock();
			return ret;
		} else {
			Lock::unlock();
			return ::malloc(_size);
		}
	}

	void dealloc(void *ptr) {
		Block *node = (Block *)ptr;
		Lock::lock();
		node->_next = _free;
		_free = node;
		Lock::unlock();
	}

private:
	ObjectBuffer(const ObjectBuffer &other) {}
	ObjectBuffer& operator = (const ObjectBuffer &rh) { return *this; }

private:
	uint32 _size;
	Block *_free;
};

// object pool, default from 4 byte ~ 8 mega bytes.
// notice that each ptr will take 1 byte to save the align used for dealloc
template<uint32 MIN_ALIGN = 2, uint32 MAX_ALIGN = 23, typename Lock = LockType>
class ObjectPool : public Singleton<ObjectPool> {
public:
	ObjectPool() { for (uint32 i = MIN_ALIGN; i <= MAX_ALIGN; ++i) _pool[i - MIN_ALIGN].set_block_size((1u << i) + 1); }
	~ObjectPool() {}

	void* alloc(uint32 size) {
		assert(size > 0);
		uint32 align = kcommon::pow_of_two_align(size);
		assert(align >= MIN_ALIGN && align <= MAX_ALIGN);
		// save align
		char *ret = (char *)_pool[align - MIN_ALIGN].alloc();
		ret[0] = (char)align;
		return (void *)(ret + 1);
	}

	void dealloc(void *ptr) {
		char *addr = ((char *)(ptr) - 1);
		uint32 align = addr[0];
		assert(align >= MIN_ALIGN && align <= MAX_ALIGN);
		_pool[align - MIN_ALIGN].dealloc(addr);
	}

private:
	ObjectPool(const ObjectPool &other) {}
	ObjectPool& operator = (const ObjectPool &rh) { return *this; }

private:
	ObjectBuffer _pool[MAX_ALIGN - MIN_ALIGN + 1];
};

NMS_END // end namespace kmem

#endif // __OBJECT_POOL_H_

