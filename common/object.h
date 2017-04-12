#ifndef __OBJECT_H_
#define __OBJECT_H_
#include "macros.h"
#include "objectpool.h"

NMS_BEGIN(kmem)

#define ALLOCATE_TRACE

#ifdef ALLOCATE_TRACE
#include <map>
#include <string>
class AllocateRecorder {
	struct Entry {
		std::string _file;
		std::string _func;
		uint32 _line;

		Entry() : _line(0) {}
		Entry(const char *file, const char *func, uint32 line) : _file(file), _func(func), _line(line) {}
	};

public:
	AllocateRecorder() {}
	~AllocateRecorder() { _repo.clear(); }

	void alloced(const char *file, const char *func, uint32 line, void *ptr) {
		_repo[ptr] = Entry(file, func, line);
	}

	void dealloced(const char *file, const char *func, uint32 line, void *ptr) {
		_repo.erase(ptr);
	}

private:
	AllocateRecorder(const AllocateRecorder &other) {}
	AllocateRecorder& operator = (const AllocateRecorder &rh) { return *this; }

private:
	typedef std::map<void *, Entry> Container;
	Container _repo;
};

#endif // ALLOCATE_TRACE

class ObjectPoolAllocator {
public:
	static void* alloc(uint32 size, const char *file, const char *func, uint32 line) {
		void *ret = _allocator.alloc(size);
#ifdef ALLOCATE_TRACE
		_recorder.alloced(file, func, line, ret);
#endif // ALLOCATE_TRACE
		return ret;
	}

	static void* alloc(uint32 size) { return _allocator.alloc(size); }

	static void dealloc(void *ptr, const char *file, const char *func, uint32 line) {
		_allocator.dealloc(ptr);
#ifdef ALLOCATE_TRACE
		_recorder.dealloced(file, func, line, ptr);
#endif // ALLOCATE_TRACE
	}

	static void dealloc(void *ptr) { _allocator.dealloc(ptr); }

private:
	static ObjectPool<2, 23> _allocator;
#ifdef ALLOCATE_TRACE
	static AllocateRecorder _recorder;
#endif // ALLOCATE_TRACE
};

ObjectPool<2, 23> ObjectPoolAllocator::_allocator;
#ifdef ALLOCATE_TRACE
AllocateRecorder ObjectPoolAllocator::_recorder;
#endif // ALLOCATE_TRACE

class DefaultAllocator {
public:
	static void* alloc(uint32 size, const char *file, const char *func, uint32 line) {
		void *ret = ::malloc(size);
#ifdef ALLOCATE_TRACE
		_recorder.alloced(file, func, line, ret);
#endif // ALLOCATE_TRACE
		return ret;
	}

	static void* alloc(uint32 size) { return ::malloc(size); }

	static void dealloc(void *ptr, const char *file, const char *func, uint32 line) {
		::free(ptr);
#ifdef ALLOCATE_TRACE
		_recorder.dealloced(file, func, line, ptr);
#endif // ALLOCATE_TRACE
	}

	static void dealloc(void *ptr) { ::free(ptr); }

private:
#ifdef ALLOCATE_TRACE
	static AllocateRecorder _recorder;
#endif // ALLOCATE_TRACE
};

#ifdef ALLOCATE_TRACE
AllocateRecorder DefaultAllocator::_recorder;
#endif // #ifdef ALLOCATE_TRACE

template<typename Allocator>
class Object {
public:

	void* operator new (size_t size) {
		return Allocator::alloc(size);
	}


	void* operator new[] (size_t size) {
		return Allocator::alloc(size);
	}

	// replacement new
	void* operator new (size_t size, void *ptr) { return ptr; }

	void operator delete (void *ptr) { Allocator::dealloc(ptr); }

	void operator delete[] (void *ptr) { Allocator::dealloc(ptr); }

	void* operator new (size_t size, const char *file, const char *func, uint32 line) {
		return Allocator::alloc(size, file, func, line);
	}

	void* operator new[] (size_t size, const char *file, const char *func, uint32 line) {
		return Allocator::alloc(size, file, func, line);
	}

	void operator delete (void *ptr, const char *file, const char *func, uint32 line) {
		Allocator::dealloc(ptr, file, func, line);
	}

	void operator delete[] (void *ptr, const char *file, const char *func, uint32 line) {
		Allocator::dealloc(ptr, file, func, line);
	}
};

typedef Object<ObjectPoolAllocator> PoolOBject;
typedef Object<DefaultAllocator> DefaultObject;

NMS_END // end namespace kmem

#undef K_NEW_D
#undef K_DELETE_D
#ifdef ALLOCATE_TRACE
#define K_NEW_D new (__FILE__, __FUNCTION__, __LINE__)
#define K_DELETE delete
#else
#define K_NEW_D new
#define K_DELETE delete
#endif // ALLOCATE_TRACE

#endif // __OBJECT_H_

