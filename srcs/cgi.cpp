#include "cgi.hpp"
#include <fcntl.h>


CGI::CGI(Configuration &conf) : _conf(conf)
{
	cout << "setting CGI." << endl;
	this->parsing();
}

void CGI::parsing()
{
	cout << "parsing CGI." << endl;
	
	// // char *argv_tmp[3] = { "php-cgi", "/mnt/nfs/homes/ykuo/project/github/42-webserv/wwww/info.php", NULL };
	// this->_argv[0] = "php-cgi";
	// this->_argv[1] = "/mnt/nfs/homes/ykuo/project/github/42-webserv/wwww/info.php";
	// this->_argv[2] = NULL;
	// this->_env = {
	// 	"REQUEST_METHOD=GET",
	// 	"QUERY_STRING=foo=bar",
	// 	"CONTENT_TYPE=text/html",
	// 	"CONTENT_LENGTH=0",
	// 	NULL
	// };
	this->_cgi_exe = "/mnt/nfs/homes/ykuo/project/github/42-webserv/cgi/php-cgi";
}

// CGI::CGI(File &file, RequestConfig &config, std::map<std::string, std::string, ft::comp> &req_headers) : 
//     _file(file), _config(config), req_headers_(req_headers)
// {
// 	_req_body = _file.getContent();
// }

// CGI::CGI(File &file, RequestConfig &config, std::map<std::string, std::string, ft::comp> &req_headers, std::string &req_body) : 
//     _file(file), _config(config), req_headers_(req_headers)
// {
// 	if (req_body.empty() && _config.getMethod() != "POST")
// 		_req_body = _file.getContent();
// 	else
// 		_req_body = req_body;
// }

// void CGI::init(int worker_id) {
// 	char *cwd = getcwd(NULL, 0);
// 	if (!cwd)
// 		return ;
// 	_cwd = cwd;
// 	free(cwd);

// 	env_ = NULL;
// 	_argv[0] = NULL;
// 	_argv[1] = NULL;
// 	_argv[2] = NULL;
// 	_extension = _file.getMimeExtension();
// 	_cgi_exe = _config.getCGI()[_extension];
// 	if (_config.getCGIBin()[0] == '/') 
// 		_cgi_path = _config.getCGIBin() + "/" + _cgi_exe;
// 	else
// 		_cgi_path = _cwd + "/" + _config.getCGIBin() + "/" + _cgi_exe;
// 	std::string cgi_path = "/tmp/webserv_cgi_tmp_" + ft::to_string(worker_id);
// 	_tmp_file.set_path(cgi_path.c_str());
// 	_tmp_file.open(true);
// 	if (worker_id)
// 		Log.print(DEBUG, "worker[" + ft::to_string(worker_id) + "] : CGI -> " + _cgi_path);
// 	else
// 		Log.print(DEBUG, "server : CGI -> " + _cgi_path);
// }

CGI::~CGI() {
//   free(_argv[0]);
//   free(_argv[1]);
//   if (env_)
//     ft::free_tab(env_);
//   _tmp_file.close();
//   _tmp_file.unlink();
}

// int CGI::execute() {
//   _file_path = _cwd + "/" + _file.getPath();

//   if (!setCGIEnv())
//     return 500;
//   if (!(_argv[0] = ft::strdup(_cgi_path.c_str())))
//     return 500;
//   if (!(_argv[1] = ft::strdup(_file_path.c_str())))
//     return 500;
//   _argv[2] = NULL;

//   int pip[2];

//   if (pipe(pip) != 0)
//     return 500;

//   pid_t pid = fork();

//   if (pid == 0) {
//     if (chdir(_file_path.substr(0, _file_path.find_last_of('/')).c_str()) == -1)
//       return 500;
//     close(pip[1]);
//     if (dup2(pip[0], 0) == -1)
//       return 500;
//     if (dup2(_tmp_file.getFd(), 1) == -1)
//       return 500;
//     close(pip[0]);
//     execve(_argv[0], _argv, env_);
//     exit(1);
//   }
//   else if (pid > 0) {
//     close(pip[0]);
//     if (_req_body.length() && write(pip[1], _req_body.c_str(), _req_body.length()) <= 0)
//       return 500;
//     close(pip[1]);

//     int status;

//     if (waitpid(pid, &status, 0) == -1)
//       return 500;
//     if (WIFEXITED(status) && WEXITSTATUS(status))
//       return 502;
//   }
//   else {
//     return 502;
//   }

//   _body = _tmp_file.getContent();
//   return 200;
// }

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
//       headers[header] = ft::trim_left(_body.substr(last + 1, end - last - 1), ' ');
//     }
//     _body.erase(0, end + 2);
//   }
//   if (headers.count("Content-Length")) {
//     size_t size = ft::stoi(headers["Content-Length"]);

