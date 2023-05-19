# include "webserv.hpp"
# include <sstream> // pour std::ostringstream
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sdesseau <sdesseau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/02 18:44:28 by sdesseau          #+#    #+#             */
/*   Updated: 2023/05/17 19:12:49 by sdesseau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/******************************************************************************
*                              CONSTRUCTORS                                   *
******************************************************************************/

Client::Client()
{
	//initialize values
}

Client::Client(int connection, Server *server) : m_new_socket(connection), _server(server)
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
	_cgiFileExtension = _server->getCgiFileExtension();
	_cgiPathToScript = _server->getCgiPathToScript();
	_upload = _server->getUpload();
	_get = _server->getGet();
	_post = _server->getPost();
	_delete = _server->getDelete();
	_errorPages = _server->getErrorPages();

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
	_cgiFileExtension = copy._cgiFileExtension;
	_cgiPathToScript = copy._cgiPathToScript;
	_upload = copy._upload;
	_get = copy._get;
	_post = copy._post;
	_delete = copy._delete;
	_errorPages = copy._errorPages;

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
	    _cgiFileExtension = rhs._cgiFileExtension;
	    _cgiPathToScript = rhs._cgiPathToScript;
	    _upload = rhs._upload;
	    _get = rhs._get;
	    _post = rhs._post;
	    _delete = rhs._delete;
	    _errorPages = rhs._errorPages;
	}
	return (*this);
}

Server *Client::getServer()
{
    return (this->_server);
}

/******************************************************************************
*                                DESTRUCTOR                                   *
******************************************************************************/

Client::~Client(void) {}

/******************************************************************************
*                             MEMBER FUNCTIONS                                *
******************************************************************************/

void Client::getPayload()
{
    std::string request_headers;
    std::string request_body;
    int content_length = 0;
    int bytesReceived = 0;

    // Read the request headers
    while (true)
    {
        int valread = recv(m_new_socket, m_buffer, sizeof(m_buffer), 0);
        if (valread == -1)
            General::exitWithError("Error in recv()");

        request_headers += std::string(m_buffer, valread);
        if (request_headers.find("\r\n\r\n") != std::string::npos)
            break; // Found the end of headers
    }

    // Find the Content-Length header
    std::string content_length_str = "Content-Length: ";
    std::string::size_type content_length_pos = request_headers.find(content_length_str);
    if (content_length_pos != std::string::npos)
    {
        content_length_pos += content_length_str.length();
        std::string::size_type end_of_line_pos = request_headers.find("\r\n", content_length_pos);
        if (end_of_line_pos != std::string::npos)
        {
            std::string content_length_value = request_headers.substr(content_length_pos, end_of_line_pos - content_length_pos);
            content_length = std::atoi(content_length_value.c_str());
        }
    }

    // Read the request body if content length is specified
    if (content_length > 0)
    {
        request_body.resize(content_length);
        int flags = fcntl(m_new_socket, F_GETFL, 0);
        if (fcntl(m_new_socket, F_SETFL, flags | O_NONBLOCK) < 0)
            General::exitWithError("Fnctl error: failed to set socket to non-blocking mode");
        while (bytesReceived < content_length)
        {
            int bytesRead = recv(m_new_socket, &request_body[bytesReceived], content_length - bytesReceived, 0);
            if (bytesRead == -1)
            {
                if (errno == EWOULDBLOCK || errno == EAGAIN)
                    continue;    // No data available yet, continue the loop or handle other tasks
                else
                    General::exitWithError("Error in recv() 2");
            }
            else if (bytesRead == 0)
                break; // Connection closed, handle appropriately
            bytesReceived += bytesRead;
        }
    }
    // Process the received payload
    m_request.raw_request = request_headers + request_body;
    General::log("\nReceived message: \n" + m_request.raw_request);
}

