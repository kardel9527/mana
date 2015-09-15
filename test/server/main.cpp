#include <stdio.h>
#include <unistd.h>
#include "sessionmgr.h"
#include "service.h"

using namespace kcommon;
using namespace kevent;

int32 main(int32 argc, char *argv[]) {
	InetAddr addr("127.0.0.1", 7788);
	Service svc;
	int32 ret = svc.open(addr, 1024, new kcommon::SessionMgr());
	ret = svc.start();

	while (true) { sleep(20); svc.update(); }

	return 0;
}
