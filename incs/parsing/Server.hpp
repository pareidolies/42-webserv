#ifndef SERVER_HPP
# define SERVER_HPP

# include "Location.hpp"
# include "webserv.hpp"
# include "utils.hpp"

# include <vector>
# include <netinet/in.h>
# include <sys/types.h>

class Server
{
	public:

		Server(void);
		Server(Server const & copy);
		~Server(void);

		Server	&operator=(Server const & rhs);

		void			init_server_config(std::vector<std::string>::iterator it, std::vector<std::string> vector);
		std::string		check_semicolon(std::string str);
		bool			check_client_max_body_size(std::string parameter);
		void			print_server(void);

	private:

		std::vector<Location*>				_locations;
		int									_domain; //AF_INET, AF_INET6, AF_UNSPEC
		int									_service; //SOCK_STREAM, SOCK_DGRAM
		int									_protocol; //use 0 for "any"
		u_long								_interface; //needs to be set to INADDR_ANY
		int									_port;
		int									_backlog; //maximum number of queued clients
		struct sockaddr_in					_address;
		int									_socket;
		std::string							_serverName;
		int									_clientMaxBodySize;
		std::string							_root;
		bool								_get;
		bool								_post;
		bool								_delete;
		//error_pages
};

#endif
