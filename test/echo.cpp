#include <string.h>

#include <memory>
#include <queue>
#include <unordered_map>

#include <network/EventBase.h>
#include <network/EventHandler.h>
#include <network/exception/exception.h>
#include <network/ip/Socket.h>
#include <network/ip/SocketOwner.h>
#include <network/ip/tcp/Acceptor.h>
#include <network/ip/tcp/Connection.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <iostream>

using namespace std;
using namespace network;
using namespace network::ip;
using namespace network::ip::tcp;

class EchoClient {
   public:
    EchoClient(EventBase& base, Connection&& conn)
        : conn_{std::move(conn)}, handler_{base, conn_.socket()} {}
    ~EchoClient() { cout << "i am killed, seeya!" << endl; }

    void enable() {
        handler_.setReadCallback(std::bind(&EchoClient::read, this));
        handler_.enableReading();
    }

    void read() {
        cout << "client " << conn_.socket() << " readable" << endl;
        char buf[128];
        size_t count = conn_.read(buf, 128);
        if (count != 0) {
            conn_.write(buf, count);
        } else if (conn_.eof()) {
            cout << "client " << conn_.socket() << " closed" << endl;
            handler_.disableReading();
            delete this;
        }
    }

   private:
    Connection conn_;
    EventHandler handler_;

    queue<char> buffered_;
};

int main(int argc, char** argv) {
    try {
        Acceptor acceptor = Socket::create(AF_INET, SOCK_STREAM, 0);
        InetAddress local = InetAddress::parseV4("127.0.0.1", 9490);
        acceptor.bind(local);
        acceptor.listen(10);

        cout << "start acceptor at 127.0.0.1:9490" << endl;

        acceptor.setnonblocking();

        EventBase base;

        EventHandler ack{base, acceptor.socket()};

        ack.enableReading();
        ack.setReadCallback([&base, &acceptor]() {
            Connection conn = acceptor.accept();
            EchoClient* client = new EchoClient(base, std::move(conn));

            cout << "new connection" << endl;
            client->enable();
        });

        cout << "start event loop..." << endl;
        base.loop();
    } catch (Exception& e) {
        cout << "Exception " << e.message() << endl;
        return -1;
    }
    return 0;
}
