
#aux_source_directory(. SOURCE_FILES)
set(SOURCE_FILES
    InetAddress.cpp
    Socket.cpp
	SocketOwner.cpp

    tcp/Acceptor.cpp
	tcp/Connector.cpp
	tcp/Connection.cpp

    helper/socket.cpp
    )

add_library(ip ${SOURCE_FILES})

