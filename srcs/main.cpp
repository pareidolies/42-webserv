# include "webserv.hpp"
# include "Configuration.hpp"

# include <string>
# include <iostream>

int main(const int argc, const char** argv)
{
    std::string file;

    if (argc > 2)
	{
		std::cerr << ANSI_RED << "Error: only one configuration file expected" << ANSI_RESET << std::endl;
		return EXIT_FAILURE;
	}
    if (argc == 2)
		file = argv[1];
    else
        file = "./conf_files/default.conf";

    Configuration	conf(file);

    TcpServer server = TcpServer("127.0.0.1", 8001);
    server.startListen();
    
    return EXIT_SUCCESS;
}
