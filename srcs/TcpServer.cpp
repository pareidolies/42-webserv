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

// void			TcpServer::print_server(void)
// {
// 	std::cout << ANSI_BLUE << "port: " << ANSI_RESET << _port << std::endl;
// 	std::cout << ANSI_BLUE << "host: " << ANSI_RESET << _host << std::endl;
// 	std::cout << ANSI_BLUE << "server names: " << ANSI_RESET << std::endl;
// 	for(std::vector<std::string>::iterator it = this->_serverName.begin(); it != this->_serverName.end(); it++)
// 	{	
// 		std::cout << (*it) << ANSI_RESET << std::endl;
// 	}
// 	std::cout << ANSI_BLUE << "root: " << ANSI_RESET << _root << std::endl;
// 	std::cout << ANSI_BLUE << "client max body size: " << ANSI_RESET << _clientMaxBodySize << std::endl;
// 	std::cout << ANSI_BLUE << "domain ip: " << ANSI_RESET << _domain << std::endl;
// 	std::cout << ANSI_BLUE << "service: " << ANSI_RESET << _service << std::endl;
// 	std::cout << ANSI_BLUE << "protocol: " << ANSI_RESET << _protocol << std::endl;
// 	std::cout << ANSI_BLUE << "interface: " << ANSI_RESET << _interface << std::endl;
// 	std::cout << ANSI_BLUE << "upload: " << ANSI_RESET << _upload << std::endl;
// 	std::cout << ANSI_BLUE << "cgi file extension: " << ANSI_RESET << _cgiFileExtension << std::endl;
// 	std::cout << ANSI_BLUE << "cgi path to script: " << ANSI_RESET << _cgiPathToScript << std::endl;
// 	std::cout << ANSI_BLUE << "maximum number of queued clients: " << ANSI_RESET << _backlog << std::endl;
// 	std::cout << ANSI_BLUE << "GET: " << ANSI_RESET << (_get ? "on" : "off" ) << std::endl;
// 	std::cout << ANSI_BLUE << "POST: " << ANSI_RESET  << (_post ? "on" : "off" ) << std::endl;
// 	std::cout << ANSI_BLUE << "DELETE: " << ANSI_RESET << (_delete ? "on" : "off" ) << std::endl;
// 	std::cout << ANSI_BLUE << "autoindex: " << ANSI_RESET << (_autoindex ? "on" : "off" ) << std::endl;
// 	std::cout << ANSI_BLUE << "error pages: " << ANSI_RESET << std::endl;
// 	for(std::map<int, std::string>::iterator it = _errorPages.begin(); it != _errorPages.end(); it++)
// 		std::cout << "[" << it->first << "] " << it->second << std::endl;
// 	for(std::vector<Location*>::iterator it = this->_locations.begin(); it != this->_locations.end(); it++)
// 	{	
// 		std::cout << ANSI_YELLOW << "LOCATION:" << ANSI_RESET << std::endl;
// 		(*it)->print_location();
// 		std::cout << std::endl;
// 	}
// }

void TcpServer::init_var(Server *serv)
{
	m_request._domain = serv->getDomain();
	m_request._service = serv->getService();
	m_request._protocol = serv->getProtocol();
	m_request._interface = serv->getInterface();
	m_request._backlog = serv->getBacklog();
	m_request._locations = serv->getLocations();
	m_request._port = serv->getPort();
	m_request._host = serv->getHost();
	m_request._serverName = serv->getServerName();
	m_request._clientMaxBodySize = serv->getClientMaxBodySize();
	m_request._root = serv->getRoot();
	m_request._index = serv->getIndex();
	m_request._autoindex = serv->getAutoindex();
	m_request._cgiFileExtension = serv->getCgiFileExtension();
	m_request._cgiPathToScript = serv->getCgiPathToScript();
	m_request._upload = serv->getUpload();
	m_request._get = serv->getGet();
	m_request._post = serv->getPost();
	m_request._delete = serv->getDelete();
	m_request._errorPages = serv->getErrorPages();
	m_serverMessage = buildResponse();
	// print_server();
}

TcpServer::TcpServer(Configuration conf): _servers(conf.getServers())
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

