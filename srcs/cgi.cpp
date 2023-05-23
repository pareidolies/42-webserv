#include "cgi.hpp"
#include <fcntl.h>

#define WRITEEND 1
#define READEND 0

CGI::CGI(Response *response) : _response(*response)
{
	cout << "setting CGI. response" << endl;
	this->_env = NULL;
	this->_extension = this->_response.get_extension();
	this->_cgi_exe = this->_response.get_cgi_map()[this->_extension];
	this->_req_body = this->_response.get_request_body();
	this->_file_path = this->_response.get_path();
	this->_status_code = this->init_arg();
	if (this->_status_code != 200)
		return ;
	this->_status_code = this->init_env();
}

std::string &CGI::getBody()
{
	return	(this->_body);
}

CGI::~CGI()
{
	free(_argv[0]);
	free(_argv[1]);
	if (_env)
		General::free_tab(_env);	
}

int	CGI::init_arg()
{
	char *cwd = getcwd(NULL, 0);
	if (!cwd)
		return (500);
	_cwd = cwd;
	free(cwd);
	this->_file_path = this->_cwd + "/" + (this->_file_path.substr(2));

	if (!(_argv[0] = strdup(this->_cgi_exe.c_str())))
        return (500);
    if (!(_argv[1] = strdup(this->_file_path.c_str())))
        return (500);
    _argv[2] = NULL;
	return (200);
}

int CGI::init_env()
{
	cout << "init CGI." << endl;
	this->_cgi_env["REQUEST_METHOD"] = this->_response.get_method();
	this->_cgi_env["CONTENT_TYPE"] = this->_response.get_content_type();
	this->_cgi_env["CONTENT_LENGTH"] = "0";
	this->_cgi_env["SERVER_PROTOCOL"] = "HTTP/1.1";
	this->_cgi_env["GATEWAY_INTERFACE"] = "Cgi/1.1";
	this->_cgi_env["SERVER_SOFTWARE"] = "WEBSERV/1.0";
	this->_cgi_env["PATH_INFO"] = this->_file_path;	
	this->_cgi_env["PATH_TRANSLATED"] = this->_file_path;
	// this->_cgi_env["SERVER_NAME"] = _config.getHost();
	this->_cgi_env[ "SERVER_PORT"] = General::to_string(this->_response.get_server()->getPort());
	if (this->_extension == ".php")
		this->_cgi_env["REDIRECT_STATUS"] = "200";

	if (this->_response.get_method() == "GET")
		this->_cgi_env["QUERY_STRING"] = this->_req_body;
	else if (this->_response.get_method() == "POST")
	{
		this->_cgi_env["CONTENT_LENGTH"] = General::to_string(this->_req_body.length());
		this->_cgi_env["PAYLOAD"] = this->_req_body;
	}
	else 
		return (400);
	return (200);
}


bool CGI::setCGIEnv()
{
	// _cgi_env["REMOTE_ADDR"] = _config.getClient().getAddr();

	// if (_config.getAuth() != "off")
	// {
	// 	_cgi_env["AUTH_TYPE"] = "Basic";
	// 	_cgi_env["REMOTE_IDENT"] = _config.getAuth().substr(0, _config.getAuth().find(':'));
	// 	_cgi_env["REMOTE_USER"] = _config.getAuth().substr(0, _config.getAuth().find(':'));
	// }

	// _cgi_env["REQUEST_URI"] = _file_path;

	// _cgi_env["SCRIPT_NAME"] = _cgi_path;

	// for (std::map<std::string, std::string>::iterator it = _req_headers.begin(); it != _req_headers.end(); it++)
	// {
	// 	if (!it->second.empty())
	// 	{
	// 		string header = "HTTP_" + General::to_upper(it->first);
	// 		std::replace(header.begin(), header.end(), '-', '_');
	// 		_cgi_env[header] = it->second;
	// 	}
	// }

	if (!(_env = (char **)malloc(sizeof(char *) * (_cgi_env.size() + 1))))
		return (false);

	int i = 0;
	for (std::map<std::string, std::string>::iterator it = _cgi_env.begin(); it != _cgi_env.end(); it++)
	{
		std::string tmp = it->first + "=" + it->second;
		if (!(_env[i] = strdup(tmp.c_str())))
			return (false);
		i++;
	}
	_env[i] = NULL;
	return (true);
}

void CGI::readFromPipe(int pipefd)
{
    std::vector<char>	buffer(4096);
    ssize_t				bytesRead;
	this->_body = "";

    while ((bytesRead = read(pipefd, buffer.data(), 4096)) > 0)
        this->_body.append(buffer.data(), bytesRead);
}

int CGI::execute()
{
    // _req_body = "name=John+Doe&age=3";
	cout << "req body: " << this->_req_body << endl;

    if (!this->setCGIEnv())
        return (500);

    int pip[2];
	int output_pipe[2];
	char buffer[4096];
	if (pipe(pip) == -1)
		return (500);
	if (pipe(output_pipe) == -1)
	{
		close(pip[READEND]);
		close(pip[WRITEEND]);
		return (500);
	}

    pid_t pid = fork();
    if (pid == -1)
        return 1;
    else if (pid == 0)
	{
        // Child process: redirect STDIN to the read end of the pipe
        close(pip[WRITEEND]);
        if (dup2(pip[READEND], STDIN_FILENO) == -1)
            return 500;
        close(pip[READEND]);

        // Redirect STDOUT to a pipe
        dup2(output_pipe[WRITEEND], STDOUT_FILENO);
        close(output_pipe[WRITEEND]);

        execve(_argv[0], _argv, _env);
        exit(1);
    }
	else
	{
        // Parent process: write the request body to the write end of the pipe
        close(pip[READEND]);
        if (_req_body.length() && write(pip[WRITEEND], _req_body.c_str(), _req_body.length()) <= 0)
            return (500);
        close(pip[WRITEEND]);

        // Read the output of the child process from the pipe
        close(output_pipe[WRITEEND]);
		readFromPipe(output_pipe[READEND]);
        close(output_pipe[READEND]);

        int status;
        if (waitpid(pid, &status, 0) == -1)
            return (500);
        if (WIFEXITED(status) && WEXITSTATUS(status))
            return (502);
    }
    return (200);
}