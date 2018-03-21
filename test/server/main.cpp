#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include "logger.h"
#include "netiohandler.h"
#include "netmgrbase.h"
#include "network.h"
#include "clientsocket.h"
#include "roommgr.h"

using namespace kcommon;
using namespace network;

void init_instance() {
	// init the logger
	klog::Logger::create();
	klog::Logger::instance()->open(klog::LL_MAX, "svr");

	RoomMgr::create();
}

void uninit_instance() {
	// uninit logger
	klog::Logger::instance()->close();
	klog::Logger::destroy();
	
	RoomMgr::destroy();
}

static bool s_server_active = true;

void stop_server(int sig) {
	s_server_active = false;
	LOG_DEBUG("receive stop singnal, server is stopping.");
}

int32 main(int32 argc, char *argv[]) {
#ifdef DAEMON
	if (::daemon(1, 0)) return -1;
#endif // DAEMON

	// handle signal
	::signal(SIGPIPE, SIG_IGN);
	::signal(SIGINT, stop_server);
	::signal(SIGQUIT, stop_server);
	::signal(SIGKILL, stop_server);

	init_instance();

	LOG_DEBUG("server start at %d", 1);

	InetAddr addr("0.0.0.0", 7788);
	NetWork net(new NetMgrBase());
	int32 ret = net.register_acceptor((int32)NHLT_CLIENT, addr);
	
	for (int i = 0; i < 10000; ++i) {
		net.add_server_handler(new TestClientHandler());
	}

	ret = net.start();

	while (s_server_active) { usleep(10000); net.update(); }

	net.stop();

	LOG_DEBUG("server stoped.");
	
	uninit_instance();

	return 0;
}

