#include "netiohandler.h"
#include "session.h"

NMS_BEGIN(kcommon)
Session::Session() : _type(ST_NONE), _state(SS_NONE), _io_handler(new kcommon::NetIoHandler()), _mgr(0) {
	_io_handler->session(this);
}

Session::~Session() {
	sdelete(_io_handler);
	_mgr = 0;
}

void Session::on_recv(ReadBuffer *buff) {
	AutoLock<LockType> guard(_msg_queue);
	_msg_queue.push(buff);
}

int32 Session::id() {
	return _io_handler->get_handle();
}

void Session::update() {
	// try handle packet
	process_packet();

	// keep alive.
	keep_alive();
}

int32 Session::send(const char *data, int32 len, bool immediately/* = false*/) {
	if (!_io_handler->is_active()) return 0;

	return _io_handler->send(data, len, immediately);
}

void Session::kickoff() {
	if (!_io_handler->is_active()) return ;

	_io_handler->disconnect();
}

void Session::keep_alive() {

}

void Session::process_packet() {
	AutoLock<LockType> guard(_msg_queue);
	while (!_msg_queue.empty()) {
		kcommon::ReadBuffer *buff = _msg_queue.pop();
		handle_packet(buff);
		delete buff;
	}
}

NMS_END // end namespace kcommon

