#include <assert.h>
#include "reactor.h"
#include "netiohandler.h"
#include "commandhandler.h"

NMS_BEGIN(network)

int32 CommandHandler::open(int32 itv) {
	return register_timer_event(itv);
}

void CommandHandler::close() {
	unregister_timer_event();
}

void CommandHandler::add(const CommandHandler::Command &cmd) {
	assert(cmd._cmd > CT_NONE && cmd._cmd < CT_MAX && "error cmd type!");
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
	assert(cmd._handler && "empty handler!");
	switch (cmd._cmd) {
	case CT_SEND_ORDERED:
		// already closed, skip
		if (!cmd._handler->active()) break;
				//
		// add write flag here.
		cmd._handler->flag(kcommon::bit_enable(cmd._handler->flag(), IHandler::HET_WRITE));
		reactor()->modify_handler(cmd._handler);
		break;
	case CT_DISCONNECT:
		reactor()->unregister_handler(cmd._handler->handle());
		cmd._handler->handle_close();
		break;
	case CT_CONNECT:
		break;
	default:
		assert(false && "unknown cmd type!");
		break;
	}
}

NMS_END // end namespace network

