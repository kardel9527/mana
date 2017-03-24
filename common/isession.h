#ifndef __I_SESSION_H_
#define __I_SESSION_H_

class ISession {
public:
	virtual ~ISession() {}

	virtual void on_recv(const char *data, uint32 len);
};

#endif // __I_SESSION_H_

