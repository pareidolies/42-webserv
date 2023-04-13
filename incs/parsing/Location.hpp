#ifndef LOCATION_HPP
# define LOCATION_HPP

# include <vector>

class Location
{
	public:

		Location(void);
		Location(Location const & copy);
		~Location(void);

		Location	&operator=(Location const & rhs);

	private:

		std::vector<int>	_vector;
};

#endif
