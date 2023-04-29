# include "webserv.hpp"


/*
	TCP server setup flow:
		socket: create socket
		bind: bind socket address
		listen: waiting for incoming request
		accept: accept incoming request
	
	chosen option:
		AF_INET		IPv4 Internet protocols
		SOCK_STREAM	Provides sequenced, reliable, two-way, connection-based byte streams.
		define 20 as the maximum length to which the queue of pending connections for sockfd may grow. 
*/

TcpServer::TcpServer(string ip_address, int port) : 
	m_ip_address(ip_address), \
	m_port(port), m_new_socket(),\
	m_incomingMessage(), \
	m_socketAddress(),\
	m_socketAddress_len(sizeof(m_socketAddress)),\
	m_serverMessage(buildResponse())
{
	cout << "Initalizing the server." << endl;
	m_socketAddress.sin_family = AF_INET;
	m_socketAddress.sin_port = htons(m_port);
	m_socketAddress.sin_addr.s_addr = inet_addr(m_ip_address.c_str());

	if (startServer() != 0)
	{
		ostringstream ss;
		ss << "Failed to start server with PORT: " << ntohs(m_socketAddress.sin_port);
		General::log(ss.str());
	}
}

TcpServer::~TcpServer()
{
	cout << "Terminating the server." << endl;
	closeServer();
}

void TcpServer::create_socket()
{
	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket < 0)
		General::exitWithError("Cannot create socket");
}

void TcpServer::listen_server()
{
	// Set SO_REUSEADDR option on the socket
    int optval = 1;
    if (setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&optval, sizeof(optval)) < 0)
        General::exitWithError("Failed to set SO_REUSEADDR. errno: ");
    std::cout << "Socket created" << std::endl;
    if ((ioctl(m_socket, FIONBIO, (char *)&optval)) < 0)
        General::exitWithError("Failed to ioctl. errno: ");
    if (bind(m_socket, (struct sockaddr*)&m_socketAddress, sizeof(m_socketAddress)) < 0)
        General::exitWithError("Failed to bind to port 9999. errno: ");
    std::cout << "Bind done" << std::endl;
    // Start listening. Hold at most 10 connections in the queue
    if (listen(m_socket, 10) < 0)
        General::exitWithError("Failed to listen on socket. errno: ");
    std::cout << "Waiting for incoming connections..." << std::endl;
}

int TcpServer::startServer()
{
    create_socket();
    std::cout << "Socket created" << std::endl;
    listen_server();
    std::cout << "Server listening" << std::endl;
    create_poll();


	// m_socket = socket(AF_INET, SOCK_STREAM, 0);
	// if (m_socket < 0)
	// 	General::exitWithError("Cannot create socket");
	// if (bind(m_socket, (sockaddr *)&m_socketAddress, m_socketAddress_len) < 0)
	// 	General::exitWithError("Cannot connect socket to address");
	return (0);
}


// create poll instance
void TcpServer::create_poll()
{
    int end_server = 0, compress_array = 0;
    int new_sd = -1;
    int close_conn;
    int current_size, rc, nfds = 1;
    struct pollfd fds[200];
    
    memset(fds, 0, sizeof(fds));
    fds[0].fd = m_socket;
    fds[0].events = POLLIN;
    int timeout = (3 * 60 * 1000);
    do
    {
        if ((rc = poll(fds, nfds, timeout)) < 0)
            break;
        if (rc == 0)
            break;
        current_size = nfds;
        for (int i = 0; i < current_size; i++)
        {
            if (fds[i].revents == 0)
                continue;
            if (fds[i].revents != POLLIN)
            {
                printf("  Error! revents = %d\n", fds[i].revents);
                end_server = 1;
                break;
            }
            if (fds[i].fd == m_socket)
            {
                printf("  Listening socket is readable\n");
                do
                {
                    new_sd = accept(m_socket, 0, 0);
                    if (new_sd < 0)
                    {
                        if (errno != EWOULDBLOCK)
                            end_server = 1;
                        break;
                    }
                    printf("  New incoming connection - %d\n", new_sd);
                    fds[nfds].fd = new_sd;
                    fds[nfds].events = POLLIN;
                    nfds++;
                } while (new_sd != -1);
            }
            else
            {
                printf("  Descriptor %d is readable\n", fds[i].fd);
                close_conn = 0;
                do
                {
                    rc = recv(fds[i].fd, m_buffer, sizeof(m_buffer), 0);
                    if (rc < 0)
                    {
                        if (errno != EWOULDBLOCK)
                            close_conn = 1;
                        break;
                    }
                    if (rc == 0)
                    {
                        printf("  Connection closed\n");
                        close_conn = 1;
                        break;
                    }
                    m_request = parse_request();
                    // Process the request
                    std::string response_str = process_request(m_request);
                    // Send the response to the client
                    std::cout << "response_str len : " << response_str.size() << std::endl;
                    // if (response_str.size() > 4096)
                    // {
                    //     if ((multi_send(fds[i].fd, response_str)) < 0)
                    //     {
                    //         perror("  send() failed");
                    //         close_conn = 1;
                    //         break;
                    //     }
                    // } else
                    if (send(fds[i].fd, response_str.c_str(), response_str.size(), 0) < 0)
                    {
                        perror("  send() failed");
                        close_conn = 1;
                        break;
                    }
                } while (1);
                if (close_conn)
                {
                    close(fds[i].fd);
                    fds[i].fd = -1;
                    compress_array = 1;
                }
            }
        }
        if (compress_array)
        {
          compress_array = 0;
          for (int i = 0; i < nfds; i++)
          {
            if (fds[i].fd == -1)
            {
              for(int j = i; j < nfds; j++)
              {
                fds[j].fd = fds[j+1].fd;
              }
              i--;
              nfds--;
            }
          }
        }    
        
        
    } while ( end_server == 0);
    for (int i = 0; i < nfds; i++)
    {
      if(fds[i].fd >= 0)
        close(fds[i].fd);
    }
}

