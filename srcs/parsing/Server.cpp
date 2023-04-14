# include "Server.hpp"

/******************************************************************************
*                              CONSTRUCTORS                                   *
******************************************************************************/

Server::Server(void)
{

}

/******************************************************************************
*                                   COPY                                      *
******************************************************************************/

Server::Server(Server const & copy) : _locations(copy._locations)
{

}

Server	&Server::operator=(Server const & rhs)
{
	if (this != &rhs)
	{
		_locations = rhs._locations;
	}
	return (*this);
}

/******************************************************************************
*                                DESTRUCTOR                                   *
******************************************************************************/

Server::~Server(void)
{

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

void	Server::init_server_config(std::vector<std::string>::iterator it, std::vector<std::string> split)
{
	bool listening = false;
	
	while (it != split.end() && (*it).compare("}") != 0)
	{
		std::stringstream ss(*it);
		std::string directive;
		std::string parameter;
		std::string tmp;

		getline(ss, directive, ' '); //check if it's a tab which separates data
		getline(ss, parameter);

		if(directive.compare("location") == 0)
		{
			Location *location = new Location();
			//location->init_location_config(it, _split);
			_locations.push_back(location);
		}
		else if (directive.compare("listen") == 0)
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
		else if (directive.compare("error_pages") == 0)
			set_error_page(it);
		else if (directive.compare("client_max_body_size") == 0)
			set_client_max_body_size(it);
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
*                                 EXCEPTIONS                                  *
******************************************************************************/

