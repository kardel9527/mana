#include <stdio.h>
#include "test_c2s_session.h"

TestC2SSession::handle_connect() {
	printf("fd [%d] connected.\n", _io_handler->get_handle());
}

TestC2SSession::handle_disconnect() {
	printf("fd [%d] disconnected.\n", _io_handler->get_handle());
}

TestC2SSession::handle_packet(ReadBuffer * packet) {
	int32 num = 0;
	*packet >> num;

	printf("begin send %d packet.\n", num);
	for (int32 i = 0; i < num; ++i) {
		char *s = "hello";
		uint32 size = ::strlen(s) + sizeof(size) + 1;
		send((const char *)&size, sizeof(size), false);
		send(s, strlen(s), false);
		send("\0", 1, true);
	}
}

kcommon::Session* SessionMgrTest::create(int32 type) {
	switch (type) {
	case kcommon:Session::ST_C2S:
		return new TestC2SSession();
	default:
		return SessionMgr::create(type);
	}
}

void SessionMgrTest::destroy(kcommon::Session *s) {
	delete s;
}

