#ifndef LOCATION_HPP
# define LOCATION_HPP

# include <vector>
# include <string>

# include "utils.hpp"

class Location
{
	public:

		class WrongConfLine : public std::exception
		{
			public:
				virtual const char * what(void) const throw(); 
		};
		
		Location(void);
		Location(std::string locate);
		Location(Location const & copy);
		~Location(void);

		Location	&operator=(Location const & rhs);

		void	init_location_config(std::vector<std::string>::iterator it, std::vector<std::string> split);
		void	print_location(void);

	private:

		std::string					_locate;

		bool						_get;
		bool						_post;
		bool						_delete;
		int							_returnCode;
		std::string					_redir;
		std::string					_root;
		std::string					_index;
		bool						_autoindex;
		std::string					_cgiFileExtension;
		std::string					_cgiPathToScript;
		std::string					_upload;
};

#endif
