# include "Server.hpp"

/******************************************************************************
*                              CONSTRUCTORS                                   *
******************************************************************************/

Server::Server(void) : _domain(AF_INET), _service(SOCK_STREAM), \
					   _protocol(0), _interface(INADDR_ANY), \
					   _backlog(200), _clientMaxBodySize(0)
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
		std::cout << ANSI_RED << "Error: value missing" << ANSI_RESET << std::endl;
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
		//std::stringstream ss(*it);
		std::string 			directive;
		std::string 			parameter;
		std::string 			tmp;
		size_t					find;

		std::string	whitespace = " \t\n\r\v\f";

		//getline(ss, directive, ' '); //check if it's a tab which separates data OK
		//getline(ss, parameter);

		find = (*it).find_first_of(whitespace);
		if (find == string::npos) 
			find  = (*it).length();
		directive = (*it).substr(0, find);
		if (find != (*it).length()) 
			parameter = (*it).substr(find + 1, (*it).length());

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

/******************************************************************************
*                                   PRINTER                                   *
******************************************************************************/

void			Server::print_server(void)
{
	std::cout << ANSI_BLUE << "port: " << ANSI_RESET << _port << std::endl;
	std::cout << ANSI_BLUE << "localhost: " << ANSI_RESET << _localhost << std::endl;
	std::cout << ANSI_BLUE << "server name: " << ANSI_RESET << _serverName << std::endl;
	std::cout << ANSI_BLUE << "root: " << ANSI_RESET << _root << std::endl;
	std::cout << ANSI_BLUE << "client max body size: " << ANSI_RESET << _clientMaxBodySize << std::endl;
	std::cout << ANSI_BLUE << "domain ip: " << ANSI_RESET << _domain << std::endl;
	std::cout << ANSI_BLUE << "service: " << ANSI_RESET << _service << std::endl;
	std::cout << ANSI_BLUE << "protocol: " << ANSI_RESET << _protocol << std::endl;
	std::cout << ANSI_BLUE << "interface: " << ANSI_RESET << _interface << std::endl;
	std::cout << ANSI_BLUE << "maximum number of queued clients: " << ANSI_RESET << _backlog << std::endl;
	std::cout << ANSI_BLUE << "GET: " << ANSI_RESET << (_get ? "on" : "off" ) << std::endl;
	std::cout << ANSI_BLUE << "POST: " << ANSI_RESET  << (_post ? "on" : "off" ) << std::endl;
	std::cout << ANSI_BLUE << "DELETE: " << ANSI_RESET << (_delete ? "on" : "off" ) << std::endl << ANSI_RESET;
}

/******************************************************************************
*                                 EXCEPTIONS                                  *
******************************************************************************/

