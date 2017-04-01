#include <stdio.h>
#include <unistd.h>
#include "test_s2c_session.h"
#include "service.h"

using namespace kcommon;
using namespace kevent;

int32 main(int32 argc, char *argv[]) {
	InetAddr addr("127.0.0.1", 7788);
	Service svc;
	int32 ret = svc.open(addr, 1024, new SessionMgrTest());
	ret = svc.start();

	while (true) { usleep(10000); svc.update(); }

	return 0;
}
