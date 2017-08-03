#include <string>
#include <stdio.h>
#include "logger.h"
#include "packet.h"
#include "test_s2c_session.h"

void TestS2CSession::handle_connect() {
	LOG_DEBUG(klog::LM_CORE, "session [id:%d, type:%d] connected.", id(), type());
	char buffer[1024] = { 0 };
	kcore::PacketHeader *header = (kcore::PacketHeader *)buffer;
	header->_size = 6 + 5;
	header->_opcode = 1;
	::sprintf(header->_data, "%s", "hello");
	send((char *)header, header->_size);
}

void TestS2CSession::handle_disconnect() {
	LOG_DEBUG(klog::LM_CORE, "session [id:%d, type:%d] disconnected.", id(), type());
}

void TestS2CSession::handle_packet(const char *data) {
	static int32 s_num = 0;
	s_num ++;
	kcore::PacketHeader *header = (kcore::PacketHeader *)data;
	switch (header->opcode()) {
	case 1:
		{
			char buffer[1024] = { 0 };
			kcore::PacketHeader *content = (kcore::PacketHeader *)buffer;
			if (!::strcmp(header->_data, "hi")) {
				::sprintf(content->_data, "%s", "hello");
				content->_size = sizeof(kcore::PacketHeader) + 5;
			}
			content->_opcode = 1;
			send(buffer, content->_size);
		}
		break;
	default:
		break;
	}
	LOG_DEBUG(klog::LM_CORE, "[id:%d, type:%d] received packet [order:%d, opcode:%d, content:%s].", id(), type(), s_num, header->opcode(), header->_data);
}

kcore::Session* SessionMgrTest::create(int32 type) {
	switch (type) {
	case kcore::Session::ST_C2S:
		return new TestS2CSession();
	default:
		return SessionMgr::create(type);
	}
}

void SessionMgrTest::destroy(kcore::Session *s) {
	delete s;
}

