#ifndef CONFIG_UTILS_HPP
# define CONFIG_UTILS_HPP

# include <string>
# include <vector>

std::string	trim(const std::string & str, std::string & whitespace);
void	    print_vector(std::vector<std::string> _vector);
std::string check_semicolon(std::string str);
bool	    check_second_bracket(std::vector<std::string>::iterator it, std::vector<std::string> vector, std::string & type);

#endif