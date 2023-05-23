#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "webserv.hpp"
#include <deque>

class Client
{
	public:

        Client();
		Client(int connection, Server *server, std::vector<Server*>	serversList);
		Client(Client const & copy);
		~Client(void);

		Client	&operator=(Client const & rhs);

        //methods
        bool getPayload();
		bool parse_request();
		std::deque<std::string> getlines(std::string buf);
		void parse_line(std::deque<std::string> &lines, std::string &raw_request);
		void handle_request_line(std::string &line);
		void handle_field_line(std::string &line);
		void handle_body(std::string &raw_request);
		void remove_carriage_return_char(std::deque<std::string> &lines);
		std::string get_query_string(std::string &request_target);
		void check_method(std::string &method);
		std::string add_root(std::string request_target);
		void check_access(std::string request_target) ;
		bool field_name_has_whitespace(std::string &field_name) const;
		bool upload_file(std::string &raw_request);
		bool is_whitespace(unsigned char c);
		Server *getServer();
		std::string getPath();
		std::string getRequestTarget();
		int getStatusCode();
		std::string getMethod();
		int getFd();
		std::vector<std::string> ft_split(const char *str, const char *charset);
		std::map<std::string, std::string> getHeaders() const ;
		std::string		getBody() const;
		bool check_if_corresponding_location(std::string &request_target);
		void set_location_data();
		int getClientMaxBodySize();
		std::string getRoot();
		std::string getIndex();
		bool getAutoindex();
		std::map<std::string, std::string> getCgi();
		std::string getUpload();
		bool getGet();
		bool getPost();
		bool getDelete();
		std::map<int, std::string>	 getErrorPages();
		bool is_method_allowed(std::string method);
		std::string getReturn();
		void    general_log(int status);
		void    error_log(int status);
		void print_headers(const std::map<std::string, std::string>& headers);
		std::string		getContentType();
		void		select_server_block();
		void		set_server_data();
		bool 		getCloseConnection();


//    struct Request {
//         std::string method;                          // méthode HTTP utilisée (GET, POST, etc.)
//         std::string uri;                             // URI de la ressource demandée
//         map<std::string, std::string> headers;  	    // en-têtes de la requête
//         std::string body;                            // corps de la requête
// 		   std::string raw_request;
//     };

// 	struct Response {
// 		std::string full_response;
// 		std::string body;
// 		std::string cgi_path;
// 		std::string body_size;
// 		std::string content_type;
//         std::map<int, std::string> status_code_list;
//         int	status_code;
// 	};


	private:

        int m_new_socket;
        std::vector<Server*>		_serversList;
		Server*   _server;
		char m_buffer[4096];
		Request m_request;
		//Response m_response;
        int									_domain; //AF_INET, AF_INET6, AF_UNSPEC
		int									_service; //SOCK_STREAM, SOCK_DGRAM
		int									_protocol; //use 0 for "any"
		u_long								_interface; //needs to be set to INADDR_ANY
		int									_backlog; //maximum number of queued clients
		//** parsing **
		//-> only in server
		std::vector<Location*>				_locations;
		Location*							_corresponding_location;

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
		std::map<std::string, std::string>	_cgi;
		std::string							_upload;
		bool								_get;
		bool								_post;
		bool								_delete;
		std::map<int, std::string>			_errorPages;

		bool								_request_is_complete;
		int									_status_code;
		std::string							_method;
		std::stringstream					_ss;
		bool								_handle_headers;
		std::string							_request_target;
		std::string							_query_string;
		std::string							_body_boundary;
		std::string							_path;

		std::string							_return;

		bool 								_close_connection;
};

int		ft_check_charset(char c, const char *charset);
int		ft_count_words(const char *str, const char *charset);
void str_to_lower(std::string &str);
void remove(std::string &str, char c);

#endif
