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

#include <map> // for map
#include <fstream> // for ifstream
#include <sys/poll.h> // for poll
#include <sys/ioctl.h> // for ioctl

#define BUFFER_SIZE 100
#define NB_EVENTS 255

using namespace std;

 // Structure pour stocker les informations de la requête
    struct Request {
        std::string method;                          // méthode HTTP utilisée (GET, POST, etc.)
        std::string uri;                             // URI de la ressource demandée
        std::map<std::string, std::string> headers;  // en-têtes de la requête
        std::string body;                            // corps de la requête
    };

class TcpServer
{
	public:
		TcpServer(Configuration conf);
		~TcpServer();
		void startListen();

	private:

		std::vector<Server*>		_servers;
		std::vector<size_t>			_servers_fd;

		string				m_ip_address;
		int					m_port;
		int					m_socket;
		int					m_new_socket;
		long				m_incomingMessage;
		struct sockaddr_in	m_socketAddress;
		unsigned int		m_socketAddress_len;
		string				m_serverMessage;

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