#ifndef SERVER_HPP
# define SERVER_HPP

# include "Location.hpp"
# include "webserv.hpp"
# include "utils.hpp"

# include <cstring>
# include <vector>
# include <map>
# include <exception>
# include <netinet/in.h>
# include <sys/types.h>

class Server
{
	public:

		class WrongConfLine : public std::exception
		{
			public:
				virtual const char * what(void) const throw(); 
		};
		class NotListening : public std::exception
		{
			public:
				virtual const char * what(void) const throw(); 
		};
		class DirOrFileError : public std::exception
		{
			public:
				virtual const char * what(void) const throw(); 
		};

		Server(void);
		Server(Server const & copy);
		~Server(void);

		Server	&operator=(Server const & rhs);

		void			init_server_config(std::vector<std::string>::iterator it, std::vector<std::string> vector);
		bool			check_client_max_body_size(std::string parameter);
		void			print_server(void);

		int getDomain() const { return (_domain); }
		int getService() const { return (_service); }
		int getProtocol() const { return (_protocol); }
		u_long getInterface() const { return (_interface); }
		int getBacklogs() const { return (_backlog); }
		std::vector<Location*> getLocations() const { return (_locations); }
		int getPort() const { return (_port); }
		std::string getHost() const { return (_host); }
		std::vector<std::string> getServerName() const { return (_serverName); }
		int getClientMaxBodySize() const { return (_clientMaxBodySize); }
		std::string getRoot() const { return (_root); }
		std::string getIndex() const { return (_index); }
		bool getAutoIndex() const { return (_autoindex); }
		std::string getCgiFileExtension() const { return (_cgiFileExtension); }
		std::string getCgiPathToScript() const { return (_cgiPathToScript); }
		std::string getUpload() const { return (_upload); }
		bool getGet() const { return (_get); }
		bool getPost() const { return (_post); }
		bool getDelete() const { return (_delete); }
		std::map<int, std::string> getErrorPages() const { return (_errorPages); }

	private:

		//** set at initialization **
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
};

#endif