Request& TcpServer::parse_request() {
    std::stringstream request_stream;                                                                                   
    request_stream << m_buffer;                                                                      
    std::string line;                                                                               
    bool headers_done = false;                                                                      
    while (std::getline(request_stream, line) && !line.empty()) {                                   
        if (!headers_done) {                                                                        
            if (line.find(":") != std::string::npos) {                                              
                std::string header_name = line.substr(0, line.find(":"));                           
                std::string header_value = line.substr(line.find(":") + 1);                         
                m_request.headers[header_name] = header_value;                                       
            } else {                                                                                
                headers_done = true;                                                                
                m_request.method = line.substr(0, line.find(" "));                                   
                m_request.uri = line.substr(line.find(" ") + 1, line.rfind(" ") - line.find(" ") - 1); 
            }
        } else {                                                                                    
            m_request.body += line + "\n";                                                           
        }
    }
    return (m_request);                                                                                  
}

std::string search_content_type(std::string filename)
{
    std::string content_type;
    int i = filename.length();
    if (filename[i - 1] == 'l' && filename[i - 2] == 'm' && filename[i - 3] == 't' && filename[i - 4] == 'h' && filename[i - 5] == '.')
        content_type = "text/html";
    else if (filename[i - 1] == 'g' && filename[i - 2] == 'p' && filename[i - 3] == 'j' && filename[i - 4] == '.')
        content_type = "image/jpeg";
    else if (filename[i - 1] == 's' && filename[i - 2] == 's' && filename[i - 3] == 'l' && filename[i - 4] == 'e' && filename[i - 5] == 'c' && filename[i - 6] == '.')
        content_type = "text/css";
    else if (filename[i - 1] == 's' && filename[i - 2] == 's' && filename[i - 3] == 'e' && filename[i - 4] == 'r' && filename[i - 5] == 't' && filename[i - 6] == '.')
        content_type = "text/javascript";
    else if (filename[i - 1] == 'p' && filename[i - 2] == 'h' && filename[i - 3] == 'p')
        content_type = "application/x-httpd-php";
    else if (filename[i - 1] == 'f' && filename[i - 2] == 'i' && filename[i - 3] == 'g')
        content_type = "image/x-icon";
        // else if pdf
    else if (filename[i - 1] == 'f' && filename[i - 2] == 'd' && filename[i - 3] == 'p')
        content_type = "application/pdf";
   else
        content_type = "text/plain";
    return (content_type);
}

