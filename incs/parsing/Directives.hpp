#ifndef DIRECTIVES_HPP
# define DIRECTIVES_HPP

# include <vector>

class Directives
{
	public:

		Directives(void);
		Directives(Directives const & copy);
		~Directives(void);

		Directives	&operator=(Directives const & rhs); //assignement operator

	private:

		std::vector<int>	_vector;
};

#endif
