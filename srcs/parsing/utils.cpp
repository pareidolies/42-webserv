# include "webserv.hpp"
# include "utils.hpp"

# include <vector>
# include <iostream>

std::string	trim(const std::string & str, std::string & whitespace)
{
    size_t strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos)
        return "";

	size_t	strEnd = str.find_last_not_of(whitespace);
    size_t	strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}

void	    print_vector(std::vector<std::string> _vector)
{
	for (size_t i = 0; i < _vector.size(); i++)
		std::cout << _vector[i] << std::endl;
}

std::string check_semicolon(std::string str)
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

bool	    check_second_bracket(std::vector<std::string>::iterator it, std::vector<std::string> vector, std::string & type)
{
	while (it != vector.end() && (*it).compare("}") != 0)
	{
		if((*it).compare(type) == 0)
		{
			std::cout << ANSI_RED << "Error: " << type << " detected within another" << ANSI_RESET << std::endl;
			return false;
		}
		it++;
	}
	if (it == vector.end())
	{
		std::cout << ANSI_RED << "Error: missing closing bracket for " << type << " block" << ANSI_RESET << std::endl;
		return false;
	}
	return (true);
}
