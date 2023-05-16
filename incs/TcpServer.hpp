#ifndef TCP_SERVER_HPP
# define TCP_SERVER_HPP

#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "Configuration.hpp"
#include "parsing/Server.hpp"
#include "Socket.hpp"

#include <vector>
#include <map> // for map
#include <fstream> // for ifstream
#include <sys/poll.h> // for poll
#include <sys/epoll.h>
#include <sys/ioctl.h> // for ioctl
#include <fcntl.h>

#define BUFFER_SIZE 100

using namespace std;

 // Structure pour stocker les informations de la requête
struct Request {
    std::string method;                          // méthode HTTP utilisée (GET, POST, etc.)
    std::string uri;                             // URI de la ressource demandée
    std::map<std::string, std::string> headers;  // en-têtes de la requête
    std::string body;                            // corps de la requête
	std::string raw_request;
};

class TcpServer
{
	public:
		TcpServer(Configuration conf);
		~TcpServer();

		void run();
		void	add_event(int epollfd, int fd, int state);
		std::vector<Socket>::iterator check_event_fd(int event_fd);
		int		acceptConnection(struct epoll_event ev, int epollfd);

        void	getHeader(int new_socket);
        void	getPayload(int new_socket);
        string	buildResponse();
        bool	sendResponse(std::string response_str, int m_new_socket);

		std::vector<Server*>		_servers;
		std::vector<Socket>			_socketList;

	private:
		

		//int					m_socket;
		//int					m_new_socket;
		long				m_incomingMessage;
		string				m_serverMessage;

		char				m_buffer[4096];
		Request				m_request;

		
};

bool parse_request(Request &m_request, char *m_buffer);
std::string process_request(const Request& request);

#endif