bool Client::parse_request() {
    std::stringstream request_stream;                                                                                   
    request_stream << m_buffer;                                                                      
    std::string line;                                                                               
    bool headers_done = false;   
    // std::cout << "BUFFER :::: "<< request_stream.str() << "END BUFFER          " << std::endl;  
    if (std::getline(request_stream, line) && !line.empty())
    {
        m_request.method = line.substr(0, line.find(" "));
                m_request.uri = line.substr(line.find(" ") + 1, line.rfind(" ") - line.find(" ") - 1); 
    }                                                                 
    while (std::getline(request_stream, line) && !line.empty()) {                 
        // std::cout << "LINE = " << line << std::endl;
            if (line.find(":") != std::string::npos) {
                // std::cout << "rentre dans find" << std::endl;
                std::string header_name = line.substr(0, line.find(":"));                           
                std::string header_value = line.substr(line.find(":") + 1);                         
                m_request.headers[header_name] = header_value;                             
            }         
    }
    // std::cout << "URI : " << m_request.uri << ", METHOD : " << m_request.method << std::endl;
    // print_headers(m_request.headers);
    return (true);                                                                                  
}

/******************************************************************************
*                                     REQUEST                                *
******************************************************************************/

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

bool delete_file(const std::string& filename)
{
    // Votre code pour supprimer le fichier ici
    // Assurez-vous d'adapter cette fonction en fonction de votre système d'exploitation et de votre structure de fichiers

    int result = std::remove(filename.c_str());

    if (result == 0)
    {
        std::cout << "File deleted: " << filename << std::endl;
        return true; // Suppression réussie
    }
    else
    {
        std::perror("Error deleting file");
        return false; // Erreur lors de la suppression du fichier
    }
}

std::string Client::process_request() 
{
    cout << this->getServer()->getHost() << ":" << this->getServer()->getPort() << endl;

    std::cout << "Method: " << m_request.method << std::endl;
    init_code_msg();
    if (m_request.method == "GET")
    {
        // Traitement de la requête GET
        if (m_request.uri == "/")
        {
            m_response.body = read_file("www/site/pages/index.html");
            m_response.body_size = to_string_custom(m_response.body.size());
            m_response.full_response = "HTTP/1.1 200 OK\r\nContent-Type:text/html\r\nContent-Length: " + m_response.body_size + "\r\n\r\n" + m_response.body;
        }
        else
        {
            std::cout << "URI: " << m_request.uri << std::endl;
            // Si l'URI est différent de "/", renvoyer le fichier correspondant
            std::string filename = m_request.uri.substr(1); // Supprimer le premier caractère "/"
            m_response.body = read_file(filename);
            if (m_response.body == "") {
                // Si le fichier n'existe pas, renvoyer une réponse 404
                m_response.content_type = "text/html";
				m_response.body = read_file("www/site/errorPages/404.html");
                m_response.body_size = to_string_custom(m_response.body.size());
                m_response.full_response = "HTTP/1.1 200 OK\r\nContent-Type:";
                m_response.full_response += m_response.content_type;
                m_response.full_response += "\r\nContent-Length: " + m_response.body_size + "\r\n\r\n" + m_response.body;
            }
            else
            {
                m_response.content_type = search_content_type(filename);
                m_response.body_size = to_string_custom(m_response.body.size());
                m_response.full_response = "HTTP/1.1 200 OK\r\nContent-Type:";
                m_response.full_response += m_response.content_type;
                m_response.full_response += "\r\nContent-Length: " + m_response.body_size + "\r\n\r\n" + m_response.body;
            }
        }
    }
    else if (m_request.method == "DELETE")
    {
        // Traitement de la requête DELETE
        // Vérifier les autorisations
        bool authorized = _delete;
        std::cout << "authorized :: " << _delete << std::endl;
        if (authorized)
        {
            // Supprimer le fichier ou la ressource spécifiée dans l'URI
            std::string filename = m_request.uri.substr(1); // Supprimer le premier caractère "/"
            bool deleted = delete_file(filename);
            if (deleted)
            {
                // Le fichier a été supprimé avec succès
                m_response.full_response = "HTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n";
            }
            else
            {
                // Erreur lors de la suppression du fichier
                m_response.full_response = "HTTP/1.1 500 Internal Server Error\r\nContent-Length: 0\r\n\r\n";
            }
        }
        else
        {
            // Autorisation refusée
            m_response.full_response = "HTTP/1.1 403 Forbidden\r\nContent-Length: 0\r\n\r\n";
        }
    }
    else if (m_request.method == "POST")
    {
        // Traitement de la requête POST
        if (m_request.uri == "/upload")
        {
            // Extraire les données du corps de la requête
            std::string boundary = get_boundary(m_request.raw_request);
            m_request.body = find_body(m_request.raw_request, boundary);
            std::string filename = get_filename(m_request.body);
            removeFirstFourLines(m_request.body);

            // Stocker le fichier dans un dossier spécifique
            save_file("www/site/files/downloads/" + filename, m_request.body);

            // Envoyer une réponse au client
            m_response.body = read_file("www/site/pages/upload_complete.html");
            m_response.body_size = to_string_custom(m_response.body.size());
            m_response.full_response = "HTTP/1.1 200 OK\r\nContent-Type:text/html\r\nContent-Length: " + m_response.body_size + "\r\n\r\n" + m_response.body;
        }
        else if (m_request.uri == "/form" || m_request.uri == "/comment")
        {
            m_response.body = read_file("www/site/pages/upload_complete.html");
            m_response.body_size = to_string_custom(m_response.body.size());
            m_response.full_response = "HTTP/1.1 200 OK\r\nContent-Type:text/html\r\nContent-Length: " + m_response.body_size + "\r\n\r\n" + m_response.body;
        }
        else
        {
            // Si l'URI n'est pas "/upload", renvoyer une réponse 404
            m_response.full_response = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
        }
    }
    else
    {
        // Requête non prise en charge
        m_response.full_response = "HTTP/1.1 501 Not Implemented\r\nContent-Length: 0\r\n\r\n";
    }
    return (m_response.full_response);
}

