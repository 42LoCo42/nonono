#pragma once
#include <string>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>

/**
* In receive():
* 	client_id is the client associated with this MSS_msg or -1 if a client connected when all slots are full
*	isNewClient is true on connect and false on message or disconnect
*	text.length() is >0 on message and 0 on (dis-)connect
*/
struct MSS_msg {
	int client_id;
	std::string text;
	bool isNewClient;
};

void error(const char* msg);

class MSS {
	int maxClients, port;
	int masterSocket, newSocket, max_sd; // max_sd = maximum socket file descriptor for select()
	std::vector<int> clientSockets;

	struct sockaddr_in address;
	socklen_t addrlen;
	fd_set socketSet;

	constexpr static int bufSize = 1025;
	char buffer[bufSize];

public:
	~MSS();

	void init(int _maxClients, int _port);
	void receive(MSS_msg& msg);
	void send(const std::string& text, int client_id) const;
	void close(int client_id);

	void getActiveClients(std::vector<int>& ids) const;
	int getClientSocket(int client_id) const;
	void getClientIP(int client_id, std::string& ip) const;
};
