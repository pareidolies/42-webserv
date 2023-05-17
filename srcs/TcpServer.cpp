# include "webserv.hpp"
#include "parsing/Server.hpp"

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

void			TcpServer::print_server(void)
{
	std::cout << ANSI_BLUE << "port: " << ANSI_RESET << _port << std::endl;
	std::cout << ANSI_BLUE << "host: " << ANSI_RESET << _host << std::endl;
	std::cout << ANSI_BLUE << "server names: " << ANSI_RESET << std::endl;
	for(std::vector<std::string>::iterator it = this->_serverName.begin(); it != this->_serverName.end(); it++)
	{	
		std::cout << (*it) << ANSI_RESET << std::endl;
	}
	std::cout << ANSI_BLUE << "root: " << ANSI_RESET << _root << std::endl;
	std::cout << ANSI_BLUE << "client max body size: " << ANSI_RESET << _clientMaxBodySize << std::endl;
	std::cout << ANSI_BLUE << "domain ip: " << ANSI_RESET << _domain << std::endl;
	std::cout << ANSI_BLUE << "service: " << ANSI_RESET << _service << std::endl;
	std::cout << ANSI_BLUE << "protocol: " << ANSI_RESET << _protocol << std::endl;
	std::cout << ANSI_BLUE << "interface: " << ANSI_RESET << _interface << std::endl;
	std::cout << ANSI_BLUE << "upload: " << ANSI_RESET << _upload << std::endl;
	std::cout << ANSI_BLUE << "cgi file extension: " << ANSI_RESET << _cgiFileExtension << std::endl;
	std::cout << ANSI_BLUE << "cgi path to script: " << ANSI_RESET << _cgiPathToScript << std::endl;
	std::cout << ANSI_BLUE << "maximum number of queued clients: " << ANSI_RESET << _backlog << std::endl;
	std::cout << ANSI_BLUE << "GET: " << ANSI_RESET << (_get ? "on" : "off" ) << std::endl;
	std::cout << ANSI_BLUE << "POST: " << ANSI_RESET  << (_post ? "on" : "off" ) << std::endl;
	std::cout << ANSI_BLUE << "DELETE: " << ANSI_RESET << (_delete ? "on" : "off" ) << std::endl;
	std::cout << ANSI_BLUE << "autoindex: " << ANSI_RESET << (_autoindex ? "on" : "off" ) << std::endl;
	std::cout << ANSI_BLUE << "error pages: " << ANSI_RESET << std::endl;
	for(std::map<int, std::string>::iterator it = _errorPages.begin(); it != _errorPages.end(); it++)
		std::cout << "[" << it->first << "] " << it->second << std::endl;
	for(std::vector<Location*>::iterator it = this->_locations.begin(); it != this->_locations.end(); it++)
	{	
		std::cout << ANSI_YELLOW << "LOCATION:" << ANSI_RESET << std::endl;
		(*it)->print_location();
		std::cout << std::endl;
	}
}

void TcpServer::init_var(Server *serv)
{
	_domain = serv->getDomain();
	_service = serv->getService();
	_protocol = serv->getProtocol();
	_interface = serv->getInterface();
	_backlog = serv->getBacklogs();
	_locations = serv->getLocations();
	_port = serv->getPort();
	_host = serv->getHost();
	_serverName = serv->getServerName();
	_clientMaxBodySize = serv->getClientMaxBodySize();
	_root = serv->getRoot();
	_index = serv->getIndex();
	_autoindex = serv->getAutoIndex();
	_cgiFileExtension = serv->getCgiFileExtension();
	_cgiPathToScript = serv->getCgiPathToScript();
	_upload = serv->getUpload();
	_get = serv->getGet();
	_post = serv->getPost();
	_delete = serv->getDelete();
	_errorPages = serv->getErrorPages();
	m_ip_address = _host;
	m_port = _port;
	m_socketAddress_len = sizeof(m_socketAddress);
	m_serverMessage = buildResponse();
	print_server();
}

TcpServer::TcpServer(Server *serv)
{
	init_var(serv);
	cout << "Initalizing the server." << endl;
	m_socketAddress.sin_family = _domain;
	m_socketAddress.sin_port = htons(_port);
	m_socketAddress.sin_addr.s_addr = inet_addr(_host.c_str());
	if (startServer() != 0)
	{
		ostringstream ss;
		ss << "Failed to start server with PORT: " << ntohs(m_socketAddress.sin_port);
		General::log(ss.str());
	}
}

TcpServer::TcpServer(string ip_address, int port) : 
	m_ip_address(ip_address), \
	m_port(port), m_new_socket(),\
	m_incomingMessage(), \
	m_socketAddress(),\
	m_socketAddress_len(sizeof(m_socketAddress)),\
	m_serverMessage(buildResponse())
{
	// create all sockets
	std::vector<Server*>::iterator it = _servers.begin();

	std::cout << ANSI_GREEN << "Setting virtual servers:" << ANSI_RESET << std::endl;

	int i = 1;
	for (; it != _servers.end(); it++)
	{
		std::cout << ANSI_GREEN << i << ". "<< (*it)->getHost() << ":"
				<< (*it)->getPort() << std::endl << ANSI_RESET;
		_socketList.push_back(Socket((*it)->getHost(), (*it)->getPort()));
		++i;
	}
}

TcpServer::~TcpServer()
{
	//cout << "Terminating the server." << endl;
	//closeServer();
}

