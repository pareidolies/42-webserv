# include "webserv.hpp"
#include "parsing/Server.hpp"
#include <map>


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
	_backlog = serv->getBacklog();
	_locations = serv->getLocations();
	_port = serv->getPort();
	_host = serv->getHost();
	_serverName = serv->getServerName();
	_clientMaxBodySize = serv->getClientMaxBodySize();
	_root = serv->getRoot();
	_index = serv->getIndex();
	_autoindex = serv->getAutoindex();
	_cgi = serv->getCgi();
	_upload = serv->getUpload();
	_get = serv->getGet();
	_post = serv->getPost();
	_delete = serv->getDelete();
	_errorPages = serv->getErrorPages();
	m_serverMessage = buildResponse();
	print_server();
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
		_socketList.push_back(Socket((*it)->getHost(), (*it)->getPort(), (*it)));
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
	std::map<int, Client> clients; //managing clients
	bool done = false; //check end of response & request
	int		server_id;

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
						
			if (it != _socketList.end())
			{
				int connection = acceptConnection(events[n], epollfd);
				//std::cout << "coucou" << std::endl;
				Client new_client(connection, (*it).getServer());
				clients[connection] = new_client;
				// std::cout << "coucou1" << std::endl;
			}

			// Receiving request
			else if (events[n].events & EPOLLIN) 
			{
				// std::cout << "COUCOU" << std::endl;
				clients[events[n].data.fd].getPayload();
				//std:cout << ANSI_RED << "hello" << ANSI_RESET << std::endl;
				// cl:ients[events[n].data.fd].getServer()->print_server();
				done = clients[events[n].data.fd].parse_request();
				//std::cout << ANSI_RED << "hello1" << ANSI_RESET << std::endl;
				//std::cout << done << std::endl;
				// std::cout << "coucou2" << std::endl;
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
				//std::string response_str = clients[events[n].data.fd].process_request();
				//done = sendResponse(response_str, events[n].data.fd);
				Response response(clients[events[n].data.fd]);
				//std::cout << "coucou2" << std::endl;
				done = response.send_response();
				//std::cout << "coucou3" << std::endl;
				if (done)
				{
					// clients[events[n].data.fd].buffer_memset();
					if (epoll_ctl(epollfd, EPOLL_CTL_DEL, events[n].data.fd, &ev) == -1)
						General::exitWithError("epoll del");
					if (close(events[n].data.fd) < 0)
						General::exitWithError("close");
					clients.erase(events[n].data.fd);
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