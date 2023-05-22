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
#include "parsing/Server.hpp"

#include "parsing/Configuration.hpp"
#include "parsing/Server.hpp"
#include "Socket.hpp"
#include "Client.hpp"

#include <vector>
#include <map> // for map
#include <fstream> // for ifstream
#include <sys/poll.h> // for poll
#include <sys/epoll.h>
#include <sys/ioctl.h> // for ioctl
#include <fcntl.h>

#define BUFFER_SIZE 2056

using namespace std;

class Configuration;

class Server;

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
        bool	sendResponse(std::string response_str, int m_new_socket);

		std::vector<Server*>		_servers;
		std::vector<Socket>			_socketList;

	private:
		

		//int					m_socket;
		//int					m_new_socket;
		long				m_incomingMessage;
		string				m_serverMessage;

		int									_domain; //AF_INET, AF_INET6, AF_UNSPEC
		int									_service; //SOCK_STREAM, SOCK_DGRAM
		int									_protocol; //use 0 for "any"
		u_long								_interface; //needs to be set to INADDR_ANY
		int									_backlog; //maximum number of queued clients
		//** parsing **
		//-> only in server
		std::vector<Location*>				_locations;
		int									_port;
		std::string							_host;
		//-> both in server and location
		std::vector<std::string>			_serverName;
		int									_clientMaxBodySize;
		std::string							_root;
		std::string							_index;
		bool								_autoindex;
		std::map<std::string, std::string>	_cgi;
		std::string							_cgiFileExtension;
		std::string							_cgiPathToScript;
		std::string							_upload;
		bool								_get;
		bool								_post;
		bool								_delete;
		std::map<int, std::string>			_errorPages;

		char m_buffer[4096];
		Request m_request;

        int		startServer();
        void	closeServer();
        void	getPayload(int new_socket);
        string	buildResponse();
		void    init_var(Server *serv);

		void	print_server(void);
};

bool parse_request(Request &m_request, char *m_buffer);
std::string handle_request(const Request& request);

#endif