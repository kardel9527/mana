#include <sys/time.h>
#include "timemanager.h"

void TimeManager::tick() {
	timeval now = { 0 , 0 };
	::gettimeofday(&now, 0);

	_ms_now = now.tv_sec * 1000 + now.tv_usec / 1000;
}
