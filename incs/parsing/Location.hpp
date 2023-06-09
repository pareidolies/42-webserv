#ifndef LOCATION_HPP
# define LOCATION_HPP

# include <vector>
# include <string>
# include <map>

# include "utils.hpp"

class Location
{
	public:

		class WrongConfLine : public std::exception
		{
			public:
				virtual const char * what(void) const throw(); 
		};
		class DirOrFileError : public std::exception
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

		//getters
		std::string					getLocate();
		std::vector<std::string>	getServerName();
		long long unsigned int		getClientMaxBodySize();
		bool						getGet();
		bool						getPost();
		bool						getDelete();
		std::string					getRoot();
		std::string					getIndex();
		bool						getAutoindex();
		std::string					getCgiFileExtension();
		std::string					getCgiPathToScript();
		std::map<std::string, std::string>					getCgi();
		std::string					getUpload();
		std::map<int, std::string>	getErrorPages();
		bool						is_method_allowed(std::string method);
		std::string					getReturn();


	private:

		std::string					_locate;

		std::vector<std::string>	_serverName;
		long long unsigned int		_clientMaxBodySize;
		bool						_get;
		bool						_post;
		bool						_delete;
		std::string					_root;
		std::string					_index;
		bool						_autoindex;
		std::string					_cgiFileExtension;
		std::string					_cgiPathToScript;
		std::map<std::string, std::string>		_cgiFile;
		std::string					_upload;
		std::map<int, std::string>	_errorPages;
		std::map<std::string, std::string>		_cgi;
		std::string					_return;
};

#endif
