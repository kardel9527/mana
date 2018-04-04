#ifndef __CALLBACK_H_
#define __CALLBACK_H_

template<typename T>
class CallbackP0;

template<typename T, typename P1>
class CallbackP1;

template<typename T, typename P1, typename P2>
class CallbackP2;

template<typename T, typename P1, typename P3>
class CallbackP3;

template<typename T, typename P1, typename P3, typename P4>
class CallbackP4;

template<typename T, typename P1, typename P3, typename P4, typename P5>
class CallbackP5;

template<typename T, typename P1, typename P3, typename P4, typename P5, typename P6>
class CallbackP6;

class Callback {
public:
	virtual void run() = 0;

	template<typename T>
	static Callback* bind(T *obj, void (T::*F)()) {
		return new CallbackP0<T>(obj, F);
	}

	template<typename T, typename P1>
	static Callback* bind(T *obj, void (T::*F)(P1), P1 p1) {
		return new CallbackP1<T, P1>(obj, F, p1);
	}
};

template<typename T>
class CallbackP0 : public Callback {
	typedef void (T::*F)();

public:
	CallbackP0(T *obj, F f) : _obj(obj), _f(f) {}

	void run() { (_obj->*_f)(); }

private:
	T *_obj;
	F _f;
};

template<typename T, typename P1>
class CallbackP1 : public Callback {
	typedef void (T::*F)(P1);

public:
	CallbackP1(T *obj, F f, P1 p1) : _obj(obj), _f(f), _p1(p1) {}

	void run() { (_obj->*_f)(_p1); }

private:
	T *_obj;
	F _f;
	P1 _p1;
};

template<typename T, typename P1, typename P2>
class CallbackP2 : public Callback {
	typedef void (T::*F)(P1, P2);

public:
	CallbackP2(T *obj, F f, P1 p1, P2 p2) : _obj(obj), _f(f), _p1(p1), _p2(p2) {}

	void run() { (_obj->*_f)(_p1, _p2); }

private:
	T *_obj;
	F _f;
	P1 _p1;
	P2 _p2;
};

#endif // __CALLBACK_H_

