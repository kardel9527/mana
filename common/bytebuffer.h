#ifndef __BYTE_BUFFER_H_
#define __BYTE_BUFFER_H_
#include <string.h>
#include <string>
#include <stdlib.h>
#include <assert.h>
#include "macros.h"
#include "petty.h"
#include "udt.h"

NMS_BEGIN(kcommon)
class ByteBuffer {
public:
	ByteBuffer() : _data(0), _wr_idx(0), _rd_idx(0) {}
	ByteBuffer(const char *data, uint32 len) { write(data, len); }
	ByteBuffer(const ByteBuffer &other) { reset(); write(other._data + other._rd_idx, other._wr_idx - other._rd_idx); }
	ByteBuffer& operator = (const ByteBuffer &rh) { reset(); write(rh._data + rh._rd_idx, rh._wr_idx - rh._rd_idx); return *this; }
	~ByteBuffer() { sfree(_data); };

	void reset() { _wr_idx = _rd_idx = 0; }

	void write(const char *data, uint32 len) {
		if (_capacity - _wr_idx < len) resize(_capacity + len);
		::memcpy(_data + _wr_idx, data, len);
		_wr_idx += len;
	}

	uint32 read(char *data, uint32 len) {
		len = min(len, _wr_idx - _rd_idx);
		::memcpy(data, _data + _rd_idx, len);
		_rd_idx += len;
		return len;
	}

	void resize(uint32 len) {
		_data = (char *)::realloc(_data, len);
		_capacity = len;
	}

	char* wr_ptr() { return _data + _wr_idx; }

private:
	char *_data;
	uint32 _capacity;
	uint32 _wr_idx, _rd_idx;
};

class ReadBuffer : public ByteBuffer {
public:
	ReadBuffer() {}
	ReadBuffer(const char *data, uint32 len) : ByteBuffer(data, len) {}

	ReadBuffer& operator >> (std::string &str) {
		return *this;
	}

#define RIGHT_SHIFT_OPERATOR(T) \
	ReadBuffer& operator >> (T &t) { assert(read((char *)&t, sizeof(T)) == sizeof(T)); return *this; }

	RIGHT_SHIFT_OPERATOR(int8)
	RIGHT_SHIFT_OPERATOR(uint8)
	RIGHT_SHIFT_OPERATOR(int16)
	RIGHT_SHIFT_OPERATOR(uint16)
	RIGHT_SHIFT_OPERATOR(int32)
	RIGHT_SHIFT_OPERATOR(uint32)
	RIGHT_SHIFT_OPERATOR(int64)
	RIGHT_SHIFT_OPERATOR(uint64)
	RIGHT_SHIFT_OPERATOR(float)
	RIGHT_SHIFT_OPERATOR(double)
	//RIGHT_SHIFT_OPERATOR(std::string)
};

class WriteBuffer : public ByteBuffer {
public:
	WriteBuffer() {}
	WriteBuffer(const char *data, uint32 len) : ByteBuffer(data, len) {}

	WriteBuffer& operator << (const std::string &str) {
		write(str.c_str(), str.size());
		return *this;
	}

#define LEFT_SHIFT_OPERATOR(T) \
	WriteBuffer& operator << (const T &t) { write((char *)&t, sizeof(T)); return *this; }

	LEFT_SHIFT_OPERATOR(int8)
	LEFT_SHIFT_OPERATOR(uint8)
	LEFT_SHIFT_OPERATOR(int16)
	LEFT_SHIFT_OPERATOR(uint16)
	LEFT_SHIFT_OPERATOR(int32)
	LEFT_SHIFT_OPERATOR(uint32)
	LEFT_SHIFT_OPERATOR(int64)
	LEFT_SHIFT_OPERATOR(uint64)
	LEFT_SHIFT_OPERATOR(float)
	LEFT_SHIFT_OPERATOR(double)
	//LEFT_SHIFT_OPERATOR(std::string)
};

NMS_END // end namespace kcommon

#endif // __BYTE_BUFFER_H_

