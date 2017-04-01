#include <string>
#include <stdio.h>
#include "test_s2c_session.h"

TestC2SSession::handle_connect() {
	printf("fd [%d] connected.\n", _io_handler->get_handle());
	int32 size = 4;
	send((const char *)&size, sizeof(size), false);
	size = 128;
	send((const char *)&size, sizeof(size), true);
}

TestC2SSession::handle_disconnect() {
	printf("fd [%d] disconnected.\n", _io_handler->get_handle());
}

TestC2SSession::handle_packet(ReadBuffer * packet) {
	static int32 s_num = 0;
	std::string text;
	s_num ++;
	*packet >> text;
	printf("handle packet %d, content[%s]\n", s_num, text.c_str());
}


