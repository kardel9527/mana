#include <pthread.h>
#include <stdio.h>
#include <assert.h>
#include <sys/time.h>
#include <stdarg.h>
#include <unistd.h>
#include "timeutil.h"
#include "logger.h"

NMS_BEGIN(klog)

static const char *s_level_desc[] = {
	"FATAL",
	"ERROR",
	"WARNING",
	"NOTICE",
	"INFO",
	"DEBUG",
};

Logger::~Logger() {
	if (_fp) { ::fclose(_fp); _fp = 0; }
	_active = false;
	_last_create_time = 0;
	_limit = LL_MAX;
	_thread = 0;
}

int32 Logger::open(LogLevel limit) {
	log_file_checking();
	if (!_fp) return -1;

	// ok, set logger available now.
	_active = true;

	// start the io write thread.
	int32 ret = ::pthread_create((pthread_t *)&_thread, NULL, &Logger::io_routine, this);
	if (ret != 0) return ret;

	_limit = limit;

	return 0;
}

void Logger::close() {
	_active = false;
	::pthread_join(_thread, 0);

	// flush rest data in cache.
	flush();
}

// TODO:more effective and thread safety?
void Logger::log(const char *file, const char *func, uint32 line, LogLevel lv, const char *fmt, ...) {
	if (lv > _limit) return ;

	time_t now = ::time(0);
	struct tm tnow = *::localtime(&now);

	char buff[4096] = { 0 };
	int n = snprintf(buff, sizeof(buff) - 1, "[%04d-%02d-%02d %02d:%02d:%02d] [%s] [%s:%s:%d] ", 
		tnow.tm_year + 1900, 
		tnow.tm_mon + 1,
		tnow.tm_mday,
		tnow.tm_hour,
		tnow.tm_min,
		tnow.tm_sec,
		s_level_desc[lv],
		file,
		func,
		line);

	va_list ap;
	va_start(ap, fmt);
	n += ::vsnprintf(buff + n, sizeof(buff) - n - 1, fmt, ap);
	va_end(ap);

	buff[n] = '\n';
	_log_buffer.lock();
	_log_buffer.write(buff, n + 1);
	_log_buffer.unlock();
}

void Logger::log_file_checking() {
	uint64 now = ::time(0);
	if (_last_create_time && ktimeutil::check_is_same_day(now, _last_create_time)) return ;

	char buff[128] = { 0 };
	struct tm tnow = *::localtime((time_t *)&now);
	sprintf(buff, "./log/%04d-%02d-%02d.log", tnow.tm_year + 1900, tnow.tm_mon + 1, tnow.tm_mday);

	if (_fp) ::fclose(_fp);

	_fp = ::fopen(buff, "a+");

	_last_create_time = now;
}

void Logger::flush() {
	_log_buffer.lock();

	while (_log_buffer.avail() > 0) {
		assert(_log_buffer.rptr() && _log_buffer.rsize() && "ring buffer logic error!");
		int32 n = _log_buffer.rsize();
		::fwrite(_log_buffer.rptr(), n, sizeof(char), _fp);
		_log_buffer.rmove(n);
	}

	_log_buffer.unlock();

	::fflush(_fp);
}

void* Logger::io_routine(void * arg) {
	Logger *logger = (Logger *)arg;

	while (logger->_active) {
		::usleep(10000);
		logger->log_file_checking();
		logger->flush();
	}

	return NULL;
}

NMS_END // end namespace klog

