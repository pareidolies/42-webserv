#ifndef SERVER_HPP
# define SERVER_HPP

# include "Directives.hpp"
# include "Location.hpp"

# include <vector>

class Server : public Directives
{
	public:

		Server(void);
		Server(Server const & copy);
		~Server(void);

		Server	&operator=(Server const & rhs);

	private:

		std::vector<Location>	_locations;
};

#endif
