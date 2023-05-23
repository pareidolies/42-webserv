#ifndef CGI_HPP
# define CGI_HPP

# include <sys/wait.h>

#include "webserv.hpp"
#include "Response.hpp"

class Response; // Forward declaration of Response class

// #define BUFFER_SIZE 4096

using namespace std;

class RequestConfig;

class CGI {
    public:
        CGI(Response *response);
        ~CGI();

        string  &getBody();
        int     init_arg();
        int     init_env();
        int     execute();
        bool    setCGIEnv();
        void    parseHeaders(map<string, string> &headers);
        void    readFromPipe(int pipefd);
        int     getStatusCode();

    private:
        Response           _response;
        int                 _status_code;
        string              _cgi_exe;
        string              _extension;
        string              _cwd;
        string              _file_path;
        string              _body;
        string              _req_body;
        map<string, string> _cgi_env;
        char                **_env;
        char                *_argv[3];
};

#endif