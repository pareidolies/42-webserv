# include "webserv.hpp"

/*
	TCP server setup flow:
		socket: create socket
		bind: bind socket address
		listen: waiting for incoming request
		accept: accept incoming request
	
	chosen option:
		AF_INET		IPv4 Internet protocols
		SOCK_STREAM	Provides sequenced, reliable, two-way, connection-based byte streams.
		define 20 as the maximum length to which the queue of pending connections for sockfd may grow. 
*/

//ADD Socket by default

Socket::Socket(string ip_address, int port, Server	*server) : 
	m_ip_address(ip_address), \
	m_port(port), \
	m_socketAddress(),\
	m_socketAddress_len(sizeof(m_socketAddress))
{
	this->_server = server;

	m_socketAddress.sin_family = AF_INET;
	m_socketAddress.sin_port = htons(m_port);
	m_socketAddress.sin_addr.s_addr = inet_addr(m_ip_address.c_str());

	if (startServer() != 0)
	{
		ostringstream ss;
		ss << "Failed to start server with PORT: " << ntohs(m_socketAddress.sin_port);
		General::log(ss.str());
	}
    startListen();
}

Socket::~Socket()
{
	//cout << "Terminating the server." << endl;
	//closeServer();
}

int Socket::startServer()
{
    int optval = 1;

	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket < 0)
	{
		General::exitWithError("Cannot create socket");
	}
    if (setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, (char *)&optval, sizeof(optval)) < 0) //"SO_REUSEPORT" very important to avoid error if same port
	{
        General::exitWithError("Failed to set SO_REUSEADDR. errno: ");
	}
	if (bind(m_socket, (sockaddr *)&m_socketAddress, m_socketAddress_len) < 0)
	{
		General::exitWithError("Cannot connect socket to address");
	}
	return (0);
}

// void Socket::closeServer()
// {
// 	close(m_socket);
// 	close(m_new_socket);
// 	exit(0);
// }

void Socket::startListen()
{
	if (listen(m_socket, 1024) < 0)
		General::exitWithError("Socket listen failed");
	ostringstream ss;
	ss <<  ANSI_PURPLE << "\n*** Listening on ADDRESS: "  << inet_ntoa(m_socketAddress.sin_addr) 
		<< " PORT: "  << ntohs(m_socketAddress.sin_port) << " ***" << ANSI_RESET;
	General::log(ss.str());
}

int	Socket::getSocketFd(void) {
	return (this->m_socket);
}

Server *Socket::getServer(void) {
	return (this->_server);
}
