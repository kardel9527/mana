#ifndef __K_OBJECT_POOL_H_
#define __K_OBJECT_POOL_H_
#include <assert.h>
#include <stdlib.h>
#include <new>
#include <stdio.h>
#include "udt.h"
#include "lock.h"

NMS_BEGIN(kcommon)

class DefaultAllocator {
public:
	void* alloc(uint32 size) { return malloc(size); }
	void dealloc(void *prt) { free(ptr); }
};

template <typename uint32 BLOCKSIZE, typename _LOCK = DefaultLock>
class ObjectBufferT {
	static const uint32 _s_block_size = sizeof(void *) > BLOCKSIZE ? sizeof(void *) : BLOCKSIZE;
	union Node {ObjectBufferT *_next; byte _data[_s_block_size]; };

public:
	ObjectBufferT() : _free(0) {}
	~ObjectBufferT() {}

	void* alloc() {
		void *ret = NULL;
		_LOCK::lock();
		if (ret = _free) {
			_free = _free->_next;
			_LOCK::unlock();
		} else {
			_LOCK::unlock();
			ret = malloc(sizeof(Node));
		}

		return ret;
	}

	void dealloc(void *ptr) {
		Node *node = (Node *)ptr;
		_LOCK::lock();
		node->_next = _free;
		_free = node;
		_LOCK::unlock();
	}

private:
	ObjectBufferT(const ObjectBufferT &other) {}
	ObjectBufferT& operator = (const ObjectBufferT &other) { return *this; }

private:
	Node *_free;
};

