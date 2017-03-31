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

	}
}

