#include <sys/time.h>
#include "timeutil.h"

NMS_BEGIN(ktimeutil)

bool check_is_same_day(uint64 t1, uint64 t2) {
	struct tm tm1 = *::localtime(&t1);
	struct tm tm2 = *::localtime(&t2);
	return tm1.tm_year == tm2.tm_year && tm1.tm_yday == tm2.tm_yday;
}

NMS_END // end namespace ktimeutil.

