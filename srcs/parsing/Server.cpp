# include "webserv.hpp"

/******************************************************************************
*                              CONSTRUCTORS                                   *
******************************************************************************/

Server::Server(void) : _domain(AF_INET), _service(SOCK_STREAM), \
					   _protocol(0), _interface(INADDR_ANY), \
					   _backlog(200), _clientMaxBodySize(0), \
					   _autoindex(false)
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
*                                  SETTERS                                    *
******************************************************************************/




/******************************************************************************
*                             MEMBER FUNCTIONS                                *
******************************************************************************/

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
	it++;
	while (it != split.end() && (*it).compare("}") != 0)
	{
		//std::stringstream ss(*it);
		std::string 			directive;
		std::string 			parameter;
		std::string 			tmp;
		size_t					find;
		std::string	whitespace = " \t\n\r\v\f";
		std::string	type = "location";
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
			if(check_second_bracket(++it, split, type))
			{
				Location *location = new Location(trim(parameter, whitespace));
				location->init_location_config(it, split);
				_locations.push_back(location);
				while ((*it) != "}")
					it++;
			}
		}
		else if (directive.compare("listen") == 0) //if multiple listen?
		{
			parameter = check_semicolon(parameter);
			find = parameter.find_first_of(":");
			if (find != string::npos)
			{
				this->_host = parameter.substr(0, find);
				parameter.erase(0, find + 1);
			}
			this->_port = atoi(((parameter).c_str()));
			listening = true;
		}
		else if (directive.compare("server_name") == 0) //if multiple server names?
		{
			parameter = check_semicolon(parameter);
			while (!parameter.empty())
			{
				find = parameter.find_first_of(whitespace);
				if (find != string::npos)
				{
					_serverName.push_back(parameter.substr(0, find));
					parameter = parameter.erase(0, find + 1);
				}
				else
				{
					_serverName.push_back(parameter);
					break;
				}
			}
		}
		else if (directive.compare("root") == 0)
		{
			parameter = check_semicolon(parameter);
			if (!dir_exists(parameter))
			{
				std::cout << ANSI_RED << "Error: [" << parameter << "]" << ANSI_RESET;
				throw Server::DirOrFileError();
			}
			this->_root = parameter;
		}
		else if (directive.compare("index") == 0)
		{
			parameter = check_semicolon(parameter);
			this->_index = parameter;
		}
		else if (directive.compare("upload") == 0)
		{
			parameter = check_semicolon(parameter);
			if (!dir_exists(parameter))
			{
				std::cout << ANSI_RED << "Error: [" << parameter << "]" << ANSI_RESET;
				throw Server::DirOrFileError();
			}
			this->_upload = parameter;
		}
		else if (directive.compare("http_methods") == 0 || directive.compare("allow_methods") == 0)
		{
			parameter = check_semicolon(parameter);
			while (!parameter.empty())
			{
				std::cout << parameter << std::endl;
				find = parameter.find_first_of(whitespace);
				if (find == string::npos)
					find = parameter.end() - parameter.begin();

				if (parameter.substr(0, find).compare("GET") == 0)
					this->_get = true;
				else if (parameter.substr(0, find).compare("POST") == 0)
					this->_post = true;
				else if (parameter.substr(0, find).compare("DELETE") == 0)
					this->_delete = true;
				else
				{
					std::cout << ANSI_RED << "Error: [" << (*it) << "]" << ANSI_RESET;
					throw Server::WrongConfLine();
				}
				parameter = parameter.erase(0, find + 1);
			}
		}
		else if (directive.compare("cgi") == 0) //if multiple
		{
			parameter = check_semicolon(parameter);
			find = parameter.find_first_of(whitespace);
			if (find != string::npos)
			{
				this->_cgiFileExtension = parameter.substr(0, find);
				parameter.erase(0, find + 1);
				this->_cgiPathToScript = trim(parameter, whitespace);
				if (!is_extension(this->_cgiFileExtension))
				{
					std::cout << ANSI_RED << "Error: [" << parameter << "]" << ANSI_RESET;
					throw Server::DirOrFileError();
				}
			}
			else
				std::cout << ANSI_RED << "Error: cgi information missing" << ANSI_RESET << std::endl;
		}
		else if (directive.compare("error_page") == 0)
		{
			std::string	num_str;
			//int			num;
			std::string	path;
			parameter = check_semicolon(parameter);
			find = parameter.find_first_of(whitespace);
			if (find != string::npos)
			{
				num_str = parameter.substr(0, find);
				parameter.erase(0, find + 1);
				path = trim(parameter, whitespace);
				if (!file_exists(path))
				{
					std::cout << ANSI_RED << "Error: [" << parameter << "]" << ANSI_RESET;
					throw Server::DirOrFileError();
				}
			}
			this->_errorPages.insert(std::make_pair(atoi(num_str.c_str()), path));
		}
		else if (directive.compare("autoindex") == 0)
		{
			parameter = check_semicolon(parameter);
			if (parameter.compare("on") == 0)
				this->_autoindex = true;
			else if (parameter.compare("off") == 0)
				this->_autoindex = false;
		}
		else if (directive.compare("client_max_body_size") == 0)
		{
			parameter = check_semicolon(parameter);
			if (check_client_max_body_size(parameter))
				this->_clientMaxBodySize = atoi(parameter.c_str());
		}
		else
		{
			std::cout << ANSI_RED << "Error: [" << (*it) << "]" << ANSI_RESET;
			throw Server::WrongConfLine();
		}
		it++;
	}
	if (listening == false || _port < 0)
		throw Server::NotListening();
	if (_errorPages.empty()) //setting default page
	{
		std::string path = "www/site/errorPages/404.html";
		this->_errorPages.insert(std::make_pair(404, path));
		path = "www/site/errorPages/403.html";
		this->_errorPages.insert(std::make_pair(403, path));
	}
	if (_upload.empty()) //setting default upload
		this->_upload = "www/site";
	if (_root.empty()) //setting default root
		this->_root = "www/site";
}

