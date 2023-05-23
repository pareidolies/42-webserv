#ifndef SOCKET_HPP
# define SOCKET_HPP

#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <fstream> // for ifstream

class Server;

class Socket
{
	public:
		Socket(string ip_address, int port, Server	*server);
		~Socket();
		void	startListen();
		int		getSocketFd(void);
		Server *getServer();
		void	*ft_memset(void *s, int c, size_t n);

	private:
		string				m_ip_address;
		int					m_port;
		int					m_socket;
		//int				m_new_socket;
		//long				m_incomingMessage;
		struct sockaddr_in	m_socketAddress;
		unsigned int		m_socketAddress_len;
		//string			m_serverMessage;
		Server				*_server;

        int		startServer();
        //void	closeServer();
};

#endif