#ifndef DIRECTIVES_HPP
# define DIRECTIVES_HPP

# include <vector>

class Directives
{
	public:

		Directives(void);
		Directives(Directives const & copy);
		virtual ~Directives(void);

		Directives	&operator=(Directives const & rhs);

	protected:

		std::vector<int>	_vector;
};

#endif
