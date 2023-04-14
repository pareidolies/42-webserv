# include "Configuration.hpp"
# include "webserv.hpp"
# include "utils.hpp"

# include <iostream>
# include <fstream>
# include <sstream>

/******************************************************************************
*                              CONSTRUCTORS                                   *
******************************************************************************/

Configuration::Configuration(void)
{

}

Configuration::Configuration(std::string file) : _file(file)
{
	
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
	}
	return (*this);
}

/******************************************************************************
*                                DESTRUCTOR                                   *
******************************************************************************/

Configuration::~Configuration(void)
{

}

/******************************************************************************
*                                  GETTERS                                    *
******************************************************************************/


/******************************************************************************
*                             MEMBER FUNCTIONS                                *
******************************************************************************/

void	Configuration::print_vector(std::vector<std::string> _vector)
{
	for (size_t i = 0; i < _vector.size(); i++)
		std::cout << _vector[i] << std::endl;
}

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
		std::stringstream ss(line);
		std::string tmp;
		std::string	whitespace = " \t\n\r\v\f";
		std::size_t bracket;
		
		//splitting bracket from data and trimming spaces
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
	/*if (this->_servers.empty())
	{
		std::cout << ANSI_RED << "Error: No server information in the file" << ANSI_RESET << std::endl;
        return false;
	}*/
	ifs.close();
	return true;
}

bool	Configuration::check_second_bracket(std::vector<std::string>::iterator it)
{
	while (it != _split.end() && (*it).compare("}") != 0)
	{
		if((*it).compare("server") == 0)
		{
			std::cout << ANSI_RED << "Error: server detected within another server" << ANSI_RESET << std::endl;
			return false;
		}
		it++;
	}
	if (it == _split.end())
	{
		std::cout << ANSI_RED << "Error: missing closing bracket for server block" << ANSI_RESET << std::endl;
		return false;
	}
	return (true);
}

void	Configuration::init_config(void)
{
	for(std::vector<std::string>::iterator beg = _split.begin(); beg != _split.end(); beg++)
	{
		if ((*beg).compare("server") == 0 && (*(beg + 1)).compare("{") == 0)
		{
			if(check_second_bracket(++beg))
			{
				Server *server = new Server();
				server->init_server_config(beg, _split);
				_servers.push_back(server);
			}
		}
	}
	print_vector(_split);
}