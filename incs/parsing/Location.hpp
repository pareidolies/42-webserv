#ifndef LOCATION_HPP
# define LOCATION_HPP

# include <vector>
# include <string>

# include "utils.hpp"

class Location
{
	public:

		Location(void);
		Location(Location const & copy);
		~Location(void);

		Location	&operator=(Location const & rhs);

	private:

		std::vector<std::string>	_vector;
		bool						_get;
		bool						_post;
		bool						_delete;
		int							_returnCode;
		std::string					_redirectionPath;
		std::string					_rootPath;
		std::string					_indexPath;
		bool						_autoindex;
		std::string					_cgiFileExtension;
		std::string					_cgiPathToScript;
		std::string					_uploadDir;
		std::string					_locate;
};

#endif
