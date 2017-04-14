#ifndef __LOGGER_H_
#define __LOGGER_H_
#include "macros.h"
#include "udt.h"
#include "ringbuffer.h"

NMS_BEGIN(kcommon)

enum LogLevel {
	LL_FATAL = 0,
	LL_ERROR = 1,
	LL_WARNING = 2,
	LL_NOTICE = 3,
	LL_INFO = 4,
	LL_DEBUG = 5
};

class Logger {
public:
	Logger() {}
	~Logger() {}

	void log(const char *module, LogLevel lv, const char *fmt, ...);

private:
	Logger(const Logger &other) {}
	Logger& operator = (const Logger &rh) { return *this; }

private:
	FILE *_fp;
	kcommon::RingBuffer<char> _log_buffer;
};

NMS_END // end namespace kcommon

#endif // __LOGGER_H_