// TcpServer::TcpServer(string ip_address, int port) : 
// 	m_ip_address(ip_address), \
// 	m_port(port), m_new_socket(),\
// 	m_incomingMessage(), \
// 	m_socketAddress(),\
// 	m_socketAddress_len(sizeof(m_socketAddress)),\
// 	m_serverMessage(buildResponse())
// {
// 	// create all sockets
// 	std::vector<Server*>::iterator it = _servers.begin();

// 	std::cout << ANSI_GREEN << "Setting virtual servers:" << ANSI_RESET << std::endl;

// 	int i = 1;
// 	for (; it != _servers.end(); it++)
// 	{
// 		std::cout << ANSI_GREEN << i << ". "<< (*it)->getHost() << ":"
// 				<< (*it)->getPort() << std::endl << ANSI_RESET;
// 		_socketList.push_back(Socket((*it)->getHost(), (*it)->getPort()));
// 		++i;
// 	}
// }

TcpServer::~TcpServer()
{
	//cout << "Terminating the server." << endl;
	//closeServer();
}


void TcpServer::add_event(int epollfd, int fd, int state)
{
	struct epoll_event ev;

	ev.events = state;
	ev.data.fd = fd;

	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev) == -1)
		General::exitWithError("epoll_ctl");
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


		event_fds = epoll_wait(epollfd, events, MAX_EVENTS, -1);

		if (event_fds == -1 && errno != EINTR)
			General::exitWithError("epoll_wait");

		// Loop that handle events happening on server fd and connections fds
		
		for (int n = 0; n < event_fds; ++n) {

			if (events[n].events & EPOLLRDHUP || events[n].events & EPOLLERR || events[n].events & EPOLLHUP)
			{
				General::exitWithError("event error");
				continue;
			}

			// Accepting a new connection
			std::vector<Socket>::iterator it = check_event_fd(events[n].data.fd);
			int server_id = it - _socketList.begin() - 1;
			if (it != _socketList.end()) 
			{
				acceptConnection(events[n], epollfd);
			}

			// Receiving request
			else if (events[n].events & EPOLLIN) 
			{
				getPayload(events[n].data.fd);
				init_var(_servers[server_id]);
				done = parse_request(m_request, m_buffer); // rajout _servers[server_id]
				if (done)
				{
					ev.events = EPOLLOUT;
					ev.data.fd = events[n].data.fd;
					if (epoll_ctl(epollfd, EPOLL_CTL_MOD, events[n].data.fd, &ev) == -1)
						General::exitWithError("epoll_create");
				}
			}
			// Sending response
			else if (events[n].events & EPOLLOUT) 
			{
				init_code_msg();
				std::string response_str = process_request(m_request);
				done = sendResponse(response_str, events[n].data.fd);
				if (done)
				{
					memset(m_buffer, 0, sizeof(m_buffer));
					epoll_ctl(epollfd, EPOLL_CTL_DEL, events[n].data.fd, &ev);
					close(events[n].data.fd);
					General::log("\n====== Waiting for a new connection ======");
				}
			}
			else 
				continue ;
		}
	}
	if (close(epollfd) == -1)
		General::exitWithError("close");
}