NMS_END // end namespace kcommon
	class Default_Alloctor {
	public:
		void *alloc(unsigned size) {
			return malloc(size);
		}
	
		void dealloc(void *ptr) {
			free(ptr);
		}
	};
	
	template <unsigned BLOCKSIZE, typename _LOCK = Null_Locker>
	class Object_Buffer : public _LOCK {
		static const unsigned _s_block_size = BLOCKSIZE < sizeof(void *) ? sizeof(void *) : BLOCKSIZE;
		struct Object_Buffer_Node {
			union  {
				Object_Buffer_Node *_next;
				char _data[_s_block_size];
			};

			Object_Buffer_Node() : _next(0) {}
		};

	public:
	Object_Buffer() : _free(0) {}

	~Object_Buffer() {
		Object_Buffer_Node *node = _free;
		while (node) {
			Object_Buffer_Node *tmp = node;
			node = node->_next;
			free(tmp);
		}
	}

	void *alloc() {
		Object_Buffer_Node *node = NULL;
	
		_LOCK::lock();
		if ((node = _free)) {
			// there are some free block in free list.
			_free = _free->_next;
			_LOCK::unlock();
		} else {
			_LOCK::unlock();
			node = (Object_Buffer_Node *)malloc(BLOCKSIZE);
		}
		return (node->_data);
	}	
		
	// alloc and construct with default constructor
	template<typename T>
	T* alloc(T *) {
		return new(alloc()) T();
	}
	
	// alloc and constructed with 1 parameter
	template<typename P, typename T>
	T* alloc(const P &p, T *) {
		return new(alloc()) T(p);
	}
	
	// alloc and constructed with 2 parameter
	template<typename P, typename P1, typename T>
	T* alloc(const P &p, const P1 &p1, T *) {
		return new(alloc()) T(p, p1);
	}

	// alloc and constructed with 3 parameter
	template<typename P, typename P1, typename P2, typename T>
	T* alloc(const P &p, const P1 &p1, const P2 &p2, T *) {
		return new(alloc()) T(p, p1, p2);
	}

	void dealloc(void *ptr) {
		Object_Buffer_Node *node = (Object_Buffer_Node *)ptr;
		_LOCK::lock();
		node->_next = _free;
		_free = node;
		_LOCK::unlock();
	}

	template<typename T>
	void dealloc(T *t) {
		t->~T();
		dealloc((void *)t);
	}
	
	private:
		Object_Buffer_Node *_free;
	};
	
	template<typename _LOCK = Null_Locker>
	class Pool_Block : public _LOCK {
		struct Pool_Block_Node {
			union {
				Pool_Block_Node *_next;
				Pool_Block *_belong;
				char *data;
			};

			Pool_Block_Node() : _next(0) {}
		};
	
	public:
		Pool_Block() : _free(0), _len(0) {}
		Pool_Block(unsigned len) : _free(0), _len(len) {}
		~Pool_Block() {
			Pool_Block_Node *node = _free;
			while (node) {
				Pool_Block_Node *tmp = node;
				node = node->_next;
				free(tmp);
			}
		}

		void set_block_len(unsigned len) { 
			_len = len;
		}
	
		void *alloc() {
			Pool_Block_Node *node = NULL;
			_LOCK::lock();
			if ((node = _free)) {
				// there are some free block in free list.
				_free = _free->_next;
				_LOCK::unlock();
			} else {
				_LOCK::unlock();
				node = (Pool_Block_Node *)malloc(_len);
			}
			node->_belong = this;
			return ((char *)node + sizeof(void *));
		}
	
		void dealloc(void *ptr) {
			Pool_Block_Node *node = (Pool_Block_Node *)ptr;
			_LOCK::lock();
			node->_next = _free;
			_free = node;
			_LOCK::unlock();
		}
	
	protected:
		Pool_Block(Pool_Block &r) {}
		Pool_Block& operator = (Pool_Block &) { return *this; }
	private:
		// block free list.
		Pool_Block_Node *_free;
		// len of this block
		unsigned _len;
	};

	template<typename _LOCK>
	class Object_Pool {
		// 2^X_ALIGN bytes
		// 64 KBytes
		static const int MAX_ALIGN = 16;
		// 4 bytes
		static const int MIN_ALIGN = 2;
	
		typedef Pool_Block<_LOCK> Block;
	
	public:
		Object_Pool() {
			for (int i = MIN_ALIGN; i < MAX_ALIGN + 1; ++i) {
				_buffer[i - MIN_ALIGN].set_block_len((1u << i) + sizeof(void *));
			}
		}

		// construct with 0 param
		template<typename T>
		T* alloc() {
			void *ptr = this->alloc(sizeof(T));
			return new (ptr) T();
		}

		template<typename P1, typename T>
		T* alloc(const P1 &p1) {
			void *ptr = this->alloc(sizeof(T));
			return new (ptr) T(p1);
		}
	
		template<typename P1, typename P2, typename T>
		T* alloc(const P1 &p1, const P2 &p2) {
			void *ptr = this->alloc(sizeof(T));
			return new (ptr) T(p1, p2);
		}
	
		template<typename P1, typename P2, typename P3, typename T>
		T* alloc(const P1 &p1, const P2 &p2, const P3 &p3) {
			void *ptr = this->alloc(sizeof(T));
			return new (ptr) T(p1, p2, p3);
		}
	
		void* alloc(unsigned size) {
			int index = get_align(size) - MIN_ALIGN;
			if (index <= 0) {
				char *ptr = new char[size + sizeof(void *)];
				*(long *)ptr = 0;
				return ptr + sizeof(void *);
			} else {
				return _buffer[index].alloc(); 
			}
		}
	
		void dealloc(void *ptr) {
			// restore sizeof(void *) header.
			char *data = (char *)ptr - sizeof(void *);
			if (*(long *)data == 0) {
				delete [] data;
			} else {
				(*(Block **)data)->dealloc(data);
			}
		}

		template <typename T>
		void dealloc(T *t) {
			t->~T();
			this->dealloc((void *)t);
		}
	
	private:
		int get_align(unsigned size) {
			for (int i = MIN_ALIGN; i < MAX_ALIGN + 1; ++i) {
				if ((1u << i) >= size) return i;
			}
			return 0;
		}
	
	private:
		Block _buffer[MAX_ALIGN - MIN_ALIGN + 1];
	};

#ifdef K_MULTI_THREAD_
	typedef Object_Pool<Mutex_Locker> Allocator;
#else
	typedef Object_Pool<Null_Locker> Allocator;
#endif

} // end namespace kcommon

#endif // __K_OBJECT_POOL_H_
