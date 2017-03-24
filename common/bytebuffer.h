#ifndef __BYTE_BUFFER_H_
#define __BYTE_BUFFER_H_
#include <string.h>
#include <string>
#include <assert.h>
#include "macros.h"
#include "udt.h"

NMS_BEGIN(kcommon)
class ByteBuffer {
public:
	ByteBuffer() : _data(0), _wr_idx(0), _rd_idx(0) {}
	ByteBuffer(const char *data, uint32 len) { write(data, len); }
	ByteBuffer(const ByteBuffer &other) { reset(); write(data, len); }
	ByteBuffer& operator = (const ByteBuffer &rh) { reset(); write(rh._data + rh._rd_idx, rh._wr_idx - rh._rd_idx); }
	~ByteBuffer() { sfree(_data); };

	void reset() { _wr_idx = _rd_idx = 0; }

	void write(const char *data, uint32 len) {
		if (_capacity - _wr_idx < len) resize(_capacity + len);
		::memepcy(_data + _wr_idx, data, len);
		_wr_idx += len;
	}

	void write(const std::string *str, uint32 len) {
		write(str->c_str(), str.size());
	}

	uint32 read(char *data, uint32 len) {
		len = min(len, _wr_idx - _rd_idx);
		::memcpy(data, _data + _rd_idx, len);
		_rd_idx += len;
		return len;
	}

	// TODO: more safe
	uint32 read(std::string *str, uint32 len = 0) {
		str->assign(_data + _rd_idx);
		_rd_idx += str->size();
		assert(_rd_idx < _wr_idx);
		return 0;
	}

	void resize(uint32 len) {
		_data = ::realloc(_data, len);
		_capacity = len;
	}

private:
	char *_data;
	uint32 _capacity;
	uint32 _wr_idx, _rd_idx;
};

class ReadBuffer : public ByteBuffer {
public:
	ReadBuffer() {}
	ReadBuffer(const char *data, uint32 len) : ByteBuffer(data, len) {}

#define RIGHT_SHIFT_OPERATOR(T) \
	ReadBuffer& operator >> (T &t) { assert(read(&t, sizeof(T)) == sizeof(T)); return *this; }

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
	RIGHT_SHIFT_OPERATOR(std::string)
};

class WriteBuffer : public ByteBuffer {
public:
	ReadBuffer() {}
	ReadBuffer(const char *data, uint32 len) : ByteBuffer(data, len) {}

#define LEFT_SHIFT_OPERATOR(T) \
	ReadBuffer& operator << (const T &t) { write(&t, sizeof(T)); return *this; }

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
	LEFT_SHIFT_OPERATOR(std::string)
};


NMS_END // end namespace kcommon

#endif // __BYTE_BUFFER_H_

