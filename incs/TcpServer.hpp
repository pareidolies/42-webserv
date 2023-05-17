#ifndef TCPSERVER_HPP
# define TCPSERVER_HPP

#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "parsing/Server.hpp"

#include <map> // for map
#include <fstream> // for ifstream
#include <sys/poll.h> // for poll
#include <sys/ioctl.h> // for ioctl

#define BUFFER_SIZE 100

using namespace std;

class Server;

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
		TcpServer(string ip_address, int port);
		TcpServer(Server *serv);
		~TcpServer();
		void startListen();
		void init_var(Server *serv);
		void print_server();

	private:
		string				m_ip_address;
		int					m_port;
		int					m_socket;
		int					m_new_socket;
		long				m_incomingMessage;
		struct sockaddr_in	m_socketAddress;
		unsigned int		m_socketAddress_len;
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
        void	acceptConnection(int &new_socket);
        void	getHeader(int &new_socket);
        void	getPayload(int &new_socket);
        string	buildResponse();
        void	sendResponse(std::string response_str);
};

Request& parse_request(Request &m_request, char *m_buffer);
std::string process_request(const Request& request);

#endif