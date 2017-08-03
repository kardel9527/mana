#include <stdio.h>
#include <unistd.h>
#include "test_c2s_session.h"
#include "core.h"
#include "udt.h"

using namespace kcommon;
using namespace kevent;
using namespace kcore;

int32 main(int32 argc, char *argv[]) {
	Core svc;
	int ret = svc.open(1024, new SessionMgrTest());
	ret = svc.start();

	svc.connect(kcommon::InetAddr("127.0.0.1", 7788), Session::ST_C2S);

	while (true) { usleep(10000); svc.update();}

	return 0;
}