std::string read_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string TcpServer::process_request(const Request& request) 
{
    std::cout << "Method: " << request.method << std::endl;
    std::string response;
    if (request.method == "GET")
    {
        // Traitement de la requête GET
        if (request.uri == "/")
        {
            // std::cout << "URI: " << request.uri << std::endl;
            std::string body = read_file("pages/index.html");
            std::string body_size = std::to_string(body.size());
            response = "HTTP/1.1 200 OK\r\nContent-Type:text/html\r\nContent-Length: " + body_size + "\r\n\r\n" + body;
        }
        else
        {
            std::cout << "URI: " << request.uri << std::endl;
            // Si l'URI est différent de "/", renvoyer le fichier correspondant
            std::string filename = request.uri.substr(1); // Supprimer le premier caractère "/"
            std::string body = read_file(filename);
            if (body == "") {
                // Si le fichier n'existe pas, renvoyer une réponse 404

				
                // std::cout << "Check filename: " << filename << std::endl;
                // response = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";

                std::string content_type = "text/html";
				body = read_file("www/site/errorPages/404.html");
                std::string body_size = std::to_string(body.size());
                response = "HTTP/1.1 200 OK\r\nContent-Type:";
                response += content_type;
                response += "\r\nContent-Length: " + body_size + "\r\n\r\n" + body;
            }
            else
            {
                std::string content_type = search_content_type(filename);
                std::string body_size = std::to_string(body.size());
                response = "HTTP/1.1 200 OK\r\nContent-Type:";
                response += content_type;
                response += "\r\nContent-Length: " + body_size + "\r\n\r\n" + body;
            }
        }
    }
    else if (request.method == "DELETE")
    {
        // Traitement de la requête DELETE
        // check authorisations
        response = "HTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n";
    }
    else if (request.method == "POST")
    {
        // Traitement de la requête POST
        response = "HTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n";
    }
    else if (request.method == "PUT")
    {
        // Traitement de la requête PUT
        response = "HTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n";
    }
    else
    {
        // Requête non prise en charge
        response = "HTTP/1.1 501 Not Implemented\r\nContent-Length: 0\r\n\r\n";
    }
    return (response);
}

void TcpServer::closeServer()
{
	close(m_socket);
	close(m_new_socket);
	exit(0);
}

void TcpServer::startListen()
{
	if (listen(m_socket, 20) < 0)
		General::exitWithError("Socket listen failed");
	ostringstream ss;
	ss << "\n*** Listening on ADDRESS: "  << inet_ntoa(m_socketAddress.sin_addr) 
		<< " PORT: "  << ntohs(m_socketAddress.sin_port) << " ***";
	General::log(ss.str());

	int bytesReceived;
	while (42)
	{
		char buffer[BUFFER_SIZE] = {0};
		General::log("\n====== Waiting for a new connection ======");
		acceptConnection(m_new_socket);
		getHeader(m_new_socket);
		getPayload(m_new_socket);
		sendResponse();
		close(m_new_socket);
	}
}

void	TcpServer::getPayload(int &new_socket)
{
	int bytesReceived;
	char buffer[1024] = {0};

	int valread = recv(m_new_socket, buffer, 1024, 0);
	if (valread == -1)
		General::exitWithError("Error in recv()");
	else
		General::log("\nReceived message: \n" + string(buffer));
}

void	TcpServer::getHeader(int &new_socket)
{
	int bytesReceived;
	char buffer[BUFFER_SIZE] = {0};

	bytesReceived = read(m_new_socket, buffer, BUFFER_SIZE);
	if (bytesReceived < 0)
		General::exitWithError("Failed to read bytes from client socket connection");
	string requestString(buffer);
	string method = requestString.substr(0, requestString.find(' '));
	General::log("Received HTTP method: " + method);
	General::log("\nReceived header: \n" + string(requestString));
}

void TcpServer::acceptConnection(int &new_socket)
{
	new_socket = accept(m_socket, (sockaddr *)&m_socketAddress, &m_socketAddress_len);
	if (new_socket < 0)
	{
		ostringstream ss;
		ss << "Server failed to accept incoming connection from ADDRESS: "
			<< inet_ntoa(m_socketAddress.sin_addr) 
			<< "; PORT: " << ntohs(m_socketAddress.sin_port);
		General::exitWithError(ss.str());
	}
}

string TcpServer::buildResponse()
{
	string htmlFile = "<!DOCTYPE html><html lang=\"en\"><body><h1> HOME </h1><p> Hello from your Server :) </p></body></html>";
	ostringstream ss;
	ss << "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " << htmlFile.size() << "\n\n"
		<< htmlFile;
	return ss.str();
}

void TcpServer::sendResponse()
{
	long bytesSent;

	bytesSent = write(m_new_socket, m_serverMessage.c_str(), m_serverMessage.size());

	if (bytesSent == m_serverMessage.size())
	{
		General::log("------ Server Response sent to client ------\n");
		General::log("Responsed message: \n" + m_serverMessage);
	}
	else
		General::log("Error sending response to client");
}