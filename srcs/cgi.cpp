#include "cgi.hpp"
#include <fcntl.h>

#define WRITEEND 1
#define READEND 0

CGI::CGI(Configuration &conf) : _conf(conf)
{
	cout << "setting CGI." << endl;
	this->init(0);
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
	// _tmp_file.close();
	// _tmp_file.unlink();
}

void CGI::init(int worker_id)
{
	(void) worker_id;
	cout << "init CGI." << endl;

	char *cwd = getcwd(NULL, 0);
	if (!cwd)
		return ;
	_cwd = cwd;
	free(cwd);
	this->_extension = "php";

	this->_env = NULL;
	this->_argv[0] = NULL;
	this->_argv[1] = NULL;
	this->_argv[2] = NULL;

// 	_cgi_exe = _config.getCGI()[_extension];
// 	if (_config.getCGIBin()[0] == '/') 
// 		_cgi_path = _config.getCGIBin() + "/" + _cgi_exe;
// 	else
// 		_cgi_path = _cwd + "/" + _config.getCGIBin() + "/" + _cgi_exe;
// 	std::string cgi_path = "/tmp/webserv_cgi_tmp_" + General::to_string(worker_id);
// 	_tmp_file.set_path(cgi_path.c_str());
// 	_tmp_file.open(true);
// 	if (worker_id)
// 		Log.print(DEBUG, "worker[" + General::to_string(worker_id) + "] : CGI -> " + _cgi_path);
// 	else
// 		Log.print(DEBUG, "server : CGI -> " + _cgi_path);

	this->_cgi_exe = this->_cwd + "/cgi/php-cgi";
	this->_file_path = this->_cwd + "/www/info.php";

	this->_cgi_env["REQUEST_METHOD"] = "GET";
	this->_cgi_env["CONTENT_TYPE"] = "text/html";
	this->_cgi_env["CONTENT_LENGTH"] = "0";
	this->_cgi_env["SERVER_PROTOCOL"] = "HTTP/1.1";
	this->_cgi_env["GATEWAY_INTERFACE"] = "Cgi/1.1";
	this->_cgi_env["PATH_INFO"] = this->_file_path;
	this->_cgi_env["PATH_TRANSLATED"] = this->_file_path;
	this->_cgi_env[ "SERVER_PORT"] = "8080";
}

// CGI::CGI(File &file, RequestConfig &config, std::map<std::string, std::string, General::comp> &req_headers) : 
//     _file(file), _config(config), _req_headers(req_headers)
// {
// 	_req_body = _file.getContent();
// }

// CGI::CGI(File &file, RequestConfig &config, std::map<std::string, std::string, General::comp> &req_headers, std::string &req_body) : 
//     _file(file), _config(config), _req_headers(req_headers)
// {
// 	if (req_body.empty() && _config.getMethod() != "POST")
// 		_req_body = _file.getContent();
// 	else
// 		_req_body = req_body;
// }

bool CGI::setCGIEnv()
{
	// if (_config.getMethod() == "POST")
	// {
	// 	_cgi_env["CONTENT_TYPE"] = _req_headers["Content-Type"];
	// 	_cgi_env["CONTENT_LENGTH"] = General::to_string(_req_body.length());
	// }
	_cgi_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	// _cgi_env["PATH_INFO"] = _file_path;
	// _cgi_env["PATH_TRANSLATED"] = _file_path;
	// _cgi_env["QUERY_STRING"] = _config.getQuery();
	// _cgi_env["REMOTE_ADDR"] = _config.getClient().getAddr();

	// if (_config.getAuth() != "off")
	// {
	// 	_cgi_env["AUTH_TYPE"] = "Basic";
	// 	_cgi_env["REMOTE_IDENT"] = _config.getAuth().substr(0, _config.getAuth().find(':'));
	// 	_cgi_env["REMOTE_USER"] = _config.getAuth().substr(0, _config.getAuth().find(':'));
	// }

	// _cgi_env["REQUEST_METHOD"] = _config.getMethod();
	// _cgi_env["REQUEST_URI"] = _file_path;

	// _cgi_env["SCRIPT_NAME"] = _cgi_path;
	// _cgi_env["SERVER_NAME"] = _config.getHost();
	// _cgi_env["SERVER_PROTOCOL"] = _config.getProtocol();
	this->_cgi_env["SERVER_PORT"] = General::to_string(8080);
	this->_cgi_env["SERVER_SOFTWARE"] = "WEBSERV/1.0";

	if (this->_extension == ".php")
		this->_cgi_env["REDIRECT_STATUS"] = "200";

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

int CGI::execute()
{
	if (!this->setCGIEnv())
    	return (500);
  	if (!(_argv[0] = strdup(this->_cgi_exe.c_str())))
    	return (500);
  	if (!(_argv[1] = strdup(this->_file_path.c_str())))
 	   return (500);
  	_argv[2] = NULL;

	int pip[2];

	if (pipe(pip) != 0)
		return (500);

	pid_t pid = fork();

	if (pid == 0)
	{
		// if (chdir(_file_path.substr(0, _file_path.find_last_of('/')).c_str()) == -1)
		// 	return (500);
		close(pip[READEND]);
		dup2(pip[WRITEEND], STDOUT_FILENO);
		// if (dup2(pip[0], 0) == -1)
		// 	return (500);
		// if (dup2(_tmp_file.getFd(), 1) == -1)
		// 	return (500);
		// close(pip[0]);
		execve(_argv[0], _argv, _env);
		exit(1);
	}
	else if (pid > 0)
	{
		close(pip[WRITEEND]);
		char buffer[1024];
		int nread = read(pip[READEND], buffer, sizeof(buffer));
		if (nread > 0)
		{
			cout << "exec_result: " << endl;
			printf("%.*s", nread, buffer);
		}
		close(pip[READEND]);
		// if (_req_body.length() && write(pip[WRITEEND], _req_body.c_str(), _req_body.length()) <= 0)
		// 	return (500);

		int status;
		if (waitpid(pid, &status, 0) == -1)
			return (500);
		if (WIFEXITED(status) && WEXITSTATUS(status))
			return (502);
	}
	else
		return (502);

	// _body = _tmp_file.getContent();
	return (200);
}

// void CGI::parseHeaders(std::map<std::string, std::string> &headers) {
//   size_t end, last;
//   std::string header;

//   while ((end = _body.find("\r\n")) != std::string::npos) {
//     if (_body.find("\r\n") == 0) {
//       _body.erase(0, end + 2);
//       break;
//     }
//     if ((last = _body.find(':', 0)) != std::string::npos) {
//       header = _body.substr(0, last);
//       headers[header] = General::trim_left(_body.substr(last + 1, end - last - 1), ' ');
//     }
//     _body.erase(0, end + 2);
//   }
//   if (headers.count("Content-Length")) {
//     size_t size = General::stoi(headers["Content-Length"]);

//     _body.erase(size);
//   }
// }
