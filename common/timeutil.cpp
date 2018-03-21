#include <time.h>
#include <string.h>
#include <sys/time.h>
#include "timeutil.h"

NMS_BEGIN(ktimeutil)

bool check_is_same_day(uint64 t1, uint64 t2) {
	struct tm tm1 = *::localtime((time_t *)&t1);
	struct tm tm2 = *::localtime((time_t *)&t2);
	return tm1.tm_year == tm2.tm_year && tm1.tm_yday == tm2.tm_yday;
}

uint64 get_current_time() {
	struct timeval now;
	memset(&now, 0, sizeof(now));
	::gettimeofday(&now, NULL);

	return now.tv_sec * 1000 + now.tv_usec / 1000;
}

NMS_END // end namespace ktimeutil.

