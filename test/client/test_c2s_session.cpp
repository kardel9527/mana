#include <stdio.h>
#include "logger.h"
#include "packet.h"
#include "test_c2s_session.h"

void TestC2SSession::handle_connect() {
	//LOG_DEBUG(klog::LM_CORE, "session [id:%d, type:%d] connect successfully.", id(), type());
}

void TestC2SSession::handle_disconnect() {
	//LOG_DEBUG(klog::LM_CORE, "session [id:%d, type:%d] disconnect successfully.", id(), type());
}

void TestC2SSession::handle_packet(const char *data) {
	kcore::PacketHeader *header = (kcore::PacketHeader *)data;
	switch (header->opcode()) {
	case 1:
		{
			char buffer[1024] = { 0 };
			kcore::PacketHeader *content = (kcore::PacketHeader *)buffer;
			if (!::strcmp(header->_data, "hello")) {
				::sprintf(content->_data, "%s", "hi");
				content->_size = sizeof(kcore::PacketHeader) + 2;
			}
			content->_opcode = 1;
			send(buffer, content->_size);
		}
		break;
	default:
		break;
	}
	//LOG_DEBUG(klog::LM_CORE, "session [id:%d, type:%d] received packet[op:%d, content:%s].", id(), type(), header->opcode(), header->_data);
}

kcore::Session* SessionMgrTest::create(int32 type) {
	switch (type) {
	case kcore::Session::ST_C2S:
		return new TestC2SSession();
	default:
		return SessionMgr::create(type);
	}
}

void SessionMgrTest::destroy(kcore::Session *s) {
	delete s;
}