int	TcpServer::acceptConnection(struct epoll_event ev, int epollfd)
{
	struct sockaddr_storage addr;
	socklen_t socklen = sizeof(addr);

	int new_socket = accept(ev.data.fd, (sockaddr *)&addr, &socklen);
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


void TcpServer::init_code_msg()
{
	m_response.status_list[200] = "OK";
	m_response.status_list[201] = "Created";
	m_response.status_list[202] = "Accepted";
	m_response.status_list[203] = "Non-Authoritative Information";
	m_response.status_list[204] = "No Content";
	m_response.status_list[205] = "Reset Content";
	m_response.status_list[206] = "Partial Content";
	m_response.status_list[207] = "Multi-Status";
	m_response.status_list[208] = "Already Reported";
	m_response.status_list[226] = "IM Used";
	m_response.status_list[300] = "Multiple Choices";
	m_response.status_list[301] = "Moved Permanently";
	m_response.status_list[302] = "Found";
	m_response.status_list[303] = "See Other";
	m_response.status_list[304] = "Not Modified";
	m_response.status_list[305] = "Use Proxy";
	m_response.status_list[306] = "Switch Proxy";
	m_response.status_list[307] = "Temporary Redirect";
	m_response.status_list[308] = "Permanent Redirect";
	m_response.status_list[400] = "Bad Request";
	m_response.status_list[401] = "Unauthorized";
	m_response.status_list[402] = "Payment Required";
	m_response.status_list[403] = "Forbidden";
	m_response.status_list[404] = "Not Found";
	m_response.status_list[405] = "Method Not Allowed";
	m_response.status_list[406] = "Not Acceptable";
	m_response.status_list[407] = "Proxy Authentication Required";
	m_response.status_list[408] = "Request Timeout";
	m_response.status_list[409] = "Conflict";
	m_response.status_list[410] = "Gone";
	m_response.status_list[411] = "Length Required";
	m_response.status_list[412] = "Precondition Failed";
	m_response.status_list[413] = "Payload Too Large";
	m_response.status_list[414] = "URI Too Long";
	m_response.status_list[415] = "Unsupported Media Type";
	m_response.status_list[416] = "Range Not Satisfiable";
	m_response.status_list[417] = "Expectation Failed";
	m_response.status_list[418] = "I\'m a teapot";
	m_response.status_list[421] = "Misdirected Request";
	m_response.status_list[422] = "Unprocessable Entity";
	m_response.status_list[423] = "Locked";
	m_response.status_list[424] = "Failed Dependency";
	m_response.status_list[425] = "Too Early";
	m_response.status_list[426] = "Upgrade Required";
	m_response.status_list[428] = "Precondition Required";
	m_response.status_list[429] = "Too Many Requests";
	m_response.status_list[431] = "Request Header Fields Too Large";
	m_response.status_list[451] = "Unavailable For Legal Reasons";
	m_response.status_list[500] = "Internal Server Error";
	m_response.status_list[501] = "Not Implemented";
	m_response.status_list[502] = "Bad Gateway";
	m_response.status_list[503] = "Service Unavailable";
	m_response.status_list[504] = "Gateway Timeout";
	m_response.status_list[505] = "HTTP Version Not Supported";
	m_response.status_list[506] = "Variant Also Negotiates";
	m_response.status_list[507] = "Insufficient Storage";
	m_response.status_list[508] = "Loop Detected";
	m_response.status_list[510] = "Not Extended";
	m_response.status_list[511] = "Network Authentication Required";
}


void TcpServer::getPayload(int m_new_socket)
{
    std::string request_headers;
    std::string request_body;
    int content_length = 0;
    int bytesReceived = 0;

    // Read the request headers
    while (true) {
        int valread = recv(m_new_socket, m_buffer, sizeof(m_buffer), 0);
        if (valread == -1)
            General::exitWithError("Error in recv() 1");

        request_headers += std::string(m_buffer, valread);
        if (request_headers.find("\r\n\r\n") != std::string::npos)
            break; // Found the end of headers
    }

    // Find the Content-Length header
    std::string content_length_str = "Content-Length: ";
    std::string::size_type content_length_pos = request_headers.find(content_length_str);
    if (content_length_pos != std::string::npos)
	{
        content_length_pos += content_length_str.length();
        std::string::size_type end_of_line_pos = request_headers.find("\r\n", content_length_pos);
        if (end_of_line_pos != std::string::npos) {
            std::string content_length_value = request_headers.substr(content_length_pos, end_of_line_pos - content_length_pos);
            content_length = std::atoi(content_length_value.c_str());
        }
    }

    // Read the request body if content length is specified
    if (content_length > 0) {
        request_body.resize(content_length);
		while (bytesReceived < content_length) {
            int bytesRead = recv(m_new_socket, &request_body[bytesReceived], content_length - bytesReceived, 0);
            if (bytesRead == -1)
                General::exitWithError("Error in recv() 2");
            bytesReceived += bytesRead;
        }
    }

    // Process the received payload
    m_request.raw_request = request_headers + request_body;
    General::log("\nReceived message: \n" + m_request.raw_request);
}


string TcpServer::buildResponse()
{
	string htmlFile = "<!DOCTYPE html><html lang=\"en\"><body><h1> HOME </h1><p> Hello from your Server :) </p></body></html>";
	ostringstream ss;
	ss << "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " << htmlFile.size() << "\n\n"
		<< htmlFile;
	return ss.str();
}

bool TcpServer::sendResponse(std::string response_str, int m_new_socket)
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