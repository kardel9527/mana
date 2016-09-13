#ifndef __I_HANDLER_H_
#define __I_HANDLER_H_
#include "udt.h"
#include "macros.h"

NMS_BEGIN(kevent)
class IReactor;

class IHandler {
public:
	enum HandlerEventType { HET_Read = 0x0001, HET_Write = 0x0002, HET_Except = 0x0004 };

public:
	IHandler() : _reactor(0) {}
	virtual ~IHandler() {}

	virtual void reactor(IReactor *ireactor) { _reactor = ireactor; }

	virtual IReactor* reactor() { return _reactor; }

	virtual int32 get_handle() { return 0; }

	virtual void set_handle(int32 hid) {}

	virtual int32 handle_input() { return 0; }

	virtual int32 handle_output() { return 0; }

	virtual int32 handle_error() { return 0; }

	virtual int32 handle_close() { return 0; }

	virtual int32 handle_timeout() { return 0; }

	virtual int32 handle_sig() { return 0; }

private:
	IReactor *_reactor;
};

NMS_END // end namespace kevent

#endif // __I_HANDLER_H_
