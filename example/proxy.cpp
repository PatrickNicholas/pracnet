#include <getopt.h>
#include <stdlib.h>

#include <functional>
#include <iostream>
#include <string>
#include <vector>

#include <network/EventBase.h>
#include <network/EventHandler.h>
#include <network/exception/exception.h>
#include <network/ip/Socket.h>
#include <network/ip/SocketOwner.h>
#include <network/ip/tcp/Acceptor.h>
#include <network/ip/tcp/Connection.h>

using namespace std;
using namespace network;
using namespace network::ip;
using namespace network::ip::tcp;

const const char* kLocalAddress = "0.0.0.0";
constexpr int16_t kDefaultProxyPort = 9490;

struct ProxyConfig {
    int16_t port;
    bool daemon;
    std::string remote;
};

ProxyConfig config{kDefaultProxyPort, false, ""};

void print_usage(const char* program) {
    cout << program << " " << endl;
    cout << endl;
    cout << "\t\ta simple proxy" << endl;
    cout << '\t' << "-d run proxy in daemon" << endl;
    cout << '\t' << "-p number set port proxy listen" << endl;
    cout << '\t' << "-r addresss remote address" << endl;
    exit(-1);
}

void parse_config(int argc, char** argv) {
    char opt;
    while ((opt = getopt(argc, argv, "d:p:r")) != -1) {
        switch (opt) {
            case 'd':
                config.daemon = true;
                break;
            case 'p':
                config.port = atoi(optarg);
                break;
            case 'r':
                config.remote = optarg;
                break;
            case ':':
            case '?':
            default:
                print_usage();
        }
    }

    return 0;
}

typedef char byte_t;

// class Buffer {
//    public:
//     Buffer(const Buffer&) = delete;
//     Buffer& operator=(const Buffer&) = delete;
//
//     explicit Buffer(size_t size = 1024) : from_index_{0}, buffer_{1024} {}
//
//     void append(const byte_t* data, size_t length) {
//         if (left() < length) {
//             buffer_.reserve(capacity() * 2);
//         }
//
//         const byte_t* end = data + length;
//         while (data < end) {
//             buffer_.push_back(data);
//         }
//     }
//
//     void drain(size_t length) {
//         assert(size() >= length);
//         from_index_ += length;
//         if (empty()) {
//             reset();
//         }
//     }
//
//     const byte_t* data() { return buffer_.data() + from_index_; }
//
//     size_t size() const { return buffer_.size() - from_index_; }
//
//     size_t empty() const { return from_index_ == buffer_.size(); }
//
//    private:
//     size_t capacity() const { return buffer_.capacity(); }
//
//     size_t left() const { return capacity() - buffer_.size(); }
//
//     void reset() {
//         from_index_ = 0;
//         buffer_.clear();
//     }
//
//     size_t from_index_;
//     std::vector<char> buffer_;
// };

class BufferedEventHandler final {
   public:
    using MessageCallback = std::functional<void()>;

    BufferedEventHandler(const BufferedEventHandler&) = delete;
    BufferedEventHandler& operator=(const BufferedEventHandler&) = delete;

    BufferedEventHandler(EventBase& base, Connection&& conn)
        : conn_{std::move(conn)}, handler_{base, conn.socket()} {
        message_callback_ = [this] { defaultMessageCallback(); };

        conn.setnonblocking();
        handler_.setReadCallback([this] { processRead(); });
        handler_.setWriteCallback([this] { processWrite(); });
        handler_.setErrorCallback([this] { processError(); });

        handler_.enableReading();
    }

    void write(const byte_t* data, size_t length) {
        if (write_buffer_.empty()) {
            size_t writen = conn_.write(data, length);
            if (writen > 0) {
                data += writen;
                length -= writen;
            }
        }

        write_buffer_.append(data, length);
        if (!handler_.isWriting()) {
            handler_.enableWriting();
        }
    }

    void setMessageCallback(const MessageCallback& cb) {
        message_callback_ = cb;
    }

   private:
    void processRead() {
        size_t readn = readToBuffer();
        if (readn > 0) {
            message_callback_(read_buffer_);
        }
    }

    void processWrite() {
        drainWriteBuffer();
        if (write_buffer_.empty()) {
            handler_.disableWriting();
        }
    }

    void processError() {}

    void drainWriteBuffer() {
        size_t total = write_buffer_.size();
        size_t writen = conn_.write(write_buffer_, total);
        if (writen > 0) {
            drain(writen);
        }
    }

