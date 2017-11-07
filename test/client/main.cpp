#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include "logger.h"
#include "netiohandler.h"
#include "netmgrbase.h"
#include "network.h"

using namespace kcommon;
using namespace network;

class TestClientHandler : public NetIoHandler {
public:
	TestClientHandler() : NetIoHandler(NHLT_CLIENT) {}

	virtual void handle_connect() {
		char buff[1024] = { 0 };
		*(int *)buff = 1024;
		send(buff, 1024);
	}

	virtual void handle_disconnect() {
	}

	virtual void handle_packet(const char *data) {
		uint32 size = *(uint32 *)data;
		send(data, size);
	}
};

void init_instance() {
	// init the logger
	klog::Logger::create();
	klog::Logger::instance()->open(klog::LL_MAX);
}

void uninit_instance() {
	// uninit logger
	klog::Logger::instance()->close();
	klog::Logger::destroy();
}

static bool s_server_active = true;

void stop_server(int sig) {
	s_server_active = false;
}

int32 main(int32 argc, char *argv[]) {
	// handle signal
	::signal(SIGINT, stop_server);
	::signal(SIGQUIT, stop_server);
	::signal(SIGKILL, stop_server);

	init_instance();


	InetAddr addr("127.0.0.1", 7788);
	NetWork net(new NetMgrBase());

	TestClientHandler *handler = new TestClientHandler();
	handler->addr(addr);
	net.add_client_handler(handler);
	int32 ret = net.start();

	while (s_server_active) { usleep(10000); net.update(); }

	net.stop();

	uninit_instance();

	return 0;
}

