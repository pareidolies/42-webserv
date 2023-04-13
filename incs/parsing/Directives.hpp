# include <vector>

class Directives
{
	public:

		Directives(void);
		Directives(unsigned int n);
		Directives(Directives const & copy);
		~Directives(void);

		Directives	&operator=(Directives const & rhs); //assignement operator

	private:

		std::vector<int>	_vector;
};