//     _body.erase(size);
//   }
// }

// std::string &CGI::getBody() {
//   return _body;
// }

bool CGI::setCGIEnv() {
	// if (_config.getMethod() == "POST")
	// {
	// 	_cgi_env["CONTENT_TYPE"] = req_headers_["Content-Type"];
	// 	_cgi_env["CONTENT_LENGTH"] = ft::to_string(_req_body.length());
	// }
	// _cgi_env["GATEWAY_INTERFACE"] = "CGI/1.1";
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
	// _cgi_env["SERVER_PORT"] = ft::to_string(_config.getPort());
	// _cgi_env["SERVER_SOFTWARE"] = "WEBSERV/1.0";

	// if (_extension == ".php")
	// 	_cgi_env["REDIRECT_STATUS"] = "200";

	// for (std::map<std::string, std::string, ft::comp>::iterator it = req_headers_.begin(); it != req_headers_.end(); it++)
	// {
	// 	if (!it->second.empty())
	// 	{
	// 		std::string header = "HTTP_" + ft::to_upper(it->first);
	// 		std::replace(header.begin(), header.end(), '-', '_');
	// 		_cgi_env[header] = it->second;
	// 	}
	// }

	// if (!(env_ = (char **)malloc(sizeof(char *) * (_cgi_env.size() + 1))))
	// 	return false;

	// int i = 0;

	// for (std::map<std::string, std::string>::iterator it = _cgi_env.begin(); it != _cgi_env.end(); it++)
	// {
	// 	std::string tmp = it->first + "=" + it->second;
	// 	if (!(env_[i] = ft::strdup(tmp.c_str())))
	// 		return false;
	// 	i++;
	// }
	// env_[i] = NULL;
	return true;
}

#define WRITEEND 1
#define READEND 0

int CGI::execute(char **envp)
{
	(void ) envp;
	string l1 = "/usr/bin/php-cgi";
	string l2 = "/mnt/nfs/homes/ykuo/project/github/42-webserv/www/info.php";
	string l3 = "REQUEST_METHOD=GET";
	string l4 = "CONTENT_TYPE=text/html";
	string l5 = "CONTENT_LENGTH=0";
	string l6 = "SERVER_PROTOCOL=HTTP/1.1";
	string l7 = "GATEWAY_INTERFACE=Cgi/1.1";
	string l8 = "PATH_INFO=/mnt/nfs/homes/ykuo/project/github/42-webserv/www/info.php";
	string l9 = "PATH_TRANSLATED=/mnt/nfs/homes/ykuo/project/github/42-webserv/www/info.php";
	string l10 = "QUERY_STRING=";
	string l11 = "REDIRECT_STATUS=200";
	string l12 = "SERVER_PORT=8080";
	
	char *c1 = strdup(l1.c_str());
	char *c2 = strdup(l2.c_str());
	const char *c3 = l3.c_str();
	const char *c4 = l4.c_str();
	const char *c5 = l5.c_str();
	const char *c6 = l6.c_str();
	const char *c7 = l7.c_str();
	const char *c8 = l8.c_str();
	const char *c9 = l9.c_str();
	const char *c10 = l10.c_str();
	const char *c11 = l11.c_str();
	const char *c12 = l12.c_str();

	char *args[] = {c1, NULL };
	char **env;

	env = (char**)malloc(sizeof(char *) * 10 + 1);
	env[0] = strdup(l3.c_str());
	env[1] = strdup(l4.c_str());
	env[2] = strdup(l5.c_str());
	env[3] = strdup(l6.c_str());
	env[4] = strdup(l7.c_str());
	env[5] = strdup("PATH_INFO=/www/info.php");
	env[6] = strdup("PATH_TRANSLATED=/mnt/nfs/homes/ykuo/project/github/42-webserv/www/info.php");
	env[7] = strdup(l10.c_str());
	env[8] = strdup(l11.c_str());
	env[9] = strdup(l12.c_str());
	env[10] = NULL;

	int pipefd[2];
	pipe(pipefd);

	pid_t pid = fork();
	if (pid == 0)
	{
		// Child process: execute the CGI script
		close(pipefd[READEND]);
		dup2(pipefd[WRITEEND], STDOUT_FILENO);
		int ret = execve(args[0], args, env);
		perror("execve() failed");
	} else if (pid > 0)
	{
		// Parent process: read the output of the CGI script
		close(pipefd[WRITEEND]);
		char buffer[1024];
		int nread = read(pipefd[READEND], buffer, sizeof(buffer));
		cout << "exec: " <<  endl;
		if (nread > 0)
		{
			cout << "exec_result: " << endl;
			printf("%.*s", nread, buffer);
		}
		close(pipefd[READEND]);
	}

	return (200);
}
