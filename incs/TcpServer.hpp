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

#define BUFFER_SIZE 100

using namespace std;

class TcpServer
{
	public:
		TcpServer(string ip_address, int port);
		~TcpServer();
		void startListen();

	private:
		string				m_ip_address;
		int					m_port;
		int					m_socket;
		int					m_new_socket;
		long				m_incomingMessage;
		struct sockaddr_in	m_socketAddress;
		unsigned int		m_socketAddress_len;
		string				m_serverMessage;

        int		startServer();
        void	closeServer();
        void	acceptConnection(int &new_socket);
        void	getHeader(int &new_socket);
        void	getPayload(int &new_socket);
        string	buildResponse();
        void	sendResponse();
};

#endif