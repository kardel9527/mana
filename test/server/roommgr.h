#include <map>
#include <vector>
#include "singleton.h"

class TestClientHandler;

class Room {
public:
	void add(TestClientHandler *c) ;

	void remove(int id);

	void broadcast(int id, const char *data);

	bool full() const { return _clients.size() >= 2; }

private:
	std::map<int, TestClientHandler *> _clients;
};

class RoomMgr : public kcommon::Singleton<RoomMgr> {
public:
	RoomMgr() { for (int i = 0; i < 100; ++i) { _rooms.push_back(Room()); }}

	void on_client_enter(TestClientHandler *c) {
		for (auto it = _rooms.begin(); it != _rooms.end(); ++it) {
			if (it->full()) continue;

			it->add(c);
			break;
		}
	}

private:
	std::vector<Room> _rooms;
};

