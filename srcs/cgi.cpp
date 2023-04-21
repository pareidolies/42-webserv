#include "CGI.hpp"

CGI::CGI(File &file, RequestConfig &config, std::map<std::string, std::string, ft::comp> &req_headers) : 
    _file(file), _config(config), req_headers_(req_headers)
{
	_req_body = _file.getContent();
}

CGI::CGI(File &file, RequestConfig &config, std::map<std::string, std::string, ft::comp> &req_headers, std::string &req_body) : 
    _file(file), _config(config), req_headers_(req_headers)
{
	if (req_body.empty() && _config.getMethod() != "POST")
		_req_body = _file.getContent();
	else
		_req_body = req_body;
}

void CGI::init(int worker_id) {
	char *cwd = getcwd(NULL, 0);
	if (!cwd)
		return ;
	_cwd = cwd;
	free(cwd);

	env_ = NULL;
	_argv[0] = NULL;
	_argv[1] = NULL;
	_argv[2] = NULL;
	_extension = _file.getMimeExtension();
	_cgi_exe = _config.getCGI()[_extension];
	if (_config.getCGIBin()[0] == '/') 
		_cgi_path = _config.getCGIBin() + "/" + _cgi_exe;
	else
		_cgi_path = _cwd + "/" + _config.getCGIBin() + "/" + _cgi_exe;
	std::string cgi_path = "/tmp/webserv_cgi_tmp_" + ft::to_string(worker_id);
	_tmp_file.set_path(cgi_path.c_str());
	_tmp_file.open(true);
	if (worker_id)
		Log.print(DEBUG, "worker[" + ft::to_string(worker_id) + "] : CGI -> " + _cgi_path);
	else
		Log.print(DEBUG, "server : CGI -> " + _cgi_path);
}

CGI::~CGI() {
  free(_argv[0]);
  free(_argv[1]);
  if (env_)
    ft::free_tab(env_);
  _tmp_file.close();
  _tmp_file.unlink();
}

int CGI::execute() {
  _file_path = _cwd + "/" + _file.getPath();

  if (!setCGIEnv())
    return 500;
  if (!(_argv[0] = ft::strdup(_cgi_path.c_str())))
    return 500;
  if (!(_argv[1] = ft::strdup(_file_path.c_str())))
    return 500;
  _argv[2] = NULL;

  int pip[2];

  if (pipe(pip) != 0)
    return 500;

  pid_t pid = fork();

  if (pid == 0) {
    if (chdir(_file_path.substr(0, _file_path.find_last_of('/')).c_str()) == -1)
      return 500;
    close(pip[1]);
    if (dup2(pip[0], 0) == -1)
      return 500;
    if (dup2(_tmp_file.getFd(), 1) == -1)
      return 500;
    close(pip[0]);
    execve(_argv[0], _argv, env_);
    exit(1);
  }
  else if (pid > 0) {
    close(pip[0]);
    if (_req_body.length() && write(pip[1], _req_body.c_str(), _req_body.length()) <= 0)
      return 500;
    close(pip[1]);

    int status;

    if (waitpid(pid, &status, 0) == -1)
      return 500;
    if (WIFEXITED(status) && WEXITSTATUS(status))
      return 502;
  }
  else {
    return 502;
  }

  _body = _tmp_file.getContent();
  return 200;
}

void CGI::parseHeaders(std::map<std::string, std::string> &headers) {
  size_t end, last;
  std::string header;

  while ((end = _body.find("\r\n")) != std::string::npos) {
    if (_body.find("\r\n") == 0) {
      _body.erase(0, end + 2);
      break;
    }
    if ((last = _body.find(':', 0)) != std::string::npos) {
      header = _body.substr(0, last);
      headers[header] = ft::trim_left(_body.substr(last + 1, end - last - 1), ' ');
    }
    _body.erase(0, end + 2);
  }
  if (headers.count("Content-Length")) {
    size_t size = ft::stoi(headers["Content-Length"]);

    _body.erase(size);
  }
}

std::string &CGI::getBody() {
  return _body;
}

bool CGI::setCGIEnv() {
	if (_config.getMethod() == "POST")
	{
		_cgi_env["CONTENT_TYPE"] = req_headers_["Content-Type"];
		_cgi_env["CONTENT_LENGTH"] = ft::to_string(_req_body.length());
	}
	_cgi_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	_cgi_env["PATH_INFO"] = _file_path;
	_cgi_env["PATH_TRANSLATED"] = _file_path;
	_cgi_env["QUERY_STRING"] = _config.getQuery();
	_cgi_env["REMOTE_ADDR"] = _config.getClient().getAddr();

	if (_config.getAuth() != "off")
	{
		_cgi_env["AUTH_TYPE"] = "Basic";
		_cgi_env["REMOTE_IDENT"] = _config.getAuth().substr(0, _config.getAuth().find(':'));
		_cgi_env["REMOTE_USER"] = _config.getAuth().substr(0, _config.getAuth().find(':'));
	}

	_cgi_env["REQUEST_METHOD"] = _config.getMethod();
	_cgi_env["REQUEST_URI"] = _file_path;

	_cgi_env["SCRIPT_NAME"] = _cgi_path;
	_cgi_env["SERVER_NAME"] = _config.getHost();
	_cgi_env["SERVER_PROTOCOL"] = _config.getProtocol();
	_cgi_env["SERVER_PORT"] = ft::to_string(_config.getPort());
	_cgi_env["SERVER_SOFTWARE"] = "WEBSERV/1.0";

	if (_extension == ".php")
		_cgi_env["REDIRECT_STATUS"] = "200";

	for (std::map<std::string, std::string, ft::comp>::iterator it = req_headers_.begin(); it != req_headers_.end(); it++)
	{
		if (!it->second.empty())
		{
			std::string header = "HTTP_" + ft::to_upper(it->first);
			std::replace(header.begin(), header.end(), '-', '_');
			_cgi_env[header] = it->second;
		}
	}

	if (!(env_ = (char **)malloc(sizeof(char *) * (_cgi_env.size() + 1))))
		return false;

	int i = 0;

	for (std::map<std::string, std::string>::iterator it = _cgi_env.begin(); it != _cgi_env.end(); it++)
	{
		std::string tmp = it->first + "=" + it->second;
		if (!(env_[i] = ft::strdup(tmp.c_str())))
			return false;
		i++;
	}
	env_[i] = NULL;
	return true;
}