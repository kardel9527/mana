#include <stdio.h>
#include "reactor.h"
#include "inetaddr.h"
#include "connector.h"
#include "epollreactor.h"
#include "udt.h"

using namespace kcommon;
using namespace kevent;

int32 main(int32 argc, char *argv[]) {
	EpollReactor impl;
	impl.open(1024, NULL, NULL);
	Reactor reactor(&impl, false);

	Connector conn;
	conn.reactor(reactor.impl());
	conn.open(5);

	InetAddr addr("127.0.0.1", 7788);
	conn.connect(addr);

	reactor.begin_handle_events();

	return 0;
}
