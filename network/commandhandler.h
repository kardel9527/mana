#ifndef __COMAND_HANDLER_H_
#define __COMAND_HANDLER_H_
#include "udt.h"
#include "macros.h"
#include "inetaddr.h"
#include "fastqueue.h"
#include "./reactor/ihandler.h"

NMS_BEGIN(network)

class NetIoHandler;

class CommandHandler : public kevent::IHandler {
public:
	enum CommandType {
		CT_NONE = 0,
		CT_SEND_ORDERED = 1, // wait to send some data.
		CT_DISCONNECT = 2, // disconnect.
		CT_CONNECT = 3,
		CT_MAX
	};

	struct Command {
		CommandType _cmd;
		NetIoHandler *_handler;

		Command() : _cmd(CT_NONE), _handler(0) {}
		Command(CommandType cmd, NetIoHandler *handler) : _cmd(cmd), _handler(handler) {}
	};

public:
	CommandHandler() { }
	virtual ~CommandHandler() { }

	int32 open(int32 itv);
	void close();

	void add(const CommandHandler::Command &cmd);

	virtual int32 handle_timeout();

private:
	void handle_cmd(const CommandHandler::Command &cmd);

private:
	kcommon::FastQueue<Command> _cmds;
};

NMS_END // end namespace network

#endif // __COMAND_HANDLER_H_

