# include <vector>
# include <string>

class Configuration
{
	public:

		Configuration(void);
		Configuration(std::string file);
		Configuration(Configuration const & copy);
		~Configuration(void);

		Configuration	&operator=(Configuration const & rhs); //assignement operator

	private:

		std::vector<int>	_vector;
		std::string			_file;
};