/******************************************************************************
*                                   PRINTER                                   *
******************************************************************************/

void			Server::print_server(void)
{
	std::cout << ANSI_BLUE << "port: " << ANSI_RESET << _port << std::endl;
	std::cout << ANSI_BLUE << "host: " << ANSI_RESET << _host << std::endl;
	std::cout << ANSI_BLUE << "server names: " << ANSI_RESET << std::endl;
	for(std::vector<std::string>::iterator it = this->_serverName.begin(); it != this->_serverName.end(); it++)
	{	
		std::cout << (*it) << ANSI_RESET << std::endl;
	}
	std::cout << ANSI_BLUE << "root: " << ANSI_RESET << _root << std::endl;
	std::cout << ANSI_BLUE << "client max body size: " << ANSI_RESET << _clientMaxBodySize << std::endl;
	std::cout << ANSI_BLUE << "domain ip: " << ANSI_RESET << _domain << std::endl;
	std::cout << ANSI_BLUE << "service: " << ANSI_RESET << _service << std::endl;
	std::cout << ANSI_BLUE << "protocol: " << ANSI_RESET << _protocol << std::endl;
	std::cout << ANSI_BLUE << "interface: " << ANSI_RESET << _interface << std::endl;
	std::cout << ANSI_BLUE << "upload: " << ANSI_RESET << _upload << std::endl;
	std::cout << ANSI_BLUE << "cgi file extension: " << ANSI_RESET << _cgiFileExtension << std::endl;
	std::cout << ANSI_BLUE << "cgi path to script: " << ANSI_RESET << _cgiPathToScript << std::endl;
	std::cout << ANSI_BLUE << "maximum number of queued clients: " << ANSI_RESET << _backlog << std::endl;
	std::cout << ANSI_BLUE << "GET: " << ANSI_RESET << (_get ? "on" : "off" ) << std::endl;
	std::cout << ANSI_BLUE << "POST: " << ANSI_RESET  << (_post ? "on" : "off" ) << std::endl;
	std::cout << ANSI_BLUE << "DELETE: " << ANSI_RESET << (_delete ? "on" : "off" ) << std::endl;
	std::cout << ANSI_BLUE << "autoindex: " << ANSI_RESET << (_autoindex ? "on" : "off" ) << std::endl;
	std::cout << ANSI_BLUE << "error pages: " << ANSI_RESET << std::endl;
	for(std::map<int, std::string>::iterator it = _errorPages.begin(); it != _errorPages.end(); it++)
		std::cout << "[" << it->first << "] " << it->second << std::endl;
	for(std::vector<Location*>::iterator it = this->_locations.begin(); it != this->_locations.end(); it++)
	{	
		std::cout << ANSI_YELLOW << "LOCATION:" << ANSI_RESET << std::endl;
		(*it)->print_location();
		std::cout << std::endl;
	}
}

/******************************************************************************
*                                 EXCEPTIONS                                  *
******************************************************************************/

const char *	Server::WrongConfLine::what(void) const throw()
{
	return (" option found in configuration file is unknown");
}

const char *	Server::NotListening::what(void) const throw()
{
	return ("Error: listening port not set or wrong port value");
}

const char *	Server::DirOrFileError::what(void) const throw()
{
	return (" directory or file or extension does not exist");
}

/******************************************************************************
*                                  GETTERS                                    *
******************************************************************************/

int				Server::getPort()
{
	return(_port);
}

std::string		Server::getHost()
{
	return(_host);
}

int			Server::getDomain()
{
	return(_domain);
}

int			Server::getService()
{
	return(_service);
}

int			Server::getProtocol()
{
	return(_protocol);
}
u_long		Server::getInterface()
{
	return(_interface);
}

int			Server::getBacklog()
{
	return(_backlog);
}

std::vector<Location*>				Server::getLocations()
{
	return(_locations);
}

std::vector<std::string>			Server::getServerName()
{
	return(_serverName);
}

int									Server::getClientMaxBodySize()
{
	return(_clientMaxBodySize);
}

std::string							Server::getRoot()
{
	return(_root);
}

std::string							Server::getIndex()
{
	return(_index);
}

bool								Server::getAutoindex()
{
	return(_autoindex);
}

std::string							Server::getCgiFileExtension()
{
	return(_cgiFileExtension);
}

std::string							Server::getCgiPathToScript()
{
	return(_cgiPathToScript);
}

std::string							Server::getUpload()
{
	return(_upload);
}

bool								Server::getGet()
{
	return(_get);
}

bool								Server::getPost()
{
	return(_post);
}

bool								Server::getDelete()
{
	return(_delete);
}

std::map<int, std::string>			Server::getErrorPages()
{
	return(_errorPages);
}