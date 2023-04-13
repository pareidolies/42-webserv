#include "webserv.hpp"

int main( const int, const char** )
{
    TcpServer server = TcpServer("127.0.0.1", 8001);
    server.startListen();

    return EXIT_SUCCESS;
}