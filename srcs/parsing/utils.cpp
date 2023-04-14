# include "utils.hpp"

# include <vector>
# include <iostream>

std::string	trim(const std::string & str, const std::string & whitespace)
{
	;
    size_t strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos)
        return "";

	size_t	strEnd = str.find_last_not_of(whitespace);
    size_t	strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}

void	print_vector(std::vector<std::string> _vector)
{
	for (size_t i = 0; i < _vector.size(); i++)
		std::cout << _vector[i] << std::endl;
}