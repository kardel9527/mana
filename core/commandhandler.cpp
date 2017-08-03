#include "./reactor/reactor.h"
#include "connector.h"
#include "commandhandler.h"

NMS_BEGIN(kcore)

int32 CommandHandler::open(Connector *connector, uint32 interval) {
	int32 ret = reactor()->register_timer(this, 0, interval);
	if (ret > 0) {
		_timer_id = ret;
		_connector = connector;
	}

	return ret;
}

void CommandHandler::add(const CommandHandler::Command &cmd) {
	_cmds.lock();
	_cmds.push(cmd);
	_cmds.unlock();
}

int32 CommandHandler::handle_timeout() {
	CommandHandler::Command cmd;
	_cmds.lock();
	if (_cmds.empty()) {
		_cmds.unlock();
		return 0;
	}

	cmd = _cmds.pop();
	_cmds.unlock();

	handle_cmd(cmd);

	return 1;
}

void CommandHandler::handle_cmd(const CommandHandler::Command &cmd) {
	switch (cmd._cmd) {
	case CT_CONNECT:
		_connector->connect(cmd._s);
		break;
	case CT_SEND_ORDERED:
		reactor()->append_handler_mask_once(cmd._fd, IHandler::HET_Write);
		break;
	case CT_DISCONNECT:
		reactor()->remove_handler(cmd._fd);
		break;
	default:
		break;
	}
}

NMS_END // end namespace kcore

