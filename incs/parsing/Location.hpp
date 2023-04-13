#ifndef LOCATION_HPP
# define LOCATION_HPP

# include "Directives.hpp"

# include <vector>
# include <string>


class Location : public Directives
{
	public:

		Location(void);
		Location(Location const & copy);
		~Location(void);

		Location	&operator=(Location const & rhs);

	private:

		std::vector<std::string>	_vector;
};

#endif
