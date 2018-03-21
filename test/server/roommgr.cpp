#include "udt.h"
#include "clientsocket.h"
#include "roommgr.h"

void Room::add(TestClientHandler *c) {
	_clients[c->id()] = c;
	c->room(this);

	if (full()) {
		char data[16] = { 0 };
		*(uint32 *)data = 7;
		*(int16 *)(data + 4) = 0x0100;
		
		auto it = _clients.begin();
		it->second->send(data, 7);
		data[6] = 1;
		++it;
		it->second->send(data, 7);
	}
}

void Room::remove(int id) {
	char data[16] = { 0 };
	*(uint32 *)data = 6;
	*(int16 *)(data + 4) = 0x0101;
	broadcast(id, data);

	_clients.erase(id);
	/*for (auto it = _clients.begin(); it != _clients.end(); ++it) {
		it->second->kickoff();
	}
	_clients.clear();
	*/
}

void Room::broadcast(int id, const char *data) {
	for (auto it = _clients.begin(); it != _clients.end(); ++it) {
		uint32 size = *(uint32*)data;
		if (it->first != id) it->second->send(data, size);
	}
}

