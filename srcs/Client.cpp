# include "webserv.hpp"
# include <sstream> // pour std::ostringstream
# include <deque>

/******************************************************************************
*                              CONSTRUCTORS                                   *
******************************************************************************/

Client::Client()
{
	//initialize values
}

Client::Client(int connection, Server *server, std::vector<Server*>	serversList) : m_new_socket(connection), _server(server), _serversList(serversList) //needs to find right server with server_name
{
    _domain = _server->getDomain(); //AF_INET, AF_INET6, AF_UNSPEC
	_service = _server->getService(); //SOCK_STREAM, SOCK_DGRAM
	_protocol = _server->getProtocol(); //use 0 for "any"
    _interface = _server->getInterface(); //needs to be set to INADDR_ANY
	_backlog = _server->getBacklog(); //maximum number of queued clients
	_locations = _server->getLocations();
	_port = _server->getPort();
	_host = _server->getHost();
	_serverName = _server->getServerName();
	_clientMaxBodySize = _server->getClientMaxBodySize();
	_root = _server->getRoot();
	_index = _server->getIndex();
	_autoindex = _server->getAutoindex();
	_cgi = _server->getCgi();
	_upload = _server->getUpload();
	_get = _server->getGet();
	_post = _server->getPost();
	_delete = _server->getDelete();
	_errorPages = _server->getErrorPages();
    _return = _server->getReturn();

    _request_is_complete = false;
    _status_code = 0;
	_handle_headers = true;

	_close_connection = false;
}

/******************************************************************************
*                                   COPY                                      *
******************************************************************************/

Client::Client(Client const & copy) : m_new_socket(copy.m_new_socket), _server(copy._server)
{
	_domain = copy._domain; //AF_INET, AF_INET6, AF_UNSPEC
	_service = copy._service; //SOCK_STREAM, SOCK_DGRAM
	_protocol = copy._protocol; //use 0 for "any"
    _interface = copy._interface; //needs to be set to INADDR_ANY
	_backlog = copy._backlog; //maximum number of queued clients
	_locations = copy._locations;
	_port = copy._port;
	_host = copy._host;
	_serverName = copy._serverName;
	_clientMaxBodySize = copy._clientMaxBodySize;
	_root = copy._root;
	_index = copy._index;
	_autoindex = copy._autoindex;
	_cgi = copy._cgi;
	_upload = copy._upload;
	_get = copy._get;
	_post = copy._post;
	_delete = copy._delete;
	_errorPages = copy._errorPages;
    _request_is_complete = copy._request_is_complete;
    _status_code = copy._status_code;
	_handle_headers = copy._handle_headers;
    _return = copy._return;

    _method = copy._method;
    _request_target = copy._request_target;
	_query_string = copy._query_string;
	_body_boundary = copy._body_boundary;
    _path = copy._path;

    _corresponding_location = copy._corresponding_location;
    m_request = copy.m_request;
	_close_connection = copy._close_connection;

	_serversList = copy._serversList;

}

Client	&Client::operator=(Client const & rhs)
{
	if (this != &rhs)
	{
		m_new_socket = rhs.m_new_socket;
		_server = rhs._server;
        _domain = rhs._domain; //AF_INET, AF_INET6, AF_UNSPEC
	    _service = rhs._service; //SOCK_STREAM, SOCK_DGRAM
	    _protocol = rhs._protocol; //use 0 for "any"
        _interface = rhs._interface; //needs to be set to INADDR_ANY
	    _backlog = rhs._backlog; //maximum number of queued clients
	    _locations = rhs._locations;
	    _port = rhs._port;
	    _host = rhs._host;
	    _serverName = rhs._serverName;
	    _clientMaxBodySize = rhs._clientMaxBodySize;
	    _root = rhs._root;
	    _index = rhs._index;
	    _autoindex = rhs._autoindex;
	    _cgi = rhs._cgi;
	    _upload = rhs._upload;
	    _get = rhs._get;
	    _post = rhs._post;
	    _delete = rhs._delete;
	    _errorPages = rhs._errorPages;
        _request_is_complete = rhs._request_is_complete;
        _status_code = rhs._status_code;
	    _handle_headers = rhs._handle_headers;
        _return = rhs._return;

    	_method = rhs._method;
    	_request_target = rhs._request_target;
		_query_string = rhs._query_string;
		_body_boundary = rhs._body_boundary;
    	_path = rhs._path;

    	_corresponding_location = rhs._corresponding_location;
    	m_request = rhs.m_request;
		_close_connection = rhs._close_connection;
		
		_serversList = rhs._serversList;
	}
	return (*this);
}

