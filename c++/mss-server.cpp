#include <MSS.hpp>
#include <cstdio>
#include <string>
using namespace std;

int main() {
	MSS server;
	MSS_msg msg;
	string ip;
	server.init(5, 37812);

	while(true) {
		server.receive(msg);
		server.getClientIP(msg.client_id, ip);

		if(msg.text.length() == 0) { // special
			if(msg.isNewClient) { // connection
				if(msg.client_id == -1) { // all slots full
					printf("A client could not connect!\n");
				}
				else {
					printf("Connection from %s as ID %d!\n", ip.c_str(), msg.client_id);
				}
			}
			else { // disconnect
				printf("Client %s with ID %d disconnected!\n", ip.c_str(), msg.client_id);
			}
		}
		else { // message
			printf("Client %s with ID %d: %s", ip.c_str(), msg.client_id, msg.text.c_str());
		}
	}
}
