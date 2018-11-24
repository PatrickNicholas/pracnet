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
        : conn_{std::move(conn)}, handler_{base, conn.socket()} {
        handler_.setReadCallback(std::bind(&EchoClient::read, this));
    }

    void enable() { handler_.enableReading(); }

    void read() {
        char buf[128];
        size_t count = conn_.read(buf, 128);
        if (count != 0) {
            conn_.write(buf, count);
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

        acceptor.setnonblocking();

        EventBase base;

        EventHandler ack{base, acceptor.socket()};
        std::unordered_map<EventID, std::shared_ptr<EchoClient>> clientMap;

        ack.enableReading();
        ack.setReadCallback([&clientMap, &base, &acceptor]() {
            Connection conn = acceptor.accept();
            shared_ptr<EchoClient> client =
                std::make_shared<EchoClient>(base, std::move(conn));
            clientMap[conn.socket()] = client;

            cout << "new connection" << endl;
            client->enable();
        });

        cout << "start event loop..." << endl;
        base.loop();
    } catch (Exception& e) {
        cout << "Exception " << e.message() << endl;
    }
    return 0;
}
