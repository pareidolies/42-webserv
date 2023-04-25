#ifndef CONFIG_UTILS_HPP
# define CONFIG_UTILS_HPP

# include <string>
# include <vector>
# include <iostream>
# include <fstream>
# include <sstream>

std::string	trim(const std::string & str, std::string & whitespace);
std::string check_semicolon(std::string str);

void	    print_vector(std::vector<std::string> _vector);

bool	    check_brackets_number(std::vector<std::string> _vector);
bool	    check_second_bracket(std::vector<std::string>::iterator it, std::vector<std::string> vector, std::string & type);

bool	    dir_exists(std::string str);
bool	    file_exists(std::string str);
bool	    is_extension(std::string str);

#endif