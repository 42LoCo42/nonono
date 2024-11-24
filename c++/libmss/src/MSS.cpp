#include <unistd.h>
#include <arpa/inet.h>

#include "MSS.hpp"
using namespace std;

void error(const char* msg) {
	perror(msg);
	exit(EXIT_FAILURE);
}

void MSS::init(int _maxClients, int _port) {
	// init vars
	maxClients = _maxClients;
	port = _port;
	clientSockets = vector<int>(_maxClients);

	// create master socket
	if((masterSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0) error("socket creation");

	// this eases usage of multiple connection
	int opt = 1;
	if(setsockopt(masterSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0) error("setsockopt");

	// type of socket
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY; // localhost
	address.sin_port = htons(port);

	// bind the socket to localhost with port
	if(bind(masterSocket, (struct sockaddr *)&address, sizeof(address)) < 0) error("bind");

	// set socket to listen with 5 pending connections
	if(listen(masterSocket, 5) < 0) error("listen");

	addrlen = (socklen_t)sizeof(address);
}

MSS::~MSS() {
	for(int i=0; i<maxClients; ++i) {
		if(clientSockets[i]) close(clientSockets[i]);
	}
	close(masterSocket);
}

void MSS::receive(MSS_msg& msg) {
	// reset the socket set
	FD_ZERO(&socketSet);
	FD_SET(masterSocket, &socketSet);
	max_sd = masterSocket;

	int sd;
	for(int i=0; i<maxClients; ++i) {
		sd = clientSockets[i];
		if(sd > 0) FD_SET(sd, &socketSet); // add active clients to set
		if(sd > max_sd) max_sd = sd; // update max_sd
	}

	// wait for activity on one socket
	int activity = select(max_sd+1, &socketSet, NULL, NULL, NULL);
	if(activity < 0) error("select");

	if(FD_ISSET(masterSocket, &socketSet)) { // activity on master socket; this is a new client
		if((newSocket = accept(masterSocket, (struct sockaddr *)&address, &addrlen)) < 0) error("accept");
		msg.isNewClient = true;
		msg.text = "";
		msg.client_id = -1;

		// add client to list
		for(int i=0; i<maxClients; ++i) {
			if(!clientSockets[i]) {
				clientSockets[i] = newSocket;
				msg.client_id = i;
				break;
			}
		}

		if(msg.client_id == -1) { // no slot for the client, close its socket
			close(newSocket);
		}
	}
	else { // activity on a client socket, this is a message or disconnect
		for(int i=0; i<maxClients; ++i) {
			sd = clientSockets[i];

			if(FD_ISSET(sd, &socketSet)) {
				msg.isNewClient = false;
				msg.client_id = i;
				int msgLen = read(sd, buffer, bufSize - 1); // read message

				if(msgLen <= 1) { // no message (only EOF), this client disconnected TODO: specific message triggers disconnect?
					close(sd);
					clientSockets[i] = 0;
					FD_CLR(sd, &socketSet);
					msg.text = "";
				}
				else {
					buffer[msgLen] = '\0';
					msg.text = string(buffer);
				}
			}
		}
	}
}

void MSS::send(const string& text, int client_id) const {
	write(clientSockets[client_id], text.c_str(), text.length());
}

void MSS::close(int client_id) {
	if(client_id >= 0 && client_id < maxClients && clientSockets[client_id] != 0) { // client exists
		int clientSocket = clientSockets[client_id];

		close(clientSocket);
		FD_CLR(clientSocket, &socketSet);
		clientSockets[client_id] = 0;
	}
}

void MSS::getActiveClients(vector<int>& ids) const {
	ids = vector<int>();

	for(int i=0; i<maxClients; ++i) {
		if(clientSockets[i] > 0) ids.push_back(i);
	}
}

int MSS::getClientSocket(int client_id) const {
	if(client_id < 0 || client_id >= maxClients) {
		return 0;
	}
	return clientSockets[client_id];
}

void MSS::getClientIP(int client_id, string& ip) const {
	struct sockaddr_in address;
	socklen_t size = sizeof(address);
	getpeername(getClientSocket(client_id), (struct sockaddr*)&address, &size);
	ip = string(inet_ntoa(address.sin_addr));
}
