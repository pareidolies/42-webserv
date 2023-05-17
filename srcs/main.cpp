# include "webserv.hpp"

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

int main(const int argc, const char** argv, char **env)
{
    (void)env;
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

    try
    {
        Configuration	conf(file);
        conf.open_and_read_file();
        conf.init_config();
        // conf.print_all(); 
        cout << "----------------------------------" << endl;
        CGI cgi(conf);
        cgi.execute();
        cout << "----------------------------------" << endl;
        // TcpServer server = TcpServer("127.0.0.1", 8000);
        // server.startListen();
    }
    catch(std::exception & e)
	{
		std::cout << ANSI_RED << e.what() << ANSI_RESET << std::endl;;
	}
   
    
    // if (isCGI(file_.getMimeExtension()))
    {
        // CGI cgi(file_, config_, config_.getHeaders(), config_.getBody());
        // cgi.init();
        // cgi.init(worker_id_);
        // if ((status_code_ = cgi.execute()) > 400)
        //     return status_code_;
        // cgi.parseHeaders(headers_);
        // body_ = cgi.getBody();
        // headers_["Content-Length"] = ft::to_string(body_.length());
        // return status_code_;
    }

    
    return EXIT_SUCCESS;
}
