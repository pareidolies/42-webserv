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

	private:

		std::string			_file;
		std::vector<Server>	_servers;
};

#endif
