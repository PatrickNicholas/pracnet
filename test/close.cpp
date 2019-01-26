
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <iostream>
#include <string>

#include <network/ip/Socket.h>
#include <network/ip/SocketOwner.h>
#include <network/ip/tcp/Acceptor.h>
#include <network/ip/tcp/Connection.h>
#include <network/ip/tcp/Connector.h>
#include <network/exception/exception.h>

using namespace std;
using namespace network;
using namespace network::ip;
using namespace network::ip::tcp;

int server() {
    Acceptor acceptor = Socket::create(AF_INET, SOCK_STREAM, 0);
    InetAddress address = InetAddress::parseV4("127.0.0.1", 9490);
    acceptor.bind(address);
    acceptor.listen(10);

    signal(SIGPIPE, SIG_IGN);  // 忽略 SIGPIPE 信号

    // acceptor.setnonblocking();
    while (true) {
        Connection conn = acceptor.accept();

        sleep(2);
        for (;;) {
            for (char c = 'a'; c <= 'z'; c++) {
                if (conn.write(&c, 1)) {
                    return 0;
                }
                sleep(1);
            }
        }
    }
}

int client() {
    Connector connector = Socket::create(AF_INET, SOCK_STREAM, 0);
    InetAddress address = InetAddress::parseV4("127.0.0.1", 9490);
    connector.connect(address);

    char buf[128] = "start";
    connector.write(buf, 5);
    for (size_t idx = 0; idx < 1; idx++) {
        for (size_t j = 0; j < 10000; j++) {
            sleep(5);
            connector.write(buf, 5);
            size_t len = connector.read(buf, 128);
            if (len == 0 && connector.eof()) {

                connector.write(buf, 1);
                // return 0;
            }
            cout << string(buf, len) << endl;
        }
    }
    return 0;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        cout << "usage " << argv[0] << " client/server" << endl;
        return 1;
    }

    try {
        string opt(argv[1]);
        if (opt == "client") {
            return client();
        } else if (opt == "server") {
            return server();
        } else {
            cout << "usage " << argv[0] << " client/server" << endl;
            return 1;
        }
    } catch (Exception& e) {
        cout << "Exception " << e.message() << endl;
        return -1;
    }
}
