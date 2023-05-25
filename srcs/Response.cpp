#include "webserv.hpp"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>

/******************************************************************************
*                              CONSTRUCTORS                                   *
******************************************************************************/

Response::Response(Client client) : _client(client)
{
	_server = _client.getServer();
	_server->print_server();
    _status_code = _client.getStatusCode();
	_get = _client.getGet();
	_post = _client.getPost();
	_delete = _client.getDelete();

	//for yang chi - CGI
	_path = _client.getPath();
	std::cout << ANSI_RED << _path << ANSI_RESET << std::endl;
	_method = _client.getMethod();
	_content_type = _client.getContentType();
	_request_body = _client.getBody();
	_query_string = _client.getQueryString();
	_cgi_map = _client.getCgi();

	init_code_msg();
}

/******************************************************************************
*                                   COPY                                      *
******************************************************************************/


/******************************************************************************
*                                DESTRUCTOR                                   *
******************************************************************************/

Response::~Response(void)
{

}

/******************************************************************************
*                                   Getter                                    *
******************************************************************************/

std::string		Response::get_method()
{
	return _method;
}

std::string		Response::get_path()
{
	return _path;
}

std::string		Response::get_content_type()
{
	return _content_type;
}

std::string		Response::get_request_body()
{
	return _request_body;
}

Server 			*Response::get_server()
{
	return _server;
}

std::string		Response::get_extension()
{
	return _extension;
}

std::map<std::string, std::string> Response::get_cgi_map()
{
	return _cgi_map;
}

std::string      Response::get_query_string()
{
	return( _query_string);
}

/******************************************************************************
*                                 RESPONSE                                    *
******************************************************************************/

std::string getFileType(const std::string& filePath) {
    std::size_t dotPosition = filePath.rfind(".");
    
    if (dotPosition != std::string::npos && dotPosition < filePath.length() - 1)
	{
        std::string fileType = filePath.substr(dotPosition);
        return fileType;
    }
    return "";
}

//add cgi management here //yangchi

bool Response::cgi_execution()
{
	std::string response_header;
	std::string response;

	CGI cgi(this);
	cgi.execute();
	_status_code = cgi.getStatusCode();

	size_t breaker = cgi.getBody().find("\r\n\r\n");
	if (breaker != std::string::npos) {
		response_header = cgi.getBody().substr(0, breaker);
		this->_body = cgi.getBody().substr(breaker + 4);
	}
	response = "HTTP/1.1 ";
	response += General::to_string(_status_code);
	response += " ";
	response += get_code_msg();
	response += "\r\n";
	response += "Content-Length:" + General::to_string(_body.size()) + "\n";
	response += response_header;
	response += "\r\n\r\n";
	response += _body;
	if (send(_client.getFd(), response.c_str(), response.size(), 0) < 0)
		_client.error_log(500);
	_client.general_log(_status_code);
	return true;
}

bool Response::send_response()
{
	if (_status_code) //error found in client
	{
		send_error_response();
		return true;
	}
	this->_extension = getFileType(_path);
	std::map<std::string, std::string>::iterator iterator = _cgi_map.find(this->_extension);
	if (iterator != _cgi_map.end())
		return cgi_execution();
	else
	{
		if (_method == "GET")
		{
			if (!_client.getReturn().empty() && _client.getRequestTarget() != _client.getReturn()) //redirection needed
				_status_code = 307; //"temporary redirect"
			else
				get_body();
			get_header_fields(_body.size());
		}
		else if (_method == "DELETE" && _delete)
			delete_file();
		else if (_method == "POST")
		{
			if (_status_code < 300)
			{
				_status_code = 201;
				return post_body();
			}
		}
	}
	if (_status_code >= 308)
		return send_error_response();
	return send_successful_response();
}

/******************************************************************************
*                                 GET                                         *
******************************************************************************/

