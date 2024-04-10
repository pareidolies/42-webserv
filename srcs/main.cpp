# include "webserv.hpp"

# include <string>
# include <iostream>
# include <signal.h>

std::sig_atomic_t g_shutdown = 1;

void	signal_handler(int signal) {
	if (signal == SIGINT)
		g_shutdown = 0;
}

int	check_filename(const char *name, const char *ext)
{
	int	x;

	x = strlen(name) - strlen(ext);
	if (strncmp(name + x, ext, strlen(ext)))
		return (0);
	return (1);
}

int main(const int argc, const char** argv, char **env)
{
    (void)env;
    std::string file;

    std::signal(SIGINT, signal_handler);

    if (argc > 2 || (argc == 2 && !check_filename(argv[1], "conf")))
	{
		std::cerr << ANSI_RED << "Error: only one configuration file expected" << ANSI_RESET << std::endl;
		return EXIT_FAILURE;
	}
    if (argc == 2)
		file = argv[1];
    else
        file = "./conf_files/default.conf";

    try
    {
        Configuration	conf(file);
        conf.open_and_read_file();
        conf.init_config();

        TcpServer server = TcpServer(conf);
        server.run();
    }
    catch(std::exception & e)
	{
		std::cout << ANSI_RED << e.what() << ANSI_RESET << std::endl;;
	}
   
    return EXIT_SUCCESS;
}
