#include <stdio.h>
#include <unistd.h>
#include "sessionmgr.h"
#include "service.h"
#include "udt.h"

using namespace kcommon;
using namespace kevent;

int32 main(int32 argc, char *argv[]) {
	Service svc;
	int ret = svc.open(1024, new kcommon::SessionMgr());
	ret = svc.start();

	svc.connect(kcommon::InetAddr("127.0.0.1", 7788), 1);

	while (true) { sleep(30); svc.update();}

	return 0;
}
