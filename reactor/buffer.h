#ifndef __NET_BUFFER_H_
#define __NET_BUFFER_H_
#include <string.h>
#include <assert.h>
#include "udt.h"

NMS_BEGIN(kevent)

class NetBuffer {
public:
	NetBuffer() : _data(0), _rd_idx(0), _wr_idx(0), _size(0) {}
	NetBuffer(uint32 size) : _data(0), _rd_idx(0), _wr_idx(0), _size(size) {_data = (int8 *)malloc(size); memset(_data, 0, size);}
	NetBuffer(const NetBuffer &other) {safe_free(_data); NetBuffer(other._size); _rd_idx = other._rd_idx; _wr_idx = other._wr_idx;}
	NetBuffer& operator = (const NetBuffer &other) {NetBuffer(other); return *this;}
	~NetBuffer() {safe_free(_data); _rd_idx = 0; _wr_idx = 0; _size = 0;}
	
	bool write(const byte *data, uint32 len) {
		if (capacity() < len && !resize(len)) return false;
		
		shrink(len);
		memcpy(_data + _wr_idx, data, len);
		_wr_idx += len;
		
		return true;
	}
	
	bool peek(const void *ptr, uint32 len) {
		if (rd_size() < len) return false;
		
		memcpy(ptr, _data + _rd_idx, len);
		return true;
	}

	byte* wr_ptr() { shrink(64); return _data + _wr_idx; }
	byte* rd_ptr() { return _data + _rd_idx; }
	
	uint32 rd_size() { return _wr_idx - _rd_idx; }
	uint32 wr_size() { return _size - _wr_idx; }
	uint32 capacity() { return _size - rd_size(); }

	void rd_move(uint32 offset) { _rd_idx += offset; }
	void wr_move(uint32 offset) { _wr_idx += offset; }

private:
	bool resize(uint32 growth) {
		if (!len) return false;
		uint32 grouth = (len / 1024 + 1) * 1024;
		byte *tmp = (byte *)malloc(_size + grouth);
		if (!tmp) return false;
		
		memset(tmp, 0, _size + grouth);
		memcpy(tmp, _data, _size);
		safe_free(_data);
		_data = tmp;
		_size += grouth;
		
		return true;
	}
	
	void shrink(uint32 len) {
		if (_size - _wr_idx >= len) return ;
		
		memcpy(_data, _data + _rd_idx, _wr_idx - _rd_idx);
		_rd_idx -= _rd_idx;
		_wr_idx -= _rd_idx;
	}
private:
	byte *_data;
	uint32 _rd_idx;
	uint32 _wr_idx;
	uint32 _size;
};

NMS_END // end namespace kevent

#endif // __NET_BUFFER_H_

