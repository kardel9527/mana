#include <stdio.h>
#include "reactor.h"
#include "acceptor.h"
#include "netiohandler.h"
#include "epollreactor.h"
#include "udt.h"

using namespace kcommon;
using namespace kevent;

int32 main(int32 argc, char *argv[]) {
	EpollReactor impl;
	impl.open(1024, 0, 0);
	Reactor reactor(&impl, false);
	
	Acceptor acceptor;
	acceptor.reactor(reactor.impl());
	acceptor.open("127.0.0.1", 7788);

	reactor.begin_handle_events(50);

	return 0;
}
