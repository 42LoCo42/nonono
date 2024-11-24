import std.stdio : writeln;
import std.algorithm;
import std.socket;

import undead.stream : FileMode;
import undead.socketstream;

void safeFun() @safe {
	writeln("Hello World");
	// allocating memory with the GC is safe too
	int* p = new int;
}

void unsafeFun() {
	int* p = new int;
	int* fiddling = p + 5;
}

void networkTest() {
	auto s = new SocketStream(new TcpSocket());
	scope(exit) s.close();

	s.socket.connect(getAddress("42loco42.duckdns.org", 37812)[0]);
	s.writeLine("foobar");
	s.readLine.writeln;
}

void main() {
	safeFun();
	unsafeFun();
	networkTest();

	"aaabbcdcc".group.writeln;
}
