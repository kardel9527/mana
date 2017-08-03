#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include "test_s2c_session.h"
#include "logger.h"
#include "core.h"

using namespace kcommon;
using namespace kevent;
using namespace kcore;

void init_instance() {
	// init the logger
	klog::Logger::create();
	klog::Logger::instance()->open(klog::LL_MAX);

	// init service
	Core::create();
}

void uninit_instance() {
	// uninit logger
	klog::Logger::instance()->close();
	klog::Logger::destroy();

	// uninit service
	Core::destroy();
}

static bool s_server_active = true;

void stop_server(int sig) {
	s_server_active = false;
	LOG_DEBUG(klog::LM_MAIN, "receive stop singnal, server is stopping.");
}

int32 main(int32 argc, char *argv[]) {
	// handle signal
	::signal(SIGINT, stop_server);
	::signal(SIGQUIT, stop_server);
	::signal(SIGKILL, stop_server);

	init_instance();

	LOG_DEBUG(klog::LM_MAIN, "server start at %d", 1);

	InetAddr addr("127.0.0.1", 7788);
	Core *core = Core::instance();
	int32 ret = core->open(addr, 1024, new SessionMgrTest());
	ret = core->start();

	while (s_server_active) { usleep(10000); core->update(); }

	core->stop();

	LOG_DEBUG(klog::LM_MAIN, "server stoped.");
	
	uninit_instance();

	return 0;
}

