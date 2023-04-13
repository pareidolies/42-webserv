#ifndef SERVER_HPP
# define SERVER_HPP

# include <vector>

class Server
{
	public:

		Server(void);
		Server(Server const & copy);
		~Server(void);

		Server	&operator=(Server const & rhs);

	private:

		std::vector<int>	_vector;
};

#endif
