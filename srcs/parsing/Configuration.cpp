# include "webserv.hpp"

# include <iostream>
# include <fstream>
# include <sstream>

/******************************************************************************
*                              CONSTRUCTORS                                   *
******************************************************************************/

Configuration::Configuration(void)
{
	//initialize values
	//std::cout << ANSI_RED << "BUILD2" << ANSI_RESET << std::endl;
}

Configuration::Configuration(std::string file) : _file(file)
{
		//std::cout << ANSI_RED << "BUILD1" << ANSI_RESET << std::endl;
}

/******************************************************************************
*                                   COPY                                      *
******************************************************************************/

Configuration::Configuration(Configuration const & copy) : _servers(copy._servers)
{

}

Configuration	&Configuration::operator=(Configuration const & rhs)
{
	if (this != &rhs)
	{
		_servers = rhs._servers;
		//to complete
	}
	return (*this);
}

/******************************************************************************
*                                DESTRUCTOR                                   *
******************************************************************************/

Configuration::~Configuration(void)
{	
	//std::cout << ANSI_RED << "DESTROY" << ANSI_RESET << std::endl;
	//if (g_shutdown == 1)
	//{
		for(std::vector<Server*>::iterator it = _servers.begin(); it != _servers.end(); it++)
		{
			// (*it)->print_server();
			if (*it)
				delete	*it;
		}
	//}
}

/******************************************************************************
*                                  GETTERS                                    *
******************************************************************************/

std::vector<Server*>		Configuration::getServers(void)
{
	return (_servers);
}

/******************************************************************************
*                             MEMBER FUNCTIONS                                *
******************************************************************************/

bool	Configuration::open_and_read_file(void) 
{
	std::ifstream				ifs(_file.c_str());
	std::string					line;


	if (ifs.fail()) //need to check if directory
    {
        std::cout << ANSI_RED << "Error: could not open file" << ANSI_RESET << std::endl;
        return false;
    }
	while (std::getline(ifs, line))
	{
		std::stringstream	ss(line);
		std::string 		tmp;
		std::string			whitespace = " \t\n\r\v\f";
		std::size_t			bracket;
		//splitting bracket from data and trimming spaces
		if (line.find("#") != std::string::npos)//remove comments
			continue;
		bracket = line.find("{");
  		if (bracket!=std::string::npos)
		{
			getline(ss, tmp, '{');
			if (!trim(tmp, whitespace).empty())
				_split.push_back(trim(tmp, whitespace));
			_split.push_back("{");
			continue;
		}
		bracket = line.find("}");
		if (bracket!=std::string::npos)
		{
			getline(ss, tmp, '}');
			if (!trim(tmp, whitespace).empty())
				_split.push_back(trim(tmp, whitespace));
			_split.push_back("}");
			continue;
		}
		getline(ss, tmp);
		if (!trim(tmp, whitespace).empty() && line.find("#")==std::string::npos) //remove empty lines and comments lines
			_split.push_back(trim(tmp, whitespace));
	}
	ifs.close();
	if (!check_brackets_number(_split))
		throw Configuration::WrongBracketsNbr();
	return true;
}

void	Configuration::init_config(void)
{
	std::string	type = "server";

	for(std::vector<std::string>::iterator beg = _split.begin(); beg != _split.end(); beg++)
	{
		if ((*beg).compare("server") == 0 && (*(beg + 1)).compare("{") == 0)
		{
			if(check_second_bracket(++beg, _split, type))
			{
				Server *server = new Server();
				server->init_server_config(beg, _split);
				_servers.push_back(server);
			}
		}
	}
	if (this->_servers.empty())
		throw Configuration::NoServer();
	//print_all();
	//print_vector(_split);
}

/******************************************************************************
*                                  PRINTER                                    *
******************************************************************************/

void	Configuration::print_all(void)
{
	for(std::vector<Server*>::iterator it = this->_servers.begin(); it != this->_servers.end(); it++)
	{	
		// std::cout << ANSI_YELLOW << "------ SERVER " << ++i <<" ------" << ANSI_RESET << std::endl;
		// (*it)->print_server();
		std::cout << std::endl;
	}
}

/******************************************************************************
*                                 EXCEPTIONS                                  *
******************************************************************************/

const char *	Configuration::WrongBracketsNbr::what(void) const throw()
{
	return ("Error: Wrong brackets number");
}

const char *	Configuration::NoServer::what(void) const throw()
{
	return ("Error: No server information in the file");
}