/******************************************************************************
*                                DESTRUCTOR                                   *
******************************************************************************/

Client::~Client(void) {}

/******************************************************************************
*                                   PRINTERS                                  *
******************************************************************************/

void    Client::general_log(int status)
{
    std::cout << ANSI_BLUE << "client: " << _host << "; status code: " << status << "; request: " << m_request.request_line << "; host:" << m_request.headers["host"] << ANSI_RESET << std::endl;
}

void    Client::error_log(int status)
{
    std::cerr << ANSI_RED << "client: " << _host << "; status code: " << status << "; request: " << m_request.request_line << "; host:" << m_request.headers["host"] << ANSI_RESET << std::endl;
}

void Client::print_headers(const std::map<std::string, std::string>& headers)
{
    std::cout << "Headers :" << std::endl;

    for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it)
    {
        std::cout << it->first << ": " << it->second << std::endl;
    }

    // Vérifier la présence de la clé "Content-Type"
    std::map<std::string, std::string>::const_iterator it = headers.find("Content-Type");
    if (it == headers.end())
    {
        std::cerr << "Erreur : la clé Content-Type n'est pas présente dans les headers." << std::endl;
        return;
    }
    std::cout << "Content-Type: " << it->second << std::endl;
}

/******************************************************************************
*                                 SERVER NAME                                 *
******************************************************************************/

void		Client::select_server_block()
{
	for (std::vector<Server*>::iterator it = _serversList.begin(); it != _serversList.end(); it++)
	{
		std::vector<std::string> vec = (*it)->getServerName();
		for (std::vector<std::string>::iterator jt = vec.begin(); jt != vec.end(); jt++)
		{
			//std::cout << ANSI_RED << *jt << ANSI_RESET << std::endl;
			std::string header = m_request.headers["host"].substr(1, header.size());
			//std::cout << header << std::endl;
			if (header.compare((*jt)) == 0 && _port == (*it)->getPort() && _host.compare((*it)->getHost()) == 0)
			{
				//std::cout << "HERE" << std::endl;
				_server = (*it);
				set_server_data();
				return;
			}
		}
	}
	return;
}

void		Client::set_server_data()
{
	_domain = _server->getDomain(); //AF_INET, AF_INET6, AF_UNSPEC
	_service = _server->getService(); //SOCK_STREAM, SOCK_DGRAM
	_protocol = _server->getProtocol(); //use 0 for "any"
    _interface = _server->getInterface(); //needs to be set to INADDR_ANY
	_backlog = _server->getBacklog(); //maximum number of queued clients
	_locations = _server->getLocations();
	_port = _server->getPort();
	_host = _server->getHost();
	_serverName = _server->getServerName();
	_clientMaxBodySize = _server->getClientMaxBodySize();
	_root = _server->getRoot();
	_index = _server->getIndex();
	_autoindex = _server->getAutoindex();
	_cgi = _server->getCgi();
	_upload = _server->getUpload();
	_get = _server->getGet();
	_post = _server->getPost();
	_delete = _server->getDelete();
	_errorPages = _server->getErrorPages();
    _return = _server->getReturn();
}

/******************************************************************************
*                                 LOCATIONS                                   *
******************************************************************************/

