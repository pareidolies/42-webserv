# include "webserv.hpp"
# include "Configuration.hpp"

# include <string>
# include <iostream>

int	check_filename(const char *name, const char *ext)
{
	int	x;

	x = strlen(name) - strlen(ext);
	if (strncmp(name + x, ext, strlen(ext)))
		return (0);
	return (1);
}

int main(const int argc, const char** argv)
{
    std::string file;

    if (argc > 2 || (argc == 2 && !check_filename(argv[1], "conf")))
	{
		std::cerr << ANSI_RED << "Error: only one configuration file expected" << ANSI_RESET << std::endl;
		return EXIT_FAILURE;
	}
    if (argc == 2)
		file = argv[1];
    else
        file = "./conf_files/default.conf";

    Configuration	conf(file);
    conf.open_and_read_file();
    conf.init_config();
    conf.print_all();

    //TcpServer server = TcpServer("127.0.0.1", 8001);
    //server.startListen();
    
    return EXIT_SUCCESS;
}
