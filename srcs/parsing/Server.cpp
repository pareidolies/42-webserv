# include "Server.hpp"

/******************************************************************************
*                              CONSTRUCTORS                                   *
******************************************************************************/

Server::Server(void) : _domain(AF_INET), _service(SOCK_STREAM), \
					   _protocol(0), _interface(INADDR_ANY), \
					   _backlog(200), _socket(-1), _clientMaxBodySize(0)
{
	//initialize values
}

/******************************************************************************
*                                   COPY                                      *
******************************************************************************/

Server::Server(Server const & copy) : _locations(copy._locations)
{
	//to complete
}

Server	&Server::operator=(Server const & rhs)
{
	if (this != &rhs)
	{
		_locations = rhs._locations;
		//to complete
	}
	return (*this);
}

/******************************************************************************
*                                DESTRUCTOR                                   *
******************************************************************************/

Server::~Server(void)
{
	for(std::vector<Location*>::iterator it = _locations.begin(); it != _locations.end(); it++)
		delete	*it;
}

/******************************************************************************
*                                  GETTERS                                    *
******************************************************************************/



/******************************************************************************
*                                  SETTERS                                    *
******************************************************************************/




/******************************************************************************
*                             MEMBER FUNCTIONS                                *
******************************************************************************/

std::string	Server::check_semicolon(std::string str)
{
	std::string	delimiter = " \t\n\r\v\f;";
	std::string	whitespace = " \t\n\r\v\f";
	size_t		semicolon;

	str = trim(str, whitespace);
	semicolon = str.find(";");
  	if (str.begin() + semicolon != str.end() - 1)
	{
		std::cout << ANSI_RED << "Error: missing semicolon" << ANSI_RESET << std::endl;
		return "";
	}
	str = trim(str, delimiter);
	if (str.empty())
	{
		std::cout << ANSI_RED << "Error: port missing" << ANSI_RESET << std::endl;
		return "";
	}
	return (str);
}

bool	Server::check_client_max_body_size(std::string parameter)
{
	const char *str = parameter.c_str();
	for(size_t i = 0; i < strlen(str); i++)
	{
		if(!isdigit(str[i]))
			return false;
	}
	long nbr = atol(str);
	if (nbr > 2147483647)
		return false;
	return true;
}

void	Server::init_server_config(std::vector<std::string>::iterator it, std::vector<std::string> split)
{
	bool listening = false;
	
	while (it != split.end() && (*it).compare("}") != 0)
	{
		std::stringstream ss(*it);
		std::string directive;
		std::string parameter;
		std::string tmp;
		size_t		find;

		getline(ss, directive, ' '); //check if it's a tab which separates data
		getline(ss, parameter);

		if(directive.compare("location") == 0)
		{
			Location *location = new Location();
			//location->init_location_config(it, split);
			_locations.push_back(location);
		}
		else if (directive.compare("listen") == 0) //if multiple listen ?
		{
			parameter = check_semicolon(parameter);
			this->_port = atoi(((parameter).c_str()));
			listening = true;
		}
		else if (directive.compare("server_name") == 0)
		{
			parameter = check_semicolon(parameter);
			this->_serverName = parameter;	
		}
		else if (directive.compare("root") == 0)
		{
			parameter = check_semicolon(parameter);
			this->_root = parameter;
		}
		else if (directive.compare("index") == 0)
		{
			parameter = check_semicolon(parameter);
			this->_serverName = parameter;
		}
		else if (directive.compare("http_methods") == 0)
		{
			parameter = check_semicolon(parameter);
			find = parameter.find("GET"); //+ check if separated by space
  			if (find!=std::string::npos)
				this->_get = true;
			find = parameter.find("POST");
  			if (find!=std::string::npos)
				this->_post = true;
			find = parameter.find("DELETE");
  			if (find!=std::string::npos)
				this->_delete = true;
		}
		/*else if (directive.compare("cgi") == 0)
		{
			
		}
		else if (directive.compare("error_pages") == 0)
			add_error_page(it);
		*/
		else if (directive.compare("client_max_body_size") == 0)
		{
			parameter = check_semicolon(parameter);
			if (check_client_max_body_size(parameter))
				this->_clientMaxBodySize = atoi(parameter.c_str());
		}
		else
		{
			std::cout << ANSI_RED << "Error: unknown option detected" << ANSI_RESET << std::endl;
		}
		it++;
	}
	if (listening == false || _port < 0)
		std::cout << ANSI_RED << "Error: listening port not set or wrong port value" << ANSI_RESET << std::endl;
}

void			Server::print_server(void)
{
	std::cout << ANSI_BLUE << "port: " << _port << std::endl;
	std::cout << "server name: " << _serverName << std::endl;
	std::cout << "root: " << _root << std::endl;
	std::cout << "client max body size: " << _clientMaxBodySize << std::endl;
	std::cout << "domain ip: " << _domain << std::endl;
	std::cout << "service: " << _service << std::endl;
	std::cout << "protocol: " << _protocol << std::endl;
	std::cout << "interface: " << _interface << std::endl;
	std::cout << "maximum number of queued clients: " << _backlog << std::endl;
	std::cout << "GET: " << _get << std::endl;
	std::cout << "POST: " << _post << std::endl;
	std::cout << "DELETE: " << _delete << std::endl << ANSI_RESET;
}

/******************************************************************************
*                                 EXCEPTIONS                                  *
******************************************************************************/