void Response::get_header_fields(int content_len)
{
	std::map<std::string, std::string>	headers;
	
	headers["Content-Length"] = General::to_string(content_len);
 	if (_status_code == 405) //"method is not allowed"
	{
		std::string tmp;
        if (_get)
			tmp = "GET, ";
		if	(_post)
			tmp += "POST, ";
		if (_delete)
			tmp += "DELETE, ";
		tmp = tmp.substr(0, tmp.size() - 2);
		headers["Allow"] = tmp;
	}
	headers["Content-Type"] = search_content_type(_client.getRequestTarget()); //note: the content type will be different if cgi

	if (_status_code >= 300 && _status_code < 400) //is directory so needs to be either redirected or set to index
	{
		if (!_client.getReturn().empty())
		{
			headers["Location"] = _client.getReturn();
			//std::cout << _client.getReturn() << std::endl;
			//std::cout << "return" << std::endl;
		}
		else
		{
			headers["Location"] = _client.getIndex();
			//std::cout << "not return" << std::endl;
		}
	}
	for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); ++it)// adds all headers
	{
		_header_fields += it->first;
		_header_fields += ": ";
		_header_fields += it->second;
		_header_fields += "\r\n";
	}
}

void Response::get_body()
{
	struct stat s;

    //std::cout << "*PATH* : " << _path << stat(_path.c_str(), &s) << std::endl;

	if (stat(_path.c_str(), &s) == 0)
	{
		if (s.st_mode & S_IFREG) //is a regular file
			_body = get_file_content(_path);
		else if (s.st_mode & S_IFDIR) //is a directory
		{ 
			if (_client.getAutoindex() == true) 
				get_autoindex_body(); //autoindex
			else
				_status_code = 307; //temporary redirect
		}
	}
	else
	{
		_status_code = 500; //"internal server error"
		_client.error_log(500);
	}
}

std::string Response::get_file_content(std::string content)
{
	std::ifstream input_file(content.c_str());
	if (!input_file.is_open())
	{
		_status_code = 500; //"internal server error"
        _client.error_log(500);
		return std::string();
	}
	_status_code = 200; //"OK"
	std::string res = std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
	input_file.close();
	return res;
}

// https://www.keycdn.com/support/nginx-directory-index

void Response::get_autoindex_body()
{
	DIR *dir;
	struct dirent *entry;
	std::vector<std::string> files;

	if (((dir = opendir(_path.c_str())) != NULL))
	{
		while ((entry = readdir(dir)) != NULL)
		{
			if (std::string(entry->d_name) != "." && std::string(entry->d_name) != "..")
			{
				files.push_back(std::string(entry->d_name));
			}
		}
		closedir(dir);
	}
	else
	{
		_status_code = 500;
        _client.error_log(500);
		return;
	}
	_body = "<!DOCTYPE html><html><body>\n"
			"<style>"
			" body {background-color: #FFFFFF; font-family: Arial, sans-serif;}"
			"div {border: solid 1px black; box-sizing: border-box; width: auto;  height: auto; margin: 0; background-color: #020024 }"
			"a {display: block; padding: 5px 10px; color: #FFFFFF;}"
			"</style>\n<div>\n";
	for (std::vector<std::string>::iterator it = files.begin(); it != files.end(); ++it)
	{
		_body += "<a href=\"";
		_body += _client.getRequestTarget();
		if (_client.getRequestTarget() != "/")
			_body += "/";
		_body += (*it);
		_body += "\">";
		_body += (*it);
		_body += "</a>";
	}
	_body += "</div></body></html>";
	_status_code = 200; //"OK"
}

/******************************************************************************
*                                 POST                                         *
******************************************************************************/

bool Response::post_body()
{
	std::string response;

	_status_code = 201;
	_body = "<!DOCTYPE html><html><body><span style=\"color:#0e5f07\"> <span style=\"font-family:\'Arial\'\"><h1>File successfully saved and/or form data received</h1><p>";
	if (_client.getHeaders()["content-type"].find("multipart") == std::string::npos)
		_body += _client.getBody();
	_body += "</p></body></html>";
	response = "HTTP/1.1 ";
	response += General::to_string(_status_code);
	response += " ";
	response += get_code_msg();
	response += "\r\n";
	response += "Content-Length:" + General::to_string(_body.size()) + "\n";
	response += "Content-Type:text/html; charset=utf-8 \n";
	response += "\r\n";
	response += _body;
	if (send(_client.getFd(), response.c_str(), response.size(), 0) <= 0)
	{
		_client.error_log(500);
	}
    _client.general_log(_status_code); //printing client data everything OK
	return true;
}

/******************************************************************************
*                                 DELETE                                         *
******************************************************************************/