bool Client::check_if_corresponding_location(std::string &request_target) //this function compares each locate with request target
{																			// and selects the closest one
	std::vector<std::string> split_locate;
	std::vector<std::string> split_target;
	const char* c = "/";


	//std::cout << ANSI_RED << "request target: " << request_target << ANSI_RESET << std::endl;
	if (!_server->getLocations().empty())
	{
        std::vector<Location*>::iterator it = _locations.begin();
		int max = 0;

        for (; it != _locations.end(); it++)
        {
			//std::cout << "locate: " << (*it)->getLocate() << std::endl;
			int	nb = 0;
			split_locate = ft_split((*it)->getLocate().c_str(), c);
			split_target = ft_split(request_target.c_str(), c);
			std::vector<std::string>::iterator j = split_locate.begin();
			std::vector<std::string>::iterator k = split_target.begin();
			while (j != split_locate.end() && k != split_target.end() && ((*j).compare((*k)) == 0))
			{
				nb++;
				j++;
				k++;
			}
			if (k == split_target.end() && j != split_locate.end())
				nb = 0;
			if (k != split_target.end() && j != split_locate.end())
				nb = 0;
			if (nb > max)
			{
				//std::cout << "HERE" << std::endl;
				_corresponding_location = (*it);
				max = nb;
			}
            //if ((*it)->getLocate().compare(request_target) == 0)
            //{
            //    _corresponding_location = (*it);
            //    //std::cout << "locate: " << _corresponding_location->getLocate() << std::endl;
            //    set_location_data();
            //    return true;
            //}
        }
		if (max > 0)
		{
			set_location_data();
			return true;
		}
        else
        {
            std::vector<Location*>::iterator it = _locations.begin();
            for (; it != _locations.end(); it++)
            {
                if ((*it)->getLocate().compare("/") == 0)
                {
                    _corresponding_location = (*it);
                    //std::cout << "locate: " << _corresponding_location->getLocate() << std::endl;
                    set_location_data();
                    return true;
                }
            }
        }
	}
	return false;
}

void Client::set_location_data()//this function sets data from corresponding location
{
    _root = _corresponding_location->getRoot();
	_index = _corresponding_location->getIndex();
	_autoindex = _corresponding_location->getAutoindex();
	_cgi = _corresponding_location->getCgi();
	_upload = _corresponding_location->getUpload();
	_get = _corresponding_location->getGet();
	_post = _corresponding_location->getPost();
	_delete = _corresponding_location->getDelete();
	_return = _corresponding_location->getReturn();
}

/******************************************************************************
*                                   REQUEST                                   *
******************************************************************************/

// void Client::getPayload() //I could not use this function, I entered infinite loop 50% of the times
// {
//     std::string request_raw;
//     std::string request_headers;
//     std::string request_body;
//     int content_length = 0;
//     int bytesReceived = 0;

//     // Read the request headers
//     while (true)
//     {
//         int valread = recv(m_new_socket, m_buffer, sizeof(m_buffer), 0);
//         if (valread == -1)
//             General::exitWithError("Error in recv()");

//         request_raw += std::string(m_buffer, valread);
//         if (request_raw.find("\r\n\r\n") != std::string::npos)
//             break; // Found the end of headers
//     }

//     std::string receivedData(m_buffer);
//     size_t headerEndPos = receivedData.find("\r\n\r\n");
//     if (headerEndPos != std::string::npos)
//     {
//         request_headers = receivedData.substr(0, headerEndPos);
//         request_body = receivedData.substr(headerEndPos + 4);
//     } else
//         std::cerr << "Invalid HTTP request: Header and body separation not found." << std::endl;

//     std::cout << "Body: " << "request_body" << std::endl;


