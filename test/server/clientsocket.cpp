#include "logger.h"
#include "roommgr.h"
#include "clientsocket.h"

void TestClientHandler::handle_connect() {
	LOG_INFO("a session [id:%d type:%d addr:%s port:%d] connected.", id(), type(), addr().ip(), addr().port());
	//RoomMgr::instance()->on_client_enter(this);
}

void TestClientHandler::handle_disconnect() {
	LOG_INFO("a session [id:%d type:%d addr:%s port:%d] disconnected.", id(), type(), addr().ip(), addr().port());
	//_r->remove(id());
}

void TestClientHandler::handle_packet(const char *data) {
	//_r->broadcast(id(), data;
	send(data, *(uint32*)data);
}
