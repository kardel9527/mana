#ifndef __RING_BUFFER_H_
#define  __RING_BUFFER_H_
#include <stdlib.h>
#include <string.h>
#include "macros.h"
#include "udt.h"

class RingBuffer {
public:
	RingBuffer() : _capacity(1024), _length(0), _wr_idx(0), _rd_idx(0) { _data = (char *)::malloc(_capacity); }
	RingBuffer(uint32 n) : _capacity(n), _length(0), _wr_idx(0), _rd_idx(0) { _data = (char *)::malloc(_capacity); }
	RingBuffer(const RingBuffer &other) : _capacity(other._capacity), _length(other._length), _wr_idx(other._wr_idx), _rd_idx(other._rd_idx) { _data = (char *)::malloc(_capacity); }
	RingBuffer& operator = (const RingBuffer &rh) { _capacity = rh._capacity; _length = rh._length; _wr_idx = rh._wr_idx; _rd_idx = rh._rd_idx; _data = (char *)::malloc(_capacity); return *this; }
	~RingBuffer() { _capacity = 0; _length = 0; _wr_idx = 0; _rd_idx = 0; safe_free(_data); }


	void write(const char *data, uint32 len) {
		if ((_capacity - length()) < len) resize(_capacity + len *2);
		uint32 left = len;
		while (left > 0) {
			uint32 wr_len = left > space() ? space() : left;
			memcpy(_data + _wr_idx, data + len - left, wr_len);
			wr_move(wr_len);
			left -= wr_len;
		}
	}

	uint32 length() { return _length; }

	// space can read.
	uint32 avail() {
		if (_rd_idx < _wr_idx) return _wr_idx - _rd_idx;
		if (_rd_idx > _wr_idx) return _capacity - _rd_idx;
		if (_rd_idx == _wr_idx) return _capacity - _rd_idx;
		return 0;
	}

	void rd_move(uint32 n) { _rd_idx = (_rd_idx + n) % _capacity; _length -= n; }

	char* rd_ptr() { return _data + _rd_idx; }

	// space for write
	uint32 space() {
		if (_wr_idx > _rd_idx) return _capacity - _wr_idx;
		if (_wr_idx < _rd_idx) return _rd_idx - _wr_idx;
		if (_wr_idx == _rd_idx) return length() > 0 ? 0 : _capacity - _wr_idx;
		return 0;
	}

	void wr_move(uint32 n) { _wr_idx = (_wr_idx + n) % _capacity; _length += n; }

	char* wr_ptr() { return _data + _wr_idx; }

	void resize(uint32 n) {
		_data = (char *)::realloc(_data, n);
		_capacity = n;
	}

private:
	char *_data;
	uint32 _capacity;
	uint32 _length;
	uint32 _wr_idx, _rd_idx;
};

#endif // __RING_BUFFER_H_