//     // Find the Content-Length header
//     std::string content_length_str = "Content-Length: ";
//     std::string::size_type content_length_pos = request_headers.find(content_length_str);
//     if (content_length_pos != std::string::npos)
//     {
//         content_length_pos += content_length_str.length();
//         std::string::size_type end_of_line_pos = request_headers.find("\r\n", content_length_pos);
//         if (end_of_line_pos != std::string::npos)
//         {
//             std::string content_length_value = request_headers.substr(content_length_pos, end_of_line_pos - content_length_pos);
//             content_length = std::atoi(content_length_value.c_str());
//         }
//     }

//     cout << "Content length: " << content_length << endl;
//     cout << "Body length: " << request_body.length() << endl;

//     // Read the request body if content length is specified
//     if (content_length > 0 && request_body.length() != content_length)
//     {
//         request_body.resize(content_length);
//         int flags = fcntl(m_new_socket, F_GETFL, 0);
//         if (fcntl(m_new_socket, F_SETFL, flags | O_NONBLOCK) < 0)
//             General::exitWithError("Fnctl error: failed to set socket to non-blocking mode");
//         while (bytesReceived < content_length)
//         {
//             int bytesRead = recv(m_new_socket, &request_body[bytesReceived], content_length - bytesReceived, 0);
//             if (bytesRead == -1)
//             {
//                 if (errno == EWOULDBLOCK || errno == EAGAIN)
//                     continue;    // No data available yet, continue the loop or handle other tasks
//                 else
//                     General::exitWithError("Error in recv() 2");
//             }
//             else if (bytesRead == 0)
//                 break; // Connection closed, handle appropriately
//             bytesReceived += bytesRead;
//         }
//     }
//     // handle the received payload
//     m_request.raw_request = request_headers + "\r\n\r\n" + request_body;
//     General::log("\nReceived message: \n" + m_request.raw_request);
// }

bool Client::getPayload() //receives all request and puts it in a buffer
{
	int valread = 0;

	valread = recv(m_new_socket, m_buffer, sizeof(m_buffer), 0);

	if (valread == 0) 
    {
		_request_is_complete = true;
		return true ;
	}
	if (valread < 0)
    {
		return false;
	}
	m_buffer[valread] = '\0';
	std::cout << m_buffer << std::endl;
	m_request.raw_request.append(m_buffer, valread);
	return true;
}

bool Client::parse_request() 
{
	if (m_request.raw_request.empty()) // no raw request so returns status code 400 "bad request"
    {
		_request_is_complete = true;
		_status_code = 400;
		//std::cout << "HERE" << std::endl;
		return _request_is_complete;
	}

    //starts parsing
	if (_method.empty()) //first recv --> important to keep this line in case of large file upload
	{
		int body_index = -1;
		std::string raw_header;
		body_index = m_request.raw_request.find("\r\n\r\n"); //finds start of body
		if (body_index < 0) 
    	{
			_status_code = 400;//"bad request"
			return true;
		}
		raw_header = m_request.raw_request.substr(0, body_index + 4);
		m_request.raw_request.erase(0, body_index + 4);
    	//std::cout << m_request.raw_request << std::endl;
		std::deque<std::string>	lines;
		lines = getlines(raw_header); //split headers
		if (!lines.empty()) 
    	{
			try
			{ 
				parse_line(lines, raw_header); // if bad headers, sends error code exception
			}
			catch (int error_code)
			{
				_status_code = error_code;
				return true;
			}
		}
	}
    
	size_t content_len = 0;
	std::stringstream ss;
	ss << m_request.headers["content-length"];
	ss >> content_len;

	if (!m_request.raw_request.empty()) 
    {
				//std::cout << _clientMaxBodySize << std::endl;
				//std::cout << content_len << std::endl;

		//if (_clientMaxBodySize > 0) //Client Max Body Size set to 0 by default
        //{

		//https://linuxhint.com/what-is-client-max-body-size-nginx/
		
		if (m_request.raw_request.size() >= content_len) //checks if whole request has been received,
		{												//if not, returns false and stays EPOLLIN
			if (_clientMaxBodySize > 0 && content_len > _clientMaxBodySize) //request too big
            {
				_status_code = 413;//"payload too large"
				_request_is_complete = true;
				return (_request_is_complete);
			}
			handle_body(m_request.raw_request); //adds body
		}
		//}
	}
	return _request_is_complete;
}

