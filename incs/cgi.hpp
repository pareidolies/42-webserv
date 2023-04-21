#ifndef CGI_HPP
# define CGI_HPP

# include <iostream>
# include <map>

# include <sys/types.h>
# include <sys/wait.h>

// # include "RequestConfig.hpp"
// # include "File.hpp"
// # include "StringUtils.hpp"

using namespace std;

class RequestConfig;

class CGI {
public:
    CGI(File &file, RequestConfig &config, map<string, string, ft::comp> &req_headers);
    CGI(File &file, RequestConfig &config, map<string, string, ft::comp> &req_headers, string &req_body);
    ~CGI();

    void init(int worker_id);
    int execute();
    bool setCGIEnv();
    void parseHeaders(map<string, string> &headers);
    string &getBody();

private:
    File &_file;
    RequestConfig &_config;
    map<string, string, ft::comp> &req_headers_;
    string _cgi_path;
    string _cgi_exe;
    string _extension;
    string _cwd;
    string _file_path;
    string _body;
    string _req_body;
    File _tmp_file;
    map<string, string> _cgi_env;
    char **_env;
    char *_argv[3];
};

#endif