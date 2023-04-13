# include "Configuration.hpp"
# include "webserv.hpp"

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

bool	Configuration::open_file(void) 
{
	std::ifstream				ifs(_file.c_str());
	std::string					line;

	if (ifs.fail())
    {
        std::cout << ANSI_RED << "Error: could not open file" << ANSI_RESET << std::endl;
        return false;
    }
	while (std::getline(ifs, line))
	{
		//find server block
	}
	if (this->_servers.empty())
	{
		std::cout << ANSI_RED << "Error: No server information in the file" << ANSI_RESET << std::endl;
        return false;
	}
	ifs.close();
	return true;
}

