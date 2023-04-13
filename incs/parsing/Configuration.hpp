# include <vector>

class Configuration
{
	public:

		Configuration(void);
		Configuration(unsigned int n);
		Configuration(Configuration const & copy);
		~Configuration(void);

		Configuration	&operator=(Configuration const & rhs); //assignement operator

	private:

		std::vector<int>	_vector;
};
