#ifndef __SNOW_FLAKE_H_
#define __SNOW_FLAKE_H_
#include <sys/time.h>
#include "macros.h"
#include "udt.h"
#include "singleton.h"

NMS_BEGIN(kcommon)

class SnowFlake : public kcommon::Singleton<SnowFlake> {
public:
	SnowFlake() : _seed(0), _server_id(0), _last_timestamp(0) {}
	~SnowFlake() {}

	void set_server_id(uint16 server_id) { _server_id = server_id; }

	uint64 generate() {
		uint64 now = timestamp();
		// used up, til next macro seconds.
		while (now == _last_timestamp && _seed == bit_max_num(_seed_bit)) now = timestamp(); // or sleep here

		// if macro seconds changed, reset the seed value.
		if (now != _last_timestamp) _seed = 0;

		// set last timestamp.
		_last_timestamp = now;

		return ((_last_timestamp - _twe_poch) << (_id_bit + _seed_bit)) | (_server_id << _seed_bit) | _seed ++;
	}

private:
	uint64 timestamp() {
		::timeval val;
		::gettimeofday(&val, 0);
		return (uint64)tv.tv_sec * 1000 + (uint64)tv.tv_usec / 1000;
	}

private:
	SnowFlake(const &SnowFlake other) {}
	SnowFlake& operator = (const SnowFlake &rh) { return *this; }

private:
	uint64 _seed;
	uint64 _server_id;
	uint64 _last_timestamp;

	/*69 years*/
	const static uint8 _timestamp_bit = 41;
	/*4096 zone*/
	const static uint8 _id_bit = 12;
	/*1024 ids per macro seconds.*/
	const static uint8 _seed_bit = 10;
	// 2017-05-01 00:00:00
	const static uint64 _twe_poch = 1493568000000L;
};

NMS_END // end namespace kcommon

#endif // __SNOW_FLAKE_H_

