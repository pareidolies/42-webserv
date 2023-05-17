#ifndef CGI_HPP
# define CGI_HPP

# include <iostream>
# include <map>

# include <sys/types.h>
# include <sys/wait.h>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>


// # include "RequestConfig.hpp"
// # include "File.hpp"
// # include "StringUtils.hpp"
#include "Configuration.hpp"

using namespace std;

class RequestConfig;

class CGI {
    public:
        CGI(Configuration &conf);
        // CGI(File &file, RequestConfig &config, map<string, string, ft::comp> &req_headers);
        // CGI(File &file, RequestConfig &config, map<string, string, ft::comp> &req_headers, string &req_body);
        ~CGI();

        string  &getBody();
        void    init(int worker_id);
        int     execute();
        bool    setCGIEnv();
        void    parseHeaders(map<string, string> &headers);

    private:
        Configuration      &_conf;
        // File            &_file;
        // RequestConfig   &_config;
        // map<string, string, ft::comp> &_req_headers;
        // map<string, string> &_req_headers;
        // string          _cgi_path;
        string              _cgi_exe;
        string              _extension;
        string              _cwd;
        string              _file_path;
        string              _body;
        string              _req_body;
        // File            _tmp_file;
        map<string, string> _cgi_env;
        char                **_env;
        char                *_argv[3];
};

#endif