std::deque<std::string> Client::getlines(std::string buf) 
{
	std::deque<std::string> lines;
	_ss << buf;

	std::string::size_type i = buf.find("\n");
	while (!buf.empty()) 
    {
		std::string inter;
		std::getline(_ss, inter);
        std::cout << ANSI_YELLOW << inter << ANSI_RESET << std::endl; //print headers to check everything ok
		lines.push_back(inter);
        //std::cout << inter << std::endl;
		buf.erase(0, i + 1);
		i = buf.find("\n");
		if (i == std::string::npos)
			i = buf.size();
	}
	return lines;
}

void Client::parse_line(std::deque<std::string> &lines, std::string &raw_request)
{
	remove_carriage_return_char(lines); //clear line

	handle_request_line(lines.front());
	lines.pop_front();

	while (!lines.empty() && !_request_is_complete)
    {
		if (_handle_headers)
		{
			handle_field_line(lines.front());
			lines.pop_front();
		}
		else 
        {
			handle_body(m_request.raw_request);
			lines.clear();
		}
	}
}

void Client::handle_request_line(std::string &line)
{
	m_request.request_line = line;
	std::vector<std::string> words = ft_split(line.c_str(), "\t\v\r ");
	std::vector<std::string>::iterator it = words.begin();

	if (*it != "GET" && *it != "POST" && *it != "DELETE") //wrong method for our webserv
		throw 501; //"not implemented"

	_method = *it;

	if (++it == words.end()) //check there is request target
		throw 400; //bad request

	_request_target = *it;
	_query_string =  get_query_string(_request_target); //if query string to delete from request target

	if (++it == words.end())
		throw 400; //"bad request" because http version missing

	if ((*it) != "HTTP/1.1")
		throw 400;//"bad request" because wrong http version
}

void Client::handle_field_line(std::string &line)
{
	std::string					field_name;
	std::vector<std::string>	field_values;

	std::string::size_type i = line.find(":");
	if (i != std::string::npos) 
    {
		field_name.assign(line.begin(), line.begin() + i);
		if (field_name.empty() || field_name_has_whitespace(field_name))
			throw 400; //"bad request"
		str_to_lower(field_name);
		line.erase(0, i + 1);
		m_request.headers[field_name] = line;

		if (field_name == "content-type") 
        {
			std::string::size_type boundary_index = line.find("boundary");
			if (boundary_index != std::string::npos) 
				_body_boundary = line.erase(0, boundary_index + 9);
		}
		return;
	}
	else if (line.empty()) //now check all headers
    {
		if (m_request.headers.count("host") == 0) //no host received
			throw 400; //"bad request"
		_handle_headers = false; //all headers have been received
		if (_method != "POST") //no need for body
			_request_is_complete = true;

		select_server_block();
		check_if_corresponding_location(_request_target); //changes the data for the one in location
		check_method(_method); //checks if method allowed
		if (_method.compare("GET") == 0)
			check_access(_request_target); //is resource requested in target accessible?

		return;
	}
	throw 400; //"wrong header"
}

void Client::handle_body(std::string &raw_request) 
{
	if (_request_is_complete == true || m_request.raw_request.empty())
		return ;

	if (_method != "POST") //if method is not post, no need for body
    {
		_request_is_complete = true;
		return;
	}
	if (m_request.headers.count("transfer-encoding") == 0 && \
		m_request.headers.count("content-length") == 0) // if no transfer-encoding and no content-length, no body
    { 
		_request_is_complete = true;
		return ;
	}
	m_request.body.append(m_request.raw_request); //adds body
	if (upload_file(m_request.raw_request)) //tries to upload file
		_request_is_complete = true;
	return ;
}

