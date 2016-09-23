#include <stdio.h>
#include "test_c2s_session.h"

void TestC2SSession::handle_connect() {
	printf("fd [%d] connected.\n", id());
}

void TestC2SSession::handle_disconnect() {
	printf("fd [%d] disconnected.\n", id());
}

void TestC2SSession::handle_packet(kcommon::ReadBuffer * packet) {
	int32 num = 0;
	*packet >> num;

	printf("begin send %d packet.\n", num);
	for (int32 i = 0; i < num; ++i) {
		const char *s = "hello";
		uint32 size = ::strlen(s) + sizeof(size);
		send((const char *)&size, sizeof(size), false);
		size -= sizeof(size);
		send((const char *)&size, sizeof(size), false);
		send(s, strlen(s), false);
	}
}

kcommon::Session* SessionMgrTest::create(int32 type) {
	switch (type) {
	case kcommon::Session::ST_C2S:
		return new TestC2SSession();
	default:
		return SessionMgr::create(type);
	}
}

void SessionMgrTest::destroy(kcommon::Session *s) {
	delete s;
}