/******************************************************************************
*                                  RESPONSE                                   *
******************************************************************************/

std::string Client::get_boundary(const std::string& request)
{
    // Chercher la clé "Content-Type" dans la requête
    size_t pos = request.find("Content-Type: multipart/form-data;");
    if (pos != std::string::npos)
    {
        // Extraire la valeur du paramètre "boundary"
        pos = request.find("boundary=", pos);
        if (pos != std::string::npos)
        {
            pos += 9;
            size_t end_pos = request.find("\r\n", pos);
            std::string boundary = request.substr(pos, end_pos - pos);
            // std::cout << "BOUNDARY = " << boundary << std::endl;
            return ("--" + boundary);
        }
    }
    return ("");
}

std::string Client::get_filename(const std::string& content)
{
    std::string filename;

    // Recherche de la ligne "Content-Disposition" qui contient le nom de fichier
    std::string disposition = "Content-Disposition: form-data; name=\"file\"; filename=\"";
    std::size_t dispositionStart = content.find(disposition);
    if (dispositionStart != std::string::npos) {
        // Recherche du début et de la fin du nom de fichier dans la ligne
        std::size_t filenameStart = dispositionStart + disposition.length();
        std::size_t filenameEnd = content.find("\"", filenameStart);
        if (filenameEnd != std::string::npos) {
            // Extrait le nom de fichier de la ligne
            filename = content.substr(filenameStart, filenameEnd - filenameStart);
        }
    }
    return (filename);
}

void Client::save_file(const std::string& path, const std::string& content)
{
    std::ofstream file(path.c_str());
    if (file.is_open())
    {
        file << content;
        file.close();
    }
}

template <typename T>
std::string to_string_custom(const T& value)
{
    std::ostringstream os;
    os << value;
    return (os.str());
}

