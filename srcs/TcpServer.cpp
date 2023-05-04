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
<<<<<<< HEAD
	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket < 0)
		General::exitWithError("Cannot create socket");
=======
    int optval = 1;

	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket < 0)
		General::exitWithError("Cannot create socket");
    if (setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR , (char *)&optval, sizeof(optval)) < 0)
        General::exitWithError("Failed to set SO_REUSEADDR. errno: ");
>>>>>>> origin/sdesseau
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
<<<<<<< HEAD
		char buffer[BUFFER_SIZE] = {0};
		General::log("\n====== Waiting for a new connection ======");
		acceptConnection(m_new_socket);
		getHeader(m_new_socket);
		getPayload(m_new_socket);
		sendResponse();
=======
		General::log("\n====== Waiting for a new connection ======");
		acceptConnection(m_new_socket);
		getPayload(m_new_socket);
        parse_request(m_request, m_buffer);
        std::string response_str = process_request(m_request);
		sendResponse(response_str);
>>>>>>> origin/sdesseau
		close(m_new_socket);
	}
}

void	TcpServer::getPayload(int &new_socket)
{
	int bytesReceived;
<<<<<<< HEAD
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
=======

	int valread = recv(m_new_socket, m_buffer, sizeof(m_buffer), 0);
	if (valread == -1)
		General::exitWithError("Error in recv()");
	else
		General::log("\nReceived message: \n" + string(m_buffer));
>>>>>>> origin/sdesseau
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

<<<<<<< HEAD
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
=======
void TcpServer::sendResponse(std::string response_str)
{
    if (send(m_new_socket, response_str.c_str(), response_str.size(), 0) < 0)
	    General::log("Error sending response to client");
    else
    {
	    General::log("------ Server Response sent to client ------\n");
	    // General::log("Responsed message: \n" + response_str);
    }
>>>>>>> origin/sdesseau
}