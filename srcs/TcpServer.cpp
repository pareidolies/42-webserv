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

TcpServer::TcpServer(Configuration conf) :
	_conf(conf),
	m_ip_address("127.0.0.1"), \
	m_port(8000), m_new_socket(),\
	m_incomingMessage(), \
	m_socketAddress(),\
	m_socketAddress_len(sizeof(m_socketAddress)),\
	m_serverMessage(buildResponse())
{
	this->_servers = conf.getServers();

	initSocket();
	createEventManager();
	while(isWaiting())
	{
		std::cout << "\033[1;97mListening ...\033[0m" << std::flush;
		std::cout << "\r";
	}
	std::cout << "\033[1;31mStop Webserv\033[0m" << std::endl;
	closeSockets();
	
	// if (startServer() != 0)
	// {
	// 	ostringstream ss;
	// 	ss << "Failed to start server with PORT: " << ntohs(m_socketAddress.sin_port);
	// 	General::log(ss.str());
	// }
}

TcpServer::~TcpServer()
{
    cout << "Terminating the server." << endl;
	closeSocket();
}

void TcpServer::initServer()
{
	int				opt = 1;
	int				serv_socket;
	int				ret;
	struct addrinfo	hints;
	struct addrinfo	*res = NULL;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	cout << "Initalizing the servers." << endl;

	for (size_t i = 0; i < _servers.size(); i++)
	{
		if ((ret = getaddrinfo(_servers[i].getHost().c_str(), _servers[i].getPort().c_str(), &hints, &res)) != 0)
			return;
			//return (this->closeFdSocket(i, ret));

		if ((serv_socket = socket((int)res->ai_family, (int)res->ai_socktype, (int)res->ai_protocol)) == -1)
			return;
			//return (errorSocket("socket call failed", res));

		setsockopt(serv_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

		this->_servers[i].setSocket(serv_socket);
		this->_servers[i].setAddress(getAddressInfo(*res->ai_addr));
		this->_serversFd.push_back(serv_socket);

		if (bind(serv_socket, res->ai_addr, res->ai_addrlen) == -1)
			return;
			//return (errorSocket("bind call failed", res));
		
		freeaddrinfo((struct addrinfo *)res);
		res = NULL;

		// if (nonBlockFd(serv_socket)) //non block a voir
		// 	return ;
		if (listen(serv_socket, NB_EVENTS) == -1)
			return;
			//return (errorSocket("Listen call failed", res));
	}
	// m_socketAddress.sin_family = AF_INET;
	// m_socketAddress.sin_port = htons(m_port);
	// m_socketAddress.sin_addr.s_addr = inet_addr(m_ip_address.c_str());
}

// int TcpServer::startServer()
// {
//     int optval = 1;

// 	m_socket = socket(AF_INET, SOCK_STREAM, 0);
// 	if (m_socket < 0)
// 		General::exitWithError("Cannot create socket");
//     if (setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR , (char *)&optval, sizeof(optval)) < 0)
//         General::exitWithError("Failed to set SO_REUSEADDR. errno: ");
// 	if (bind(m_socket, (sockaddr *)&m_socketAddress, m_socketAddress_len) < 0)
// 		General::exitWithError("Cannot connect socket to address");
// 	return (0);
// }

void	TcpServer::createEventManager() 
{
	struct epoll_event	event;
	int					j = 0;

	memset(&event, 0, sizeof(event));
	this->_epollFd = epoll_create(NB_EVENTS);
	if (this->_epollFd == -1)
	{
		return;
		// this->_errSocket = true;
		// perror("err epoll_create");
	}
	// if (this->nonBlockFd(this->_epollFd) == 1)
	// 	return ;
	for (size_t i = 0; i < this->_servers.size(); i++)
	{
			event.events = EPOLLIN;
			event.data.fd = this->_serversFd[j];
			if (epoll_ctl(this->_epollFd, EPOLL_CTL_ADD, this->_serversFd[j], &event) == -1)
			{
				return;
				// this->_errSocket = true;
				// perror("err epoll_ctl");
			}
			j++;
	}
	event.events = EPOLLIN;
	event.data.fd = 0;
	if (epoll_ctl(this->_epollFd, EPOLL_CTL_ADD, 0, &event) == -1)
	{
		return;
		// this->_errSocket = true;
		// perror("err epoll_ctl");
	}
}

void TcpServer::closeServer()
{
	close(m_socket);
	close(m_new_socket);
	exit(0);
}

bool TcpServer::isWaiting()
{
	struct epoll_event	event[NB_EVENTS];
	int					nbrFd;
	int					index_serv;
	int					index_wreq;

	nbrFd = epoll_wait(this->_epollFd, event, NB_EVENTS, 1000);
	if (nbrFd == -1)
	{
		// perror("epoll_wait");
		// this->_errSocket = true;
		return false;
	}
	for (int j = 0; j < nbrFd; j++)
	{
		if (event[j].data.fd == 0)
			return false;
		acceptConnection(m_new_socket);
		getPayload(m_new_socket);
	    parse_request(m_request, m_buffer);
     	std::string response_str = process_request(m_request);
		sendResponse(response_str);
		close(m_new_socket);

		// else if ((index_serv = isServerFd(event[j].data.fd)) >= 0)
		// 	createConnection(index_serv);
		// else
		// {
		// 	Request	req(event[j].data.fd, this->_servers, this->_clientServerFds, this->_epollFd);

		// 	if ((index_wreq = isAwaitingRequest(event[j].data.fd)) != -1)
		// 		req = this->_awaitingRequest[index_wreq];

		// 	req.request(event[j].data.fd);

		// 	if (!req.getAwaitingRequest() && index_wreq != -1)
		// 			this->_awaitingRequest.erase(this->_awaitingRequest.begin() + index_wreq);

		// 	if (req.getcloseConnection())
		// 		this->closeConnection(event[j].data.fd);
		// 	else if (req.getAwaitingRequest())
		// 	{
		// 		if (this->isAwaitingRequest(event[j].data.fd) == (size_t)-1)
		// 			this->_awaitingRequest.push_back(req);
		// 		else
		// 			this->_awaitingRequest[index_wreq] = req;
		// 	}
		// 	else
		// 	{
		// 		Response	rep(req, req.getServBlock(), this->_envp);

		// 		rep.sendData();
		// 		if (rep.getCloseConnection() && !req.getAwaitingRequest())
		// 			this->closeConnection(event[j].data.fd);
		// 	}
	}
	return true;
}


// void TcpServer::startListen()
// {
// 	if (listen(m_socket, 20) < 0)
// 		General::exitWithError("Socket listen failed");
// 	ostringstream ss;
// 	ss << "\n*** Listening on ADDRESS: "  << inet_ntoa(m_socketAddress.sin_addr) 
// 		<< " PORT: "  << ntohs(m_socketAddress.sin_port) << " ***";
// 	General::log(ss.str());

// 	int bytesReceived;
// 	while (42)
// 	{
// 		General::log("\n====== Waiting for a new connection ======");
// 		acceptConnection(m_new_socket);
// 		getPayload(m_new_socket);
//      parse_request(m_request, m_buffer);
//      std::string response_str = process_request(m_request);
// 		sendResponse(response_str);
// 		close(m_new_socket);
// 	}
// }

void	TcpServer::getPayload(int &new_socket)
{
	int bytesReceived;

	int valread = recv(m_new_socket, m_buffer, sizeof(m_buffer), 0);
	if (valread == -1)
		General::exitWithError("Error in recv()");
	else
		General::log("\nReceived message: \n" + string(m_buffer));
}

void TcpServer::acceptConnection(int &new_socket)
{
	new_socket = accept(m_socket, (sockaddr *)&m_socketAddress, &m_socketAddress_len);
	if (new_socket < 0)
	{
		ostringstream ss;
		ss << "Server failed to accept incoming connection from ADDRESS: "
			<< inet_ntoa(m_socketAddress.sin_addr) 
			<< "; PORT: " << ntohs(m_socketAddress.sin_port);
		General::exitWithError(ss.str());
	}
}

string TcpServer::buildResponse()
{
	string htmlFile = "<!DOCTYPE html><html lang=\"en\"><body><h1> HOME </h1><p> Hello from your Server :) </p></body></html>";
	ostringstream ss;
	ss << "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " << htmlFile.size() << "\n\n"
		<< htmlFile;
	return ss.str();
}

void TcpServer::sendResponse(std::string response_str)
{
    if (send(m_new_socket, response_str.c_str(), response_str.size(), 0) < 0)
	    General::log("Error sending response to client");
    else
    {
	    General::log("------ Server Response sent to client ------\n");
	    // General::log("Responsed message: \n" + response_str);
    }
}
