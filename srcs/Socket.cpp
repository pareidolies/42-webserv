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
}

int Socket::startServer()
{
    int optval = 1;
	int ret;
	struct addrinfo	hints;
	struct addrinfo	*res = NULL;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if ((ret = getaddrinfo(m_ip_address.c_str(), General::to_string(m_port).c_str(), &hints, &res)) != 0)
		General::exitWithError("Cannot create socket");

	if ((m_socket = socket((int)res->ai_family, (int)res->ai_socktype, (int)res->ai_protocol)) == -1)
		General::exitWithError("Cannot create socket");
    if (setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, (char *)&optval, sizeof(optval)) < 0) //"SO_REUSEPORT" very important to avoid error if same port
	{
        General::exitWithError("Failed to set SO_REUSEADDR. errno: ");
	}
	this->_server->setAddress(getAddressInfo(*res->ai_addr));
	if (bind(m_socket, res->ai_addr, res->ai_addrlen) < 0)
	{
		General::exitWithError("Cannot connect socket to address");
	}
	freeaddrinfo((struct addrinfo *)res);
	res = NULL;
	return (0);
}

void Socket::startListen()
{
	if (listen(m_socket, 1024) < 0)
		General::exitWithError("Socket listen failed");
	ostringstream ss;
	ss <<  ANSI_PURPLE << "\n*** Listening on ADDRESS: "  << _server->getHost() 
		<< " PORT: "  << ntohs(m_socketAddress.sin_port) << " ***\n" << ANSI_RESET;
	General::log(ss.str());
}

int	Socket::getSocketFd(void) {
	return (this->m_socket);
}

Server *Socket::getServer(void) {
	return (this->_server);
}

std::string		Socket::getAddressInfo(const struct sockaddr addr)
{
	char	address[50];

	if (getnameinfo(&addr, sizeof(addr), address, sizeof(address), 0, 0, NI_NUMERICHOST) == -1)
		std::cerr << "getnameinfo() call failed" << std::endl;
	return (address);
}