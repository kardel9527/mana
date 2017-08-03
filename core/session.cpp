#include "netiohandler.h"
#include "session.h"

NMS_BEGIN(kcore)
Session::Session() : _id(0), _type(ST_NONE), _state(SS_NONE), _io_handler(new NetIoHandler()), _mgr(0) {
	_io_handler->session(this);
}

Session::~Session() {
	_id = 0;
	_type = ST_NONE;
	_state = SS_NONE;
	sdelete(_io_handler);
	_mgr = 0;
}

void Session::on_recv(const char *data) {
	kcommon::AutoLock<LockType> guard(_msg_queue);
	_msg_queue.push((char *)data);
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
	kcommon::AutoLock<LockType> guard(_msg_queue);
	while (!_msg_queue.empty()) {
		char *packet = _msg_queue.pop();
		handle_packet(packet);
		::free(packet);
	}
}

NMS_END // end namespace kcore

