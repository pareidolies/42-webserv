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

int TcpServer::startServer()
{
    int optval = 1;

	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket < 0)
		General::exitWithError("Cannot create socket");
    if (setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR , (char *)&optval, sizeof(optval)) < 0)
        General::exitWithError("Failed to set SO_REUSEADDR. errno: ");
	if (bind(m_socket, (sockaddr *)&m_socketAddress, m_socketAddress_len) < 0)
		General::exitWithError("Cannot connect socket to address");
	return (0);
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
		General::log("\n====== Waiting for a new connection ======");
		acceptConnection(m_new_socket);
		getPayload(m_new_socket);
        parse_request(m_request, m_buffer);
        std::string response_str = process_request(m_request);
		sendResponse(response_str);
		memset(m_buffer, 0, sizeof(m_buffer));
		close(m_new_socket);
	}
}

void TcpServer::getPayload(int &new_socket)
{
    int content_length = 0;
    char *body_start = NULL;
    char *content_length_str = NULL;
    int bytesReceived = 0;

    // Read the request headers to find the Content-Length header
    int valread = recv(m_new_socket, m_buffer, sizeof(m_buffer), 0);
    if (valread == -1)
        General::exitWithError("Error in recv()");

    // Find the start of the request body
    body_start = strstr(m_buffer, "\r\n\r\n");
    if (body_start != NULL) {
        body_start += 4;
        // Look for the Content-Length header
        content_length_str = strstr(m_buffer, "Content-Length: ");
        if (content_length_str != NULL) {
            content_length_str += strlen("Content-Length: ");
            content_length = atoi(content_length_str);
            // Make sure we have enough space in the buffer
            if (content_length > sizeof(m_buffer) - (body_start - m_buffer)) {
                General::exitWithError("Payload too large for buffer");
            }
            // Read the payload
            bytesReceived = recv(m_new_socket, body_start, content_length, 0);
            if (bytesReceived == -1)
                General::exitWithError("Error in recv()");
        }
    }
	m_request.raw_request = std::string(m_buffer);
    General::log("\nReceived message: \n" + string(m_buffer));
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

void TcpServer::sendResponse(std::string response_str)
{
    if (send(m_new_socket, response_str.c_str(), response_str.size(), 0) < 0)
	    General::log("Error sending response to client");
    else
    {
	    General::log("------ Server Response sent to client ------\n");
	    // General::log("Responsed message: \n" + response_str);
    }
}