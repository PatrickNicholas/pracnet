#include <string.h>

#include <network/EventBase.h>
#include <network/ip/Socket.h>
#include <network/ip/SocketOwner.h>
#include <network/ip/tcp/Acceptor.h>
#include <network/ip/tcp/Connection.h>
#include <network/exception/exception.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <iostream>

using namespace std;
using namespace network;
using namespace network::ip;
using namespace network::ip::tcp;

int main(int argc, char **argv) {
	try {
		Acceptor acceptor = Socket::create(AF_INET, SOCK_STREAM, 0);
		InetAddress local = InetAddress::parseV4("127.0.0.1", 9490);
		acceptor.bind(local);
		acceptor.listen(10);
		
		cout << "start accept" << endl;
		do {
			Connection conn = acceptor.accept();
			cout << "new connection" << endl;
			const char *msg = "hello world";
			size_t count = conn.write(msg, strlen(msg));
			cout << "write " << count << endl;
		} while (true);
	}
	catch (Exception &e) {
		cout << "Exception " << e.message() << endl;
	}
	return 0;
}
