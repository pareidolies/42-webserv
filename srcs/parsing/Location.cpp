# include "webserv.hpp"

/******************************************************************************
*                              CONSTRUCTORS                                   *
******************************************************************************/

Location::Location(void)
{
	_get = false;
	_post = false;
	_delete = false;
}

Location::Location(std::string locate) : _locate(locate)
{
	_get = false;
	_post = false;
	_delete = false;
	_clientMaxBodySize = 0;
	_root = "";
	_index = "";
	_autoindex = false;
	_upload = "";
	_return = "";
}

/******************************************************************************
*                                   COPY                                      *
******************************************************************************/

Location::Location(Location const & copy) : _root(copy._root)
{
	_locate = copy._locate;
	_serverName = copy._serverName;
	_clientMaxBodySize = copy._clientMaxBodySize;
	_get= copy._get;
	_post = copy._post;
	_delete = copy._delete;
	_root = copy._root;
	_index = copy._index;
	_autoindex = copy._autoindex;
	_cgiFileExtension = copy._cgiFileExtension;
	_cgiPathToScript = copy._cgiPathToScript;
	_upload = copy._upload;
	_errorPages = copy._errorPages;
	_cgi = copy._cgi;
	_return = copy._return;
}

Location	&Location::operator=(Location const & rhs)
{
	if (this != &rhs)
	{
		_root = rhs._root;
		_locate = rhs._locate;
		_serverName = rhs._serverName;
		_clientMaxBodySize = rhs._clientMaxBodySize;
		_get= rhs._get;
		_post = rhs._post;
		_delete = rhs._delete;
		_root = rhs._root;
		_index = rhs._index;
		_autoindex = rhs._autoindex;
		_cgiFileExtension = rhs._cgiFileExtension;
		_cgiPathToScript = rhs._cgiPathToScript;
		_upload = rhs._upload;
		_errorPages = rhs._errorPages;
		_cgi = rhs._cgi;
		_return = rhs._return;
	}
	return (*this);
}

/******************************************************************************
*                                DESTRUCTOR                                   *
******************************************************************************/

Location::~Location(void) {
}

/******************************************************************************
*                             MEMBER FUNCTIONS                                *
******************************************************************************/

void	Location::init_location_config(std::vector<std::string>::iterator it, std::vector<std::string> split)
{
	it++;
	while (it != split.end() && (*it).compare("}") != 0)
	{
		std::string 			directive;
		std::string 			parameter;
		std::string 			tmp;
		size_t					find;

		std::string	whitespace = " \t\n\r\v\f";

		find = (*it).find_first_of(whitespace);
		if (find == string::npos) 
			find  = (*it).length();
		directive = (*it).substr(0, find);
		if (find != (*it).length()) 
			parameter = (*it).substr(find + 1, (*it).length());

		if (directive.compare("root") == 0)
		{
			parameter = check_semicolon(parameter);
			if (!dir_exists(parameter))
			{
				std::cout << ANSI_RED << "Error: [" << parameter << "]" << ANSI_RESET;
				throw Location::DirOrFileError();
			}
			this->_root = parameter;
		}
		else if (directive.compare("index") == 0)
		{
			parameter = check_semicolon(parameter);
			this->_index = parameter;
		}
		else if (directive.compare("return") == 0)
		{
			parameter = check_semicolon(parameter);
			this->_return = parameter;
		}
		else if (directive.compare("upload") == 0)
		{
			parameter = check_semicolon(parameter);
			if (!dir_exists(parameter))
			{
				std::cout << ANSI_RED << "Error: [" << parameter << "]" << ANSI_RESET;
				throw Location::DirOrFileError();
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
					throw Location::WrongConfLine();
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
					throw Location::DirOrFileError();
				}
				_cgi.insert ( std::pair<std::string,std::string>(_cgiFileExtension,_cgiPathToScript) );
			}
			else
				std::cout << ANSI_RED << "Error: cgi information missing" << ANSI_RESET << std::endl;
		}
		else if (directive.compare("autoindex") == 0)
		{
			parameter = check_semicolon(parameter);
			if (parameter.compare("on") == 0)
				this->_autoindex = true;
			else if (parameter.compare("off") == 0)
				this->_autoindex = false;
		}
		else
		{
			std::cout << ANSI_RED << "Error: [" << (*it) << "]" << ANSI_RESET;
			throw Location::WrongConfLine();
		}
		it++;
	}
}

/******************************************************************************
*                                   PRINTER                                   *
******************************************************************************/

void			Location::print_location(void)
{
	std::cout << ANSI_CYAN << "location: " << ANSI_RESET << _locate << std::endl;
	std::cout << ANSI_CYAN << "root: " << ANSI_RESET << _root << std::endl;
	std::cout << ANSI_CYAN << "return: " << ANSI_RESET << _return << std::endl;

	std::cout << ANSI_BLUE << "cgi: " << ANSI_RESET << std::endl;
	for(std::map<std::string, std::string>::iterator it = _cgi.begin(); it != _cgi.end(); it++)
		std::cout << "[" << it->first << "] " << it->second << std::endl;
	std::cout << ANSI_CYAN << "GET: " << ANSI_RESET << (_get ? "on" : "off" ) << std::endl;
	std::cout << ANSI_CYAN << "POST: " << ANSI_RESET  << (_post ? "on" : "off" ) << std::endl;
	std::cout << ANSI_CYAN << "DELETE: " << ANSI_RESET << (_delete ? "on" : "off" ) << std::endl;
	std::cout << ANSI_CYAN << "autoindex: " << ANSI_RESET << (_autoindex ? "on" : "off" ) << std::endl;
}

/******************************************************************************
*                                 EXCEPTIONS                                  *
******************************************************************************/

const char *	Location::WrongConfLine::what(void) const throw()
{
	return (" found in configuration file is unknown");
}

const char *	Location::DirOrFileError::what(void) const throw()
{
	return (" directory or file or extension does not exist");
}

/******************************************************************************
*                                     UTILS                                   *
******************************************************************************/

bool	Location::is_method_allowed(std::string method)
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

std::string					Location::getLocate()
{
	return(_locate);
}

std::vector<std::string>	Location::getServerName()
{
	return(_serverName);
}

long long unsigned int					Location::getClientMaxBodySize()
{
	return(_clientMaxBodySize);
}

bool						Location::getGet()
{
	return(_get);
}

bool						Location::getPost()
{
	return(_post);
}

bool						Location::getDelete()
{
	return(_delete);
}

std::string					Location::getRoot()
{
	return(_root);
}

std::string					Location::getIndex()
{
	return (_index);
}

bool						Location::getAutoindex()
{
	return(_autoindex);
}

std::string					Location::getCgiFileExtension()
{
	return(_cgiFileExtension);
}

std::string					Location::getCgiPathToScript()
{
	return(_cgiPathToScript);
}

std::map<std::string, std::string>				Location::getCgi()
{
	return(_cgi);
}

std::string					Location::getUpload()
{
	return(_upload);
}

std::map<int, std::string>	Location::getErrorPages()
{
	return(_errorPages);
}

std::string							Location::getReturn()
{
	return(_return);
}
