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

		bool			open_file(void);
		void			print_vector(std::vector<std::string> _vector);
		std::string		trim(const std::string & str);

	private:

		std::string					_file;
		std::vector<Server>			_servers;
		std::vector<std::string>	_split;
};

std::ostream &operator<<( std::ostream & o, Configuration const & rhs);

#endif