int TcpServer::startServer()
{
    int optval = 1;

	m_socket = socket(_domain, _service, _protocol);
	if (m_socket < 0)
		General::exitWithError("Cannot create socket");
    if (setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR , (char *)&optval, sizeof(optval)) < 0)
        General::exitWithError("Failed to set SO_REUSEADDR. errno: ");
	if (bind(m_socket, (sockaddr *)&m_socketAddress, m_socketAddress_len) < 0)
		General::exitWithError("Cannot connect socket to address");
	return (0);
}

std::vector<Socket>::iterator TcpServer::check_event_fd(int event_fd) 
{
	std::vector<Socket>::iterator it = _socketList.begin();
	for (; it != _socketList.end(); it++)
		if (event_fd == (*it).getSocketFd())
			break;
	return it;
}

void	TcpServer::run(void)
{
	//session cookies ?
	struct epoll_event ev, events[MAX_EVENTS];
	int event_fds, epollfd;
	//std::map<int, Client> clients; //managing clients
	bool done = false; //check end of response & request

	//create the epoll instance
	epollfd = epoll_create(1);
	if (epollfd == -1)
		General::exitWithError("epoll_create");
	for(int i = 0;i < (int)_socketList.size();i++)
		add_event(epollfd, _socketList[i].getSocketFd(), EPOLLIN);

	General::log("\n====== Waiting for a new connection ======");

	while (42) 
	{
		General::log("\n====== Waiting for a new connection ======");
		acceptConnection(m_new_socket);
		getPayload(m_new_socket);
        parse_request(m_request, m_buffer);
        std::string response_str = process_request(m_request);
		sendResponse(response_str);
		memset(m_buffer, 0, sizeof(m_buffer));
		close(m_new_socket);
	}
	if (close(epollfd) == -1)
		General::exitWithError("close");
}

void TcpServer::getPayload(int& new_socket)
{
    std::string request_headers;
    std::string request_body;
    int content_length = 0;
    int bytesReceived = 0;

    // Read the request headers
    while (true) {
        int valread = recv(new_socket, m_buffer, sizeof(m_buffer), 0);
        if (valread == -1)
            General::exitWithError("Error in recv()");

        request_headers += std::string(m_buffer, valread);
        if (request_headers.find("\r\n\r\n") != std::string::npos)
            break; // Found the end of headers
    }

    // Find the Content-Length header
    std::string content_length_str = "Content-Length: ";
    std::string::size_type content_length_pos = request_headers.find(content_length_str);
    if (content_length_pos != std::string::npos) {
        content_length_pos += content_length_str.length();
        std::string::size_type end_of_line_pos = request_headers.find("\r\n", content_length_pos);
        if (end_of_line_pos != std::string::npos) {
            std::string content_length_value = request_headers.substr(content_length_pos, end_of_line_pos - content_length_pos);
            // content_length = std::stoi(content_length_value);
        }
    }

    // Read the request body if content length is specified
    if (content_length > 0) {
        request_body.resize(content_length);

        while (bytesReceived < content_length) {
            int bytesRead = recv(new_socket, &request_body[bytesReceived], content_length - bytesReceived, 0);
            if (bytesRead == -1)
                General::exitWithError("Error in recv()");
            bytesReceived += bytesRead;
        }
    }

    // Process the received payload
    m_request.raw_request = request_headers + request_body;
    General::log("\nReceived message: \n" + m_request.raw_request);
}

void TcpServer::acceptConnection(int &new_socket)
{
	new_socket = accept(m_socket, (sockaddr *)&m_socketAddress, &m_socketAddress_len);
	if (new_socket < 0)
	{
		ostringstream ss;
		ss << "Server failed to accept incoming connection from ADDRESS: "
			<< "; PORT: " ;
		General::exitWithError(ss.str());
	}
	int flag = fcntl(new_socket, F_GETFL, 0);
	if (fcntl(new_socket, F_SETFL, flag | O_NONBLOCK) < 0)
	{
		stringstream ss;
		ss << "Fnctl error : server failed to accept incoming connection from ADDRESS: "
			<< "; PORT: ";
		General::exitWithError(ss.str());
	}
	add_event(epollfd, new_socket, EPOLLIN);
	return (new_socket);
}

void TcpServer::getPayload(int m_new_socket)
{
    long unsigned int content_length = 0;
    char *body_start = NULL;
    char *content_length_str = NULL;
    int bytesReceived = 0;

    // Read the request headers to find the Content-Length header
    int valread = recv(m_new_socket, m_buffer, sizeof(m_buffer), 0);
    if (valread == -1)
        General::exitWithError("Error in recv()");

    // Find the start of the request body
    body_start = strstr(m_buffer, "\r\n\r\n");
    if (body_start != NULL) {
        body_start += 4;
        // Look for the Content-Length header
        content_length_str = strstr(m_buffer, "Content-Length: ");
        if (content_length_str != NULL) {
            content_length_str += strlen("Content-Length: ");
            content_length = atoi(content_length_str);
            // Make sure we have enough space in the buffer
            if (content_length > sizeof(m_buffer) - (body_start - m_buffer)) {
                General::exitWithError("Payload too large for buffer");
            }
            // Read the payload
            bytesReceived = recv(m_new_socket, body_start, content_length, 0);
            if (bytesReceived == -1)
                General::exitWithError("Error in recv()");
        }
    }
	m_request.raw_request = std::string(m_buffer);
    General::log("\nReceived message: \n" + string(m_buffer));
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
	{
	    General::log("Error sending response to client");
		return (false);
	}
    else
    {
	    General::log("------ Server Response sent to client ------\n");
	    // General::log("Responsed message: \n" + response_str);
		return (true);
    }
}