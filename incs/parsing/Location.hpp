# include <vector>

class Location
{
	public:

		Location(void);
		Location(unsigned int n);
		Location(Location const & copy);
		~Location(void);

		Location	&operator=(Location const & rhs); //assignement operator

	private:

		std::vector<int>	_vector;
};