    void readToBuffer() {
        char buffer[1024] = {'\0'};

        size_t size = conn_.read(buffer, 1024);
        if (size > 0) {
            read_buffer_.append(buffer, size);
        }
    }

    void defaultMessageCallback() {}

    Connection conn_;
    EventHandler handler_;

    Buffer read_buffer_;
    Buffer write_buffer_;
    size_t read_high_water_mark_;
    size_t write_high_water_mark_;

    MessageCallback message_callback_;
};

class HttpContext final {
   public:
};

class HttpRequest final {
   public:
    template <typename Type>
    Type header(const std::string& data) const {
        return header(data.data());
    }

    template <typename Type>
    Type header(const char* data) const {}

   protected:
    enum Version { kV1, kV11 };
};

class HttpResponse final {
   public:
};

class HttpClient final {
   public:
    HttpClient(const HttpClient& client) = delete;
    HttpClient& operator=(HttpClient&) = delete;

    HttpClient(EventBase& base, const InetAddress& address);

    HttpResponse do(const HttpRequest& req) const;

   private:
};

class HttpResponseParser final {
   public:

};

class HttpRequestWriter final {};
class HttpResponseWriter final {};

class HttpRequestParser {
   public:
    HttpRequestParser(const HttpRequestParser&) = delete;
    HttpRequestParser& operator=(const HttpRequestParser&) = delete;

    void parse(Buffer& buffer) {
        const byte_t* data = buffer.data();
        size_t size = buffer.size();

        const byte_t* end = data + size;

        bool cr = false;
        while (state_ == kHeader && data < end) {
            line_.push_back(*data);
            switch (*data) {
                case '\r':
                    cr = true;
                    break;
                case '\n':
                    if (cr) {
                        line.append('\0');
                        processLine();
                    }
                default:
                    cr = false;
            }
            data += 1;
        }

        while (data < end) {
            body_.push_back(*data++);
        }
    }

    bool ExtractAndReset(HttpRequest& req) {
        // TODO(maochuan)
        line_.clear();
        headers_.clear();
        body_.clear();
        state_ = kHeader;
    }

   private:
    void processLine() {
        if (line_ == "\r\n") {
            state_ = kBody;
        } else {
            headers_.emplace_back();
            std::swap(headers_.back(), line_);
        }
    }

    enum { kHeader, kBody };
    unsigned state_ = kHeader;
    vector<byte_t> line_;
    vector<vector<byte_t>> headers_;
    vector<byte_t> body_;
};

class Client final {
   public:
    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;

    Client(EventBase& base, Connection&& conn)
        : handler_{base, std::move(conn)} {
        handler_.setMessageCallback(
            [this](Buffer& buffer) { processMessage(buffer); });
    }

   private:
    void processMessage(Buffer& buffer) { parser_.parse(buffer); }

    void requestComplete() {
        InetAddress address = InetAddress::parseV4(config.remote);
        Connector connector = Socket::create(AF_INET, SOCK_STREAM, 0);
        connector.connect(address);
    }

    EventBase& base_;
    BufferedEventHandler handler_;
    HttpParser parser_;
};

class AcceptorHandler final {
   public:
    AcceptorHandler(const AcceptorHandler&) = delete;
    AcceptorHandler& operator=(const AcceptorHandler&) = delete;

    AcceptorHandler(EventBase& base, Acceptor& acceptor)
        : handler_{base, acceptor.socket()} {
        acceptor.setnonblocking();
        handler.setReadCallback([this]() { accept(); });
    }

    void enable() { handler_.enableReading(); }
    void disable() { handler_.disableReading(); }

   private:
    void accept() {
        Connection conn = acceptor.accept();
        Client* client = new Client(base, std::move(conn));
        // client->enable();
    }

    EventHandler handler_;
};

int start_server(EventBase& base) {
    try {
        InetAddress address = InetAddress::parseV4(kLocalAddress, config.port);
        Acceptor acceptor = Socket::create(AF_INET, SOCK_STREAM, 0);
        acceptor.bind();
        acceptor.listen(128);

        cout << "listen " << address << endl;

        AcceptorHandler ack{base, acceptor};
        ack.enable();

        cout << "start accept requests" << endl;

        base.loop();
        return 0;
    } catch (Exception& e) {
        cout << e.what() << endl;
        return 1;
    }
}

int main(int argc, char** argv) {
    parse_config(argc, argv);

    EventBase base;
    return start_server(base);
}
