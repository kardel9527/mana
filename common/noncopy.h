#ifndef __NON_COPY_H_
#define __NON_COPY_H_

class Noncopy {
public:
	Noncopy() {}
	~Noncopy() {}

private:
	Noncopy(const Noncopy &) {}
	const Noncopy& operator = (const Noncopy&) { return *this; }
};

#endif // __NON_COPY_H_