/******************************************************************************
*                                     UTILS                                   *
******************************************************************************/

bool	Client::is_method_allowed(std::string method)
{
	//std::cout << "method " << method << std::endl;
	if (method.compare("GET") == 0 && _get)
		return true;
	else if (method.compare("POST") == 0 && _post)
		return true;
	else if (method.compare("DELETE") == 0 && _delete)
		return true;
	return false;
}

void Client::check_method(std::string &method)
{
	if(!_server->is_method_allowed(method))
    {
		_status_code = 405; //"method not allowed"
		_request_is_complete = true;
	}
}

void Client::remove_carriage_return_char(std::deque<std::string> &lines) 
{
	std::deque<std::string>::iterator it;

	for (it = lines.begin(); it != lines.end(); it++)
    {
		if (!(*it).empty() && (*it)[(*it).size() - 1] == '\r') 
			(*it).erase((*it).size() - 1, 1);
	}
}

std::string Client::get_query_string(std::string &request_target)
{
	std::string str;

	std::string::size_type i = request_target.find("?");
	if (i != std::string::npos) 
    {
		str.assign(request_target.begin() + i + 1, request_target.end());
		request_target.erase(i, request_target.size());
	}
	return str;
}

std::string Client::add_root(std::string request_target)
{
	if (request_target[0] != '/')
		request_target = "/" + request_target;

	return _server->getRoot() + request_target;
}

void Client::check_access(std::string request_target) 
{
	_path = add_root(request_target);
	int val = access(_path.c_str(), 0);

	if (val < 0 && (_return.empty())) //if return set, page redirected anyway
    {
		if (errno == ENOENT)
        {
            std::cout << "path" << _path << std::endl;
            error_log(404);
			throw 404; //"not found"
        }
		else
		{
        	error_log(500);
			throw 500; //"internal error server"
		}
	}
}

bool Client::field_name_has_whitespace(std::string &field_name) const 
{
	if (field_name[field_name.size() - 1] == ' '  || 
		field_name[field_name.size() - 1] == '\t' ||
		field_name[field_name.size() - 1] == '\r' ||
		field_name[field_name.size() - 1] == '\f' ||
		field_name[field_name.size() - 1] == '\v')
		return true;
	return false;
}

bool Client::upload_file(std::string &raw_request) //check for bigger file
{
	std::string upload_path = "./www/site/files"; //default upload
	std::string filename;
	std::string file_body;

	if (!_server->getUpload().empty())
		upload_path = _server->getUpload();

	//std::cout << upload_path << std::endl;

	if (access(upload_path.c_str(), X_OK) != 0) 
    {
        error_log(500);
		_status_code = 500;
		_request_is_complete = true;
		return false;
	}

	while (m_request.raw_request.find("filename") != std::string::npos) 
    {
		// check if "filename" exist.
		int filename_index = 0;
		if (m_request.raw_request.find("filename=") != std::string::npos)
        {
			filename_index = m_request.raw_request.find("filename=");
			m_request.raw_request.erase(0, filename_index);
		}

		// get the filename
		int n = 0;
		if (m_request.raw_request.find("\n") != std::string::npos)
        {
			n = m_request.raw_request.find("\n");
			filename = m_request.raw_request.substr(10, n - 12 );
		}

		// remove the chars before the real content
		int content_start_index = 0;
		if (m_request.raw_request.find("\r\n\r\n") != std::string::npos)
        {
			content_start_index = m_request.raw_request.find("\r\n\r\n");
			m_request.raw_request.erase(0, content_start_index + 4);
		}

		// remove the end boundary
		int content_end_index = 0;
		if(m_request.raw_request.find(_body_boundary) != std::string::npos) 
        {
			content_end_index = m_request.raw_request.find(_body_boundary);
			file_body = m_request.raw_request.substr(0, content_end_index - 5);
			m_request.raw_request.erase(0, content_end_index + _body_boundary.size() + 8);
		}

		std::ofstream temp_file((upload_path + "/" + filename).c_str());
		if (!temp_file.is_open()) 
        {
            error_log(500);
			_status_code = 500;
			_request_is_complete = true;
			return false;
		}
		temp_file << file_body;
		temp_file.close();
		if (m_request.raw_request.empty() || \
			m_request.raw_request.find(_body_boundary) == std::string::npos)
        {
			break ;
		}
	}
	//std::cout << "size: " <<  m_request.raw_request.size() << std::endl;
	_request_is_complete = true;
	return true;
}

