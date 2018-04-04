#ifndef __TIME_MANAGER_H_
#define __TIME_MANAGER_H_
#include "macros.h"
#include "udt.h"
#include "singleton.h"
#include "noncopy.h"

class TimeManager : public Noncopy, public kcommon::Singleton<TimeManager> {
public:
	TimeManager() : _fix_time(0), _ms_now(0) {}
	~TimeManager() {}

	void tick();

	void fix_time(int32 fix_time) { _fix_time = fix_time; }
	int32 fix_time() const { return _fix_time; }

	uint64 ms_now() const { return _ms_now + _fix_time; }
	uint32 now() const { return ms_now() / 1000; }

private:
	int32 _fix_time;
	uint64 _ms_now;
};

#define sTimeMgr (*TimeManager::Instance())

#endif // __TIME_MANAGER_H_
