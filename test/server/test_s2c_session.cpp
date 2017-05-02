#include <string>
#include <stdio.h>
#include "logger.h"
#include "test_s2c_session.h"

void TestS2CSession::handle_connect() {
	LOG_DEBUG("session", "session [id:%d, type:%d] connected.", id(), type());
	int32 size = 4;
	send((const char *)&size, sizeof(size), false);
	size = 128;
	send((const char *)&size, sizeof(size), true);
}

void TestS2CSession::handle_disconnect() {
	LOG_DEBUG("session", "session [id:%d, type:%d] disconnected.", id(), type());
}

void TestS2CSession::handle_packet(kcommon::ReadBuffer * packet) {
	static int32 s_num = 0;
	std::string text;
	s_num ++;
	*packet >> text;
	LOG_DEBUG("session", "[id:%d, type:%d] received packet [order:%d, text:%s].", s_num, text.c_str());
}

kcommon::Session* SessionMgrTest::create(int32 type) {
	switch (type) {
	case kcommon::Session::ST_C2S:
		return new TestS2CSession();
	default:
		return SessionMgr::create(type);
	}
}

void SessionMgrTest::destroy(kcommon::Session *s) {
	delete s;
}

