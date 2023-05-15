# include "webserv.hpp"

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

TcpServer::~TcpServer()
{
	cout << "Terminating the server." << endl;
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
	//bool done; check end of response & request

	//create the epoll instance
	epollfd = epoll_create(1);
	if (epollfd == -1)
		General::exitWithError("epoll_create");
	for(int i = 0;i < (int)_socketList.size();i++)
		add_event(epollfd, _socketList[i].getSocketFd(), EPOLLIN);

	while (42) 
	{
		event_fds = epoll_wait(epollfd, events, MAX_EVENTS, -1);

		if (event_fds == -1 && errno != EINTR)
			General::exitWithError("epoll_wait");

		// Loop that handle events happening on server fd and connections fds
		General::log("\n====== Waiting for a new connection ======");
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
				acceptConnection(events[n], epollfd);
			}

			// Receiving request
			else if (events[n].events & EPOLLIN) 
			{
				getPayload(events[n].data.fd);
				parse_request(m_request, m_buffer);
				ev.events = EPOLLOUT;
				ev.data.fd = events[n].data.fd;
				if (epoll_ctl(epollfd, EPOLL_CTL_MOD, events[n].data.fd, &ev) == -1)
					General::exitWithError("epoll_create");
			}
			// Sending response
			else if (events[n].events & EPOLLOUT) 
			{
				std::string response_str = process_request(m_request);
				sendResponse(response_str, events[n].data.fd);
				memset(m_buffer, 0, sizeof(m_buffer));
				epoll_ctl(epollfd, EPOLL_CTL_DEL, events[n].data.fd, &ev);
				close(events[n].data.fd);
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

void TcpServer::sendResponse(std::string response_str, int m_new_socket)
{
    if (send(m_new_socket, response_str.c_str(), response_str.size(), 0) < 0)
	    General::log("Error sending response to client");
    else
    {
	    General::log("------ Server Response sent to client ------\n");
	    // General::log("Responsed message: \n" + response_str);
    }
}