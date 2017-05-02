#include <stdio.h>
#include <unistd.h>
#include <singal.h>
#include "test_s2c_session.h"
#include "logger.h"
#include "service.h"

using namespace kcommon;
using namespace kevent;

void init_instance() {
	// init the logger
	klog::Logger::create();
	klog::Logger::instance()->open(klog::LL_MAX);

	// init service
	Service::create();
}

void uninit_instance() {
	// uninit logger
	klog::Logger::instance()->close();
	klog::Logger::destroy();

	// uninit service
	Service::destroy();
}

static bool s_server_active = true;

void stop_server(int sig) {
	s_server_active = false;
	LOG_DEBUG("main", "receive stop singnal, server is stopping.");
}

int32 main(int32 argc, char *argv[]) {
	// handle signal
	::signal(SIGINT, stop_server);
	::signal(SIGQUIT, stop_server);
	::signal(SIGKILL, stop_server);

	init_instance();

	LOG_DEBUG("main", "server start at %d", 1);

	InetAddr addr("127.0.0.1", 7788);
	Service *svc = Service::instance();
	int32 ret = svc->open(addr, 1024, new SessionMgrTest());
	ret = svc->start();

	while (s_server_active) { usleep(10000); svc.update(); }

	svc->stop();

	uninit_instance();

	LOG_DEBUG("main", "server stoped.");

	return 0;
}