std::string read_file(const std::string& filename) {
    std::ifstream file(filename.c_str());
    if (!file.is_open()) {
        return ("");
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return (buffer.str());
}

std::string find_body(const std::string& request, const std::string& boundary)
{
    std::string data;

    // Find the starting position of the first boundary
    std::string::size_type start = request.find(boundary);
    if (start == std::string::npos) {
        // Boundary not found
        return (data);
    }

    // Find the ending position of the first boundary
    std::string::size_type end = request.find(boundary, start + boundary.length());
    if (end == std::string::npos) {
        // Boundary not found
        return (data);
    }

    // Extract the data between the boundaries
    data = request.substr(start + boundary.length(), end - (start + boundary.length()));

    return (data);
}

void removeFirstFourLines(std::string& data)
{
    // Find the position of the fourth occurrence of "\r\n"
    std::string::size_type lineStart = 0;
    for (int i = 0; i < 4; ++i) {
        lineStart = data.find("\r\n", lineStart);
        if (lineStart == std::string::npos) {
            // Not enough lines to skip
            return ;
        }
        lineStart += 2; // Move past the current "\r\n"
    }
    // Erase the first 4 lines
    if (lineStart != std::string::npos) {
        data.erase(0, lineStart);
    }
}

std::string get_file_extension(const std::string& filename)
{
    size_t dotIndex = filename.find_last_of(".");
    if (dotIndex != std::string::npos && dotIndex < filename.length() - 1)
    {
        return filename.substr(dotIndex);
    }
    return "";
}

bool is_cgi_script(const std::string& uri)
{
    // Ajoutez ici la logique spécifique pour déterminer si l'URI correspond à un script CGI.
    // Par exemple, vous pouvez vérifier si l'extension du fichier est associée à des scripts CGI, tels que .cgi ou .pl.
    size_t queryIndex = uri.find("?");
    if (queryIndex != std::string::npos)
    {
    std::string extension = get_file_extension(uri);

    // Vérifie si l'extension du fichier est dans la liste des extensions CGI
        if (extension == ".cgi" || extension == ".pl")
        {
            return true;
        }
    }
    return false;
}

void Client::init_code_msg()
{
	m_response.status_code_list[200] = "OK";
	m_response.status_code_list[201] = "Created";
	m_response.status_code_list[202] = "Accepted";
	m_response.status_code_list[203] = "Non-Authoritative Information";
	m_response.status_code_list[204] = "No Content";
	m_response.status_code_list[205] = "Reset Content";
	m_response.status_code_list[206] = "Partial Content";
	m_response.status_code_list[207] = "Multi-Status";
	m_response.status_code_list[208] = "Already Reported";
	m_response.status_code_list[226] = "IM Used";
	m_response.status_code_list[300] = "Multiple Choices";
	m_response.status_code_list[301] = "Moved Permanently";
	m_response.status_code_list[302] = "Found";
	m_response.status_code_list[303] = "See Other";
	m_response.status_code_list[304] = "Not Modified";
	m_response.status_code_list[305] = "Use Proxy";
	m_response.status_code_list[306] = "Switch Proxy";
	m_response.status_code_list[307] = "Temporary Redirect";
	m_response.status_code_list[308] = "Permanent Redirect";
	m_response.status_code_list[400] = "Bad Request";
	m_response.status_code_list[401] = "Unauthorized";
	m_response.status_code_list[402] = "Payment Required";
	m_response.status_code_list[403] = "Forbidden";
	m_response.status_code_list[404] = "Not Found";
	m_response.status_code_list[405] = "Method Not Allowed";
	m_response.status_code_list[406] = "Not Acceptable";
	m_response.status_code_list[407] = "Proxy Authentication Required";
	m_response.status_code_list[408] = "Request Timeout";
	m_response.status_code_list[409] = "Conflict";
	m_response.status_code_list[410] = "Gone";
	m_response.status_code_list[411] = "Length Required";
	m_response.status_code_list[412] = "Precondition Failed";
	m_response.status_code_list[413] = "Payload Too Large";
	m_response.status_code_list[414] = "URI Too Long";
	m_response.status_code_list[415] = "Unsupported Media Type";
	m_response.status_code_list[416] = "Range Not Satisfiable";
	m_response.status_code_list[417] = "Expectation Failed";
	m_response.status_code_list[418] = "I\'m a teapot";
	m_response.status_code_list[421] = "Misdirected Request";
	m_response.status_code_list[422] = "Unprocessable Entity";
	m_response.status_code_list[423] = "Locked";
	m_response.status_code_list[424] = "Failed Dependency";
	m_response.status_code_list[425] = "Too Early";
	m_response.status_code_list[426] = "Upgrade Required";
	m_response.status_code_list[428] = "Precondition Required";
	m_response.status_code_list[429] = "Too Many Requests";
	m_response.status_code_list[431] = "Request Header Fields Too Large";
	m_response.status_code_list[451] = "Unavailable For Legal Reasons";
	m_response.status_code_list[500] = "Internal Server Error";
	m_response.status_code_list[501] = "Not Implemented";
	m_response.status_code_list[502] = "Bad Gateway";
	m_response.status_code_list[503] = "Service Unavailable";
	m_response.status_code_list[504] = "Gateway Timeout";
	m_response.status_code_list[505] = "HTTP Version Not Supported";
	m_response.status_code_list[506] = "Variant Also Negotiates";
	m_response.status_code_list[507] = "Insufficient Storage";
	m_response.status_code_list[508] = "Loop Detected";
	m_response.status_code_list[510] = "Not Extended";
	m_response.status_code_list[511] = "Network Authentication Required";
}


std::string search_content_type(std::string filename)
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

Server *Client::getServer()
{
    return (_server);
}