bool Client::is_whitespace(unsigned char c)
{
    if (c == ' ' || c == '\t' || c == '\n' ||
        c == '\r' || c == '\v' || c == '\f')
        return true;
    return false;
}

//ft_split
int		ft_check_charset(char c, const char *charset)
{
	int i;

	i = 0;
	while (charset[i] != 0)
	{
		if (c == charset[i])
			return (1);
		i++;
	}
	return (0);
}

int		ft_count_words(const char *str, const char *charset)
{
	int len;

	len = 0;
	if (!(ft_check_charset(*str, charset)))
		len++;
	while (*str)
	{
		if (ft_check_charset(*str, charset))
		{
			while (ft_check_charset(*str, charset) && *str != 0)
				str++;
			len++;
		}
		else
			str++;
	}
	if (ft_check_charset(*(str - 1), charset))
		len--;
	return (len);
}

std::vector<std::string> Client::ft_split(const char *str, const char *charset)
{
	int		i;
	int		len;
	std::vector<std::string> tokens;

	while (*str != 0)
	{
		if (ft_check_charset(*str, charset))
			str++;
		else
		{
			len = 0;
			while (!(ft_check_charset(str[len], charset)) && str[len] != 0)
				len++;
			i = 0;
			std::string intermediate;
			while (i < len) {
				intermediate.push_back(*(str++));
				i++;
			}
			tokens.push_back(intermediate);
		}
	}
	return tokens;
}

void str_to_lower(std::string &str)
{
	std::string::iterator it = str.begin();
	for (; it != str.end(); it++)
		*it = std::tolower(*it);
}

void remove(std::string &str, char c)
{
	std::string result;
	for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
	{
		if (*it != c)
		{
			result += *it;
		}
	}
	str = result;
}

/******************************************************************************
*                                   GETTERS                                   *
******************************************************************************/

Server *Client::getServer()
{
    return (_server);
}

std::string Client::getPath()
{
    return (_path);
}

std::string Client::getRequestTarget()
{
    return (_request_target);
}

int Client::getStatusCode()
{
    return (_status_code);
}

std::string Client::getMethod()
{
    return (_method);
}

int Client::getFd()
{
    return(m_new_socket);
}

std::map<std::string, std::string> Client::getHeaders() const 
{
	return m_request.headers;
}

std::string		Client::getBody() const{
	return m_request.body;
}

std::string		Client::getContentType()
{
	std::string result = m_request.headers["content-type"];
	return result;
}

int         Client::getClientMaxBodySize()
{
    	return (_clientMaxBodySize);
}

std::string Client::getRoot()
{
    return(_root);
}

std::string Client::getIndex()
{
    return(_index);
}

bool Client::getAutoindex()
{
    return(_autoindex);
}

std::map<std::string, std::string> Client::getCgi()
{
	return (_cgi);
}

std::string Client::getUpload()
{
    return (_upload);
}

bool Client::getGet()
{
    return(_get);
}

bool Client::getPost()
{
    return(_post);
}

bool Client::getDelete()
{
    return(_delete);
}

std::map<int, std::string>	Client::getErrorPages()
{
    return(_errorPages);
}

std::string Client::getReturn()
{
    return (_return);
}
