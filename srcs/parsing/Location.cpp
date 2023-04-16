# include "Location.hpp"
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
			this->_upload = parameter;
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
		else if (directive.compare("cgi") == 0) //if multiple
		{
			parameter = check_semicolon(parameter);
			find = parameter.find_first_of(whitespace);
			if (find != string::npos)
			{
				this->_cgiFileExtension = parameter.substr(0, find);
				parameter.erase(0, find + 1);
				this->_cgiPathToScript = trim(parameter, whitespace);
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
			std::cout << ANSI_RED << "Error: unknown option detected: " << (*it) << ANSI_RESET << std::endl;
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
