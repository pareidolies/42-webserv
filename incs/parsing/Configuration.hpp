#ifndef CONFIGURATION_HPP
# define CONFIGURATION_HPP

# include "Server.hpp"

# include <vector>
# include <string>

class Configuration
{
	public:

		Configuration(void);
		Configuration(std::string file);
		Configuration(Configuration const & copy);
		~Configuration(void);

		Configuration	&operator=(Configuration const & rhs);

		bool			open_and_read_file(void);
		void			print_vector(std::vector<std::string> _vector);
		void			init_config(void);
		bool			check_second_bracket(std::vector<std::string>::iterator it);

	private:

		std::string					_file;
		std::vector<Server*>		_servers;
		std::vector<std::string>	_split;
};

std::ostream &operator<<( std::ostream & o, Configuration const & rhs);

#endif
