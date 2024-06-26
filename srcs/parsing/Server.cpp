# include "webserv.hpp"

/******************************************************************************
*                              CONSTRUCTORS                                   *
******************************************************************************/

Server::Server(void) : _domain(AF_INET), _service(SOCK_STREAM), \
					   _protocol(0), _interface(INADDR_ANY), \
					   _backlog(1024), _clientMaxBodySize(0), \
					   _autoindex(false)
{
	_port = 80;
	_host = "0.0.0.0";
	_root = "";
	_index = "";
	_autoindex = false;
	_upload = "";
	_get = false;
	_post = false;
	_delete = false;
	_return = "";
}

/******************************************************************************
*                                   COPY                                      *
******************************************************************************/

Server::Server(Server const & copy) : _locations(copy._locations)
{
	_domain = copy._domain; //AF_INET, AF_INET6, AF_UNSPEC
	_service = copy._service; //SOCK_STREAM, SOCK_DGRAM
	_protocol = copy._protocol; //use 0 for "any"
    _interface = copy._interface; //needs to be set to INADDR_ANY
	_backlog = copy._backlog; //maximum number of queued clients
	_locations = copy._locations;
	_port = copy._port;
	_host = copy._host;
	_serverName = copy._serverName;
	_clientMaxBodySize = copy._clientMaxBodySize;
	_root = copy._root;
	_index = copy._index;
	_autoindex = copy._autoindex;
	_cgiFileExtension = copy._cgiFileExtension;
	_cgiPathToScript = copy._cgiPathToScript;
	_cgi = copy._cgi;
	_upload = copy._upload;
	_get = copy._get;
	_post = copy._post;
	_delete = copy._delete;
	_errorPages = copy._errorPages;
}

Server	&Server::operator=(Server const & rhs)
{
	if (this != &rhs)
	{
		_domain = rhs._domain; //AF_INET, AF_INET6, AF_UNSPEC
	    _service = rhs._service; //SOCK_STREAM, SOCK_DGRAM
	    _protocol = rhs._protocol; //use 0 for "any"
        _interface = rhs._interface; //needs to be set to INADDR_ANY
	    _backlog = rhs._backlog; //maximum number of queued clients
	    _locations = rhs._locations;
	    _port = rhs._port;
	    _host = rhs._host;
	    _serverName = rhs._serverName;
	    _clientMaxBodySize = rhs._clientMaxBodySize;
	    _root = rhs._root;
	    _index = rhs._index;
	    _autoindex = rhs._autoindex;
	    _cgiFileExtension = rhs._cgiFileExtension;
	    _cgiPathToScript = rhs._cgiPathToScript;
		_cgi = rhs._cgi;
	    _upload = rhs._upload;
	    _get = rhs._get;
	    _post = rhs._post;
	    _delete = rhs._delete;
	    _errorPages = rhs._errorPages;
	}
	return (*this);
}

/******************************************************************************
*                                DESTRUCTOR                                   *
******************************************************************************/

Server::~Server(void)
{
	//if (g_shutdown == 1)
	//{
		for(std::vector<Location*>::iterator it = _locations.begin(); it != _locations.end(); it++)
		{
			// (*it)->print_location();
			if (*it)
				delete	*it;
		}
	//}
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
	return true;
}

void	Server::init_server_config(std::vector<std::string>::iterator it, std::vector<std::string> split)
{
	bool listening = false;
	it++;
	while (it != split.end() && (*it).compare("}") != 0)
	{
		std::string 			directive;
		std::string 			parameter;
		std::string 			tmp;
		size_t					find;
		std::string	whitespace = " \t\n\r\v\f";
		std::string	type = "location";
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
		else if (directive.compare("http_methods") == 0)
		{
			parameter = check_semicolon(parameter);
			while (!parameter.empty())
			{
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
				_cgi.insert ( std::pair<std::string,std::string>(_cgiFileExtension,_cgiPathToScript) );
			}
			else
				std::cout << ANSI_RED << "Error: cgi information missing" << ANSI_RESET << std::endl;
		}
		else if (directive.compare("error_page") == 0)
		{
			std::string	num_str;
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
				this->_clientMaxBodySize = atoll(parameter.c_str());
		}
		else
		{
			std::cout << ANSI_RED << "Error: [" << (*it) << "]" << ANSI_RESET;
			throw Server::WrongConfLine();
		}
		it++;
	}
	if (listening == false || _port <= 0 || _host.empty())
		throw Server::NotListening();
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
	std::cout << ANSI_BLUE << "index: " << ANSI_RESET << _index << std::endl;
	std::cout << ANSI_BLUE << "cgi: " << ANSI_RESET << std::endl;
	for(std::map<std::string, std::string>::iterator it = _cgi.begin(); it != _cgi.end(); it++)
		std::cout << "[" << it->first << "] " << it->second << std::endl;
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
		// std::cout << ANSI_YELLOW << "LOCATION:" << ANSI_RESET << std::endl;
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
*                                     UTILS                                   *
******************************************************************************/

bool	Server::is_method_allowed(std::string method)
{
	if (method.compare("GET") == 0 && _get)
		return true;
	else if (method.compare("POST") == 0 && _post)
		return true;
	else if (method.compare("DELETE") == 0 && _delete)
		return true;
	return false;
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

long long unsigned int						Server::getClientMaxBodySize()
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

std::map<std::string, std::string> Server::getCgi()
{
	return (_cgi);
}

std::string							Server::getReturn()
{
	return(_return);
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

std::string			Server::getErrorPage(int code)
{
	return (_errorPages[code]);
}

void	Server::setAddress(std::string a) { this->_host = a; }