void Response::delete_file()
{
	//_path = _client.getRoot() + _client.getRequestTarget();

	struct stat sb;
	if (stat(_path.c_str(), &sb) == -1)
	{
		_status_code = 500;
        _client.error_log(500);
	}
	else if (sb.st_mode & S_IFDIR)
		_status_code = 403;//"forbidden"
	else if (sb.st_mode & S_IFREG)
	{
		if (remove(_path.c_str()) != 0)
		{
			_status_code = 500;
            _client.error_log(500);
		}
		else
		{
			_status_code = 200; //"OK"
			_body = "<!DOCTYPE html><html lang=\"en\"><head><title>";
			_body += "DELETED";
			_body += "</title></head><body><center><h1><span style=\"color:#0e5f07\"> <span style=\"font-family:\'Arial\'\">";
			_body += "File successfully deleted";
			_body += "</span></h1></center></body></html>";
		}
	}
}

/******************************************************************************
*                             SUCCESS                                         *
******************************************************************************/

bool Response::send_successful_response()
{
	std::string response;

	response = "HTTP/1.1 ";
	response += General::to_string(_status_code);
	response += " ";
	response += get_code_msg();
	response += "\r\n";
	response += _header_fields;
	response += "\r\n";
	response += _body;

	//std::cout << response << std::endl;

	if (send(_client.getFd(), response.c_str(), response.size(), 0) < 0)
	{
        _client.error_log(500);
	}
	_client.general_log(_status_code);
	return true;
}

/******************************************************************************
*                                   ERROR                                     *
******************************************************************************/

bool Response::send_error_response()
{
    //std::cout << "SEGFAUL1" << std::endl;
	std::string response;
	_body.clear();
    //std::cout << "SEGFAULT2" << std::endl;
	response = "HTTP/1.1 ";
	response += General::to_string(_status_code);
    //std::cout << "SEGFAULT3" << std::endl;
	response += " ";
    //std::cout << "SEGFAULT4" << std::endl;
	response += get_code_msg();
	response += "\r\n";
    //std::cout << "SEGFAULT5" << std::endl;
	if (set_defined_error_page() == false) //sees if there is an error page defined in config
		set_default_error_page();//otherwise creates a default error page
    //std::cout << "SEGFAULT6" << std::endl;
	response += _header_fields;
	response += "\r\n";
    //std::cout << "SEGFAULT7" << std::endl;
	response += _body;

    //std::cout << "SEGFAULT?" << std::endl;

	if (send(_client.getFd(), response.data(), response.size(), 0) < 0)
	{
		_client.error_log(500);
	}
    //std::cout << "SEGFAULT?" << std::endl;
	_client.error_log(_status_code);
	return true;
}

bool Response::set_defined_error_page()
{
	//std::cout << _status_code << std::endl;
	//_server->print_server();
	if (!_server->getErrorPage(_status_code).empty())
	{
        //std::cout << "SEGFAULT8" << std::endl;
		_path = _server->getErrorPage(_status_code);
        //std::cout << "SEGFAULT9" << std::endl;

		std::ifstream myfile(_path.c_str());
		if (myfile.is_open())
		{
			std::string myline;
			while (myfile)
			{
				std::getline(myfile, myline);
				_body += myline;
			}
			myfile.close();
			return true;
		}
		_client.error_log(500);
	}
	return false;
}

void Response::set_default_error_page()
{
	_body = "<!DOCTYPE html><html lang=\"en\"><head><title>";
	_body += General::to_string(_status_code) + " " + get_code_msg();
	_body += "</title></head><body><span style=\"color:#980606\"> <span style=\"font-family:\'Arial\'\"><h1>";
	_body += "<" + General::to_string(_status_code) + "> " + get_code_msg();
	_body += "</h1></span></body></html>";
}

/******************************************************************************
*                                   UTILS                                     *
******************************************************************************/

std::string Response::get_code_msg()
{
	if (_status_code_list.count(_status_code))
		return _status_code_list[_status_code];
	return std::string();
}

void Response::init_code_msg()
{
	_status_code_list[200] = "OK"; //used
	_status_code_list[201] = "Created"; //used
	_status_code_list[202] = "Accepted";//?
	_status_code_list[203] = "Non-Authoritative Information";
	_status_code_list[204] = "No Content";
	_status_code_list[205] = "Reset Content";
	_status_code_list[206] = "Partial Content";
	_status_code_list[207] = "Multi-Status";
	_status_code_list[208] = "Already Reported";
	_status_code_list[226] = "IM Used";
	_status_code_list[300] = "Multiple Choices";
	_status_code_list[301] = "Moved Permanently";
	_status_code_list[302] = "Found";
	_status_code_list[303] = "See Other";
	_status_code_list[304] = "Not Modified";
	_status_code_list[305] = "Use Proxy";
	_status_code_list[306] = "Switch Proxy";
	_status_code_list[307] = "Temporary Redirect"; //used
	_status_code_list[308] = "Permanent Redirect";
	_status_code_list[400] = "Bad Request";//used
	_status_code_list[401] = "Unauthorized";
	_status_code_list[402] = "Payment Required";
	_status_code_list[403] = "Forbidden";//used
	_status_code_list[404] = "Not Found";//used
	_status_code_list[405] = "Method Not Allowed"; //used
	_status_code_list[406] = "Not Acceptable";//add this one
	_status_code_list[407] = "Proxy Authentication Required";
	_status_code_list[408] = "Request Timeout";
	_status_code_list[409] = "Conflict";
	_status_code_list[410] = "Gone";
	_status_code_list[411] = "Length Required";
	_status_code_list[412] = "Precondition Failed";
	_status_code_list[413] = "Payload Too Large";
	_status_code_list[414] = "URI Too Long";
	_status_code_list[415] = "Unsupported Media Type";
	_status_code_list[416] = "Range Not Satisfiable";
	_status_code_list[417] = "Expectation Failed";
	_status_code_list[418] = "I\'m a teapot";
	_status_code_list[421] = "Misdirected Request";
	_status_code_list[422] = "Unprocessable Entity";
	_status_code_list[423] = "Locked";
	_status_code_list[424] = "Failed Dependency";
	_status_code_list[425] = "Too Early";
	_status_code_list[426] = "Upgrade Required";
	_status_code_list[428] = "Precondition Required";
	_status_code_list[429] = "Too Many Requests";
	_status_code_list[431] = "Request Header Fields Too Large";
	_status_code_list[451] = "Unavailable For Legal Reasons";
	_status_code_list[500] = "Internal Server Error"; //used
	_status_code_list[501] = "Not Implemented"; //used
	_status_code_list[502] = "Bad Gateway";
	_status_code_list[503] = "Service Unavailable";
	_status_code_list[504] = "Gateway Timeout";
	_status_code_list[505] = "HTTP Version Not Supported";
	_status_code_list[506] = "Variant Also Negotiates";
	_status_code_list[507] = "Insufficient Storage";
	_status_code_list[508] = "Loop Detected";
	_status_code_list[510] = "Not Extended";
	_status_code_list[511] = "Network Authentication Required";
}

