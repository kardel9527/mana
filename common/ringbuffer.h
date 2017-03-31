#ifndef __RING_BUFFER_H_
#define  __RING_BUFFER_H_
#include <stdlib.h>
#include <string.h>
#include "macros.h"
#include "lock.h"
#include "petty.h"
#include "udt.h"

NMS_BEGIN(kcommon)

template<typename T, typename Lock = LockType>
class RingBuffer : public Lock {
public:
	RingBuffer() : _data(0), _capacity(16), _wr_idx(0), _rd_idx(0) { resize(_capacity); }
	RingBuffer(uint32 n) : _data(0), _capacity(n), _wr_idx(0), _rd_idx(0) { resize(_capacity); }
	RingBuffer(const RingBuffer &other) { *this = other;}
	RingBuffer& operator = (const RingBuffer &rh) { uint32 size = rh.avail(); if (space() < size) resize(size); rh.peek(_data, size); _wr_idx += size; return *this; }
	~RingBuffer() { _capacity = 0; _wr_idx = 0; _rd_idx = 0; sfree(_data); }

	uint32 write(const T *data, uint32 len) {
		if (space() < len) resize(_capacity + len);

		uint32 post = min(len, _capacity - _wr_idx % _capacity);
		::memcpy(_data + _wr_idx % _capacity, data, bytes(post));
		::memcpy(_data, data + post, bytes(len - post));

		_wr_idx += len;

		return len;
	}

	void write(const T &data) { write(&data, 1); }

	uint32 read(T *data, uint32 len) {
		uint32 ret = peek(data, len);
		_rd_idx += ret;
		return ret;
	}

	void read(T &data) { read(&data, 1); }

	uint32 peek(T *data, uint32 len) const {
		len = min(len, avail());
		uint32 post = min(len, _capacity - _rd_idx % _capacity);

		::memcpy(data, _data + _rd_idx % _capacity, bytes(post));
		::memcpy(data + post, _data, bytes(len - post));

		return len;
	}

	void peek(T &data) const { peek(&data, 1); }

	// space can read.
	uint32 avail() const { return _wr_idx - _rd_idx; }

	void rd_move(uint32 n) { _rd_idx += n; }

	// real space for write
	uint32 space() const { return _capacity - (_wr_idx - _rd_idx); }

	void resize(uint32 n) {
		n = roundup_pow_of_two(n);
		_data = (T *)::realloc(_data, bytes(n));
		_capacity = n;
	}

private:
	uint32 bytes(uint32 len) const { return len * sizeof(T); }

private:
	T *_data;
	uint32 _capacity;
	uint32 _wr_idx, _rd_idx;
};

NMS_END // end namespace kcommon

#endif // __RING_BUFFER_H_

