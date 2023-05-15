# include "webserv.hpp"

/******************************************************************************
*                              CONSTRUCTORS                                   *
******************************************************************************/

Location::Location(void)
{
	//initialize values
}

Location::Location(std::string locate) : _locate(locate)
{
	//initialize values
}

/******************************************************************************
*                                   COPY                                      *
******************************************************************************/

Location::Location(Location const & copy) : _root(copy._root)
{

}

Location	&Location::operator=(Location const & rhs)
{
	if (this != &rhs)
	{
		_root = rhs._root;
	}
	return (*this);
}

/******************************************************************************
*                                DESTRUCTOR                                   *
******************************************************************************/

Location::~Location(void)
{

}

/******************************************************************************
*                                  GETTERS                                    *
******************************************************************************/


/******************************************************************************
*                             MEMBER FUNCTIONS                                *
******************************************************************************/

void	Location::init_location_config(std::vector<std::string>::iterator it, std::vector<std::string> split)
{
	it++;
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
	std::cout << ANSI_CYAN << "cgi file extension: " << ANSI_RESET << _cgiFileExtension << std::endl;
	std::cout << ANSI_CYAN << "cgi path to script: " << ANSI_RESET << _cgiPathToScript << std::endl;
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
