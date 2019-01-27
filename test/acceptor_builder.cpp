#include <network/ip/tcp/AcceptorBuilder.h>

using namespace network::ip::tcp;

int main() {
    AcceptorBuilder builder;

    InetAddress address = InetAddress::parseV4();
    Acceptor acceptor = builder.backlog(10).bind(address).builder();
}
