#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "webserv.hpp"

class Client
{
	public:

        Client();
		Client(int connection, Server *server);
		Client(Client const & copy);
		~Client(void);

		Client	&operator=(Client const & rhs);

        //methods
        void getPayload();
        bool parse_request() ;
        void print_headers(const std::map<std::string, std::string>& headers);
        std::string process_request() ;
        std::string get_boundary(const std::string& request);
        std::string get_filename(const std::string& content);
        void        save_file(const std::string& path, const std::string& content);
        std::string process_request(const Request& request);

	private:

        int m_new_socket;
        Server*  _server;
		char m_buffer[4096];
		Request m_request;
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

std::string read_file(const std::string& filename);
template <typename T> std::string to_string_custom(const T& value);
std::string find_body(const std::string& request, const std::string& boundary);
void removeFirstFourLines(std::string& data);
std::string search_content_type(std::string filename);

#endif
