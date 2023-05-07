#ifndef CONFIGURATION_HPP
# define CONFIGURATION_HPP

# include "Server.hpp"
# include "utils.hpp"

# include <vector>
# include <string>

class Configuration
{
	public:

		class WrongBracketsNbr : public std::exception
		{
			public:
				virtual const char * what(void) const throw(); 
		};
		class NoServer : public std::exception
		{
			public:
				virtual const char * what(void) const throw(); 
		};

		Configuration(void);
		Configuration(std::string file);
		Configuration(Configuration const & copy);
		~Configuration(void);

		Configuration	&operator=(Configuration const & rhs);

		bool			open_and_read_file(void);
		void			init_config(void);
		void			print_all(void);

	private:

		std::string					_file;
		std::vector<Server*>		_servers;
		std::vector<std::string>	_split;
};

std::ostream &operator<<( std::ostream & o, Configuration const & rhs);

#endif
