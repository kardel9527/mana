#ifndef __COMAND_HANDLER_H_
#define __COMAND_HANDLER_H_
#include "udt.h"
#include "macros.h"
#include "inetaddr.h"
#include "fastqueue.h"
#include "./reactor/ihandler.h"

NMS_BEGIN(kcore)

class Session;
class Connector;

class CommandHandler : public kevent::IHandler {
public:
	enum CommandType {
		CT_NONE = 0,
		CT_CONNECT = 1, // connect to a server.
		CT_SEND_ORDERED = 2, // wait to send some data.
		CT_DISCONNECT = 3, // disconnect.
	};

	struct Command {
		CommandType _cmd;
		union {
			int32 _fd;
			Session *_s;
		};

		Command() : _cmd(CT_NONE) {}
		Command(CommandType cmd, int32 fd) : _cmd(cmd), _fd(fd) {}
		Command(CommandType cmd, Session *s) : _cmd(cmd), _s(s) {}
	};

public:
	CommandHandler() : _timer_id(-1), _connector(0) {}
	virtual ~CommandHandler() { _timer_id = -1; _connector = 0; }

	int32 open(Connector *connector, uint32 interval);

	void add(const CommandHandler::Command &cmd);

	virtual int32 handle_timeout();

private:
	void handle_cmd(const CommandHandler::Command &cmd);

private:
	int32 _timer_id;
	Connector *_connector;
	kcommon::FastQueue<Command> _cmds;
};

NMS_END // end namespace kcore

#endif // __COMAND_HANDLER_H_

