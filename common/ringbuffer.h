#ifndef __RING_BUFFER_H_
#define  __RING_BUFFER_H_
#include <stdlib.h>
#include <string.h>
#include "macros.h"
#include "udt.h"

class RingBuffer {
public:
	RingBuffer() : _capacity(1024), _wr_idx(0), _rd_idx(0) { _data = (char *)::malloc(_capacity); }
	RingBuffer(uint32 n) : _capacity(n), _wr_idx(0), _rd_idx(0) { _data = (char *)::malloc(_capacity); }
	RingBuffer(const RingBuffer &other) { *this = other;}
	RingBuffer& operator = (const RingBuffer &rh) { _capacity = rh._capacity; _wr_idx = rh._wr_idx; _rd_idx = rh._rd_idx; _data = (char *)::malloc(_capacity); return *this; }
	~RingBuffer() { _capacity = 0; _wr_idx = 0; _rd_idx = 0; sfree(_data); }


	uint32 write(const char *data, uint32 len) {
		if (space() < len) resize(_capacity + len * 2);

		uint32 post = min(len, _capacity - _wr_idx % _capacity);
		::memcpy(_data + _wr_idx % _capacity, data, post);
		::memcpy(_data, data + post, len - post);

		_wr_idx += len;

		return len;
	}

	uint32 read(char *data, uint32 len) {
		uint32 ret = peek(data, len);
		_rd_idx += ret;
		return ret;
	}

	uint32 peek(char *data, uint32 len) {
		len = min(len, avail());
		uint32 post = min(len, _capacity - _rd_idx % _capacity);

		::memcpy(data, _data + _rd_idx % _capacity, post);
		::memcpy(data + post, _data, len - post);

		return len;
	}

	// space can read.
	uint32 avail() { return _wr_idx - _rd_idx; }

	void rd_move(uint32 n) { _rd_idx += n; }

	// space for write
	uint32 space() { return _capacity - (_wr_idx - _rd_idx); }

	void resize(uint32 n) {
		_data = (char *)::realloc(_data, n);
		_capacity = n;
	}

private:
	char *_data;
	uint32 _capacity;
	uint32 _wr_idx, _rd_idx;
};

#endif // __RING_BUFFER_H_