std::string Response::search_content_type(std::string filename)
{
    std::string content_type = "text/html";
    int i = filename.length();
    
    if (i >= 5)
    {
        // TEXT TYPE
        if (filename.substr(i - 4, 4) == ".css" )
           content_type = "text/css";
        else if (filename.substr(i - 4, 4) == ".csv")
            content_type = "text/csv";
        else if (filename.substr(i - 5, 5) == ".html")
            content_type = "text/html";
        else if (filename.substr(i - 4, 4) == ".xml")
            content_type = "text/xml";
        else if (filename.substr(i - 5, 5) == ".scss")
            content_type = "text/css";
        else if (filename.substr(i - 4, 4) == ".txt")
            content_type = "text/plain";

        // IMAGE TYPE
        if (filename.substr(i - 4, 4) == ".svg")
            content_type = "image/svg+xml";
        else if (filename.substr(i - 4, 4) == ".gif")
            content_type = "image/gif";
        else if (filename.substr(i - 5, 5) == ".jpeg")
            content_type = "image/jpeg";
        else if (filename.substr(i - 4, 4) == ".png")
            content_type = "image/webp";                 // show png as a webp
        else if (filename.substr(i - 5, 5) == ".tiff")
            content_type = "image/tiff";
        else if (filename.substr(i - 4, 4) == ".webp")
            content_type = "image/webp";
        else if (filename.substr(i - 4, 4) == ".ico")
            content_type = "image/x-icon";
        else if (filename.substr(i - 4, 4) == ".bmp")
            content_type = "image/bmp";
        else if (filename.substr(i - 4, 4) == ".jpg")
            content_type = "image/jpeg";
        else if (filename.substr(i - 4, 4) == ".jpe")
            content_type = "image/jpeg";
        else if (filename.substr(i - 4, 4) == ".jif")
            content_type = "image/jif";
        else if (filename.substr(i - 4, 4) == ".jfif")
            content_type = "image/jfif";
        else if (filename.substr(i - 4, 4) == ".jfi")
            content_type = "image/jfi";
        else if (filename.substr(i - 4, 4) == ".jpx")
            content_type = "image/jpx";
        else if (filename.substr(i - 4, 4) == ".jp2")
            content_type = "image/jp2";
        else if (filename.substr(i - 4, 4) == ".j2k")
            content_type = "image/j2k";
        else if (filename.substr(i - 4, 4) == ".j2c")
            content_type = "image/j2c";
        else if (filename.substr(i - 4, 4) == ".jpc")
            content_type = "image/jpc";
        
        // AUDIO TYPE
        if (filename.substr(i - 4, 4) == ".mp3")
            content_type = "audio/mpeg";
        else if (filename.substr(i - 4, 4) == ".wav")
            content_type = "audio/wav";
        else if (filename.substr(i - 5, 5) == ".flac")
            content_type = "audio/flac";
        else if (filename.substr(i - 4, 4) == ".aac")
            content_type = "audio/aac";
        else if (filename.substr(i - 4, 4) == ".ogg")
            content_type = "audio/ogg";
        else if (filename.substr(i - 4, 4) == ".oga")
            content_type = "audio/oga";
        else if (filename.substr(i - 4, 4) == ".m4a")
            content_type = "audio/m4a";
        else if (filename.substr(i - 4, 4) == ".m4b")
            content_type = "audio/m4b";
        else if (filename.substr(i - 4, 4) == ".m4p")
            content_type = "audio/m4p";
        else if (filename.substr(i - 4, 4) == ".m4r")
            content_type = "audio/m4r";
        else if (filename.substr(i - 4, 4) == ".m4v")
            content_type = "audio/m4v";
        else if (filename.substr(i - 4, 4) == ".m4s")
            content_type = "audio/m4s";
        else if (filename.substr(i - 4, 4) == ".m4a")
            content_type = "audio/m4a";
        
        // VIDEO TYPE
        if (filename.substr(i - 4, 4) == ".mp4")
            content_type = "video/mp4";
        else if (filename.substr(i - 4, 4) == ".m4v")
            content_type = "video/m4v";
        else if (filename.substr(i - 4, 4) == ".m4p")
            content_type = "video/m4p";
        else if (filename.substr(i - 4, 4) == ".m4b")
            content_type = "video/m4b";
        else if (filename.substr(i - 4, 4) == ".m4r")
            content_type = "video/m4r";
        else if (filename.substr(i - 4, 4) == ".m4s")
            content_type = "video/m4s";
        else if (filename.substr(i - 4, 4) == ".m4a")
            content_type = "video/m4a";
        else if (filename.substr(i - 4, 4) == ".m4v")
            content_type = "video/m4v";
        else if (filename.substr(i - 4, 4) == ".m4p")
            content_type = "video/m4p";
        else if (filename.substr(i - 4, 4) == ".m4b")
            content_type = "video/m4b";
        else if (filename.substr(i - 4, 4) == ".m4r")
            content_type = "video/m4r";
        else if (filename.substr(i - 4, 4) == ".m4s")
            content_type = "video/m4s";

        // APPLICATION TYPE
        
        if (filename.substr(i - 4, 4) == ".pdf")
            content_type = "application/pdf";
        else if (filename.substr(i - 4, 4) == ".doc")
            content_type = "application/msword";
        else if (filename.substr(i - 4, 4) == ".docx")
            content_type = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
        else if (filename.substr(i - 4, 4) == ".xls")
            content_type = "application/vnd.ms-excel";
        else if (filename.substr(i - 4, 4) == ".xlsx")
            content_type = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
        else if (filename.substr(i - 4, 4) == ".ppt")
            content_type = "application/vnd.ms-powerpoint";
        else if (filename.substr(i - 4, 4) == ".pptx")
            content_type = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
        else if (filename.substr(i - 4, 4) == ".pps")
            content_type = "application/vnd.ms-powerpoint";
        else if (filename.substr(i - 4, 4) == ".ppsx")
            content_type = "application/vnd.openxmlformats-officedocument.presentationml.slideshow";
        else if (filename.substr(i - 4, 4) == ".odt")
            content_type = "application/vnd.oasis.opendocument.text";
        else if (filename.substr(i - 4, 4) == ".odp")
            content_type = "application/vnd.oasis.opendocument.presentation";
    }
    return (content_type);
}
