# include "webserv.hpp"

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
	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket < 0)
		General::exitWithError("Cannot create socket");
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
	ss << "\n*** Listening on ADDRESS: " << inet_ntoa(m_socketAddress.sin_addr) << " PORT: " << ntohs(m_socketAddress.sin_port) << " ***\n\n";
	General::log(ss.str());

	int bytesReceived;
	while (42)
	{
		char buffer[BUFFER_SIZE] = {0};
		General::log("\n====== Waiting for a new connection ======");
		acceptConnection(m_new_socket);
		bytesReceived = read(m_new_socket, buffer, BUFFER_SIZE);
		if (bytesReceived < 0)
			General::exitWithError("Failed to read bytes from client socket connection");
		General::log("------ Received Request from client ------");
		string requestString(buffer);
		string method = requestString.substr(0, requestString.find(' '));
		General::log("Received HTTP method: " + method);
		General::log("\nReceived header: \n" + string(requestString));
		int valread = recv(m_new_socket, buffer, 1024, 0);
		if (valread == -1)
			General::exitWithError("Error in recv()");
		else if (valread == 0)
		{
			close(m_new_socket);
			General::log("Client disconnected");
		}
		else
			General::log("\nReceived message: \n" + string(buffer));
		sendResponse();
		close(m_new_socket);
	}
}

void TcpServer::acceptConnection(int &new_socket)
{
	new_socket = accept(m_socket, (sockaddr *)&m_socketAddress, &m_socketAddress_len);
	if (new_socket < 0)
	{
		ostringstream ss;
		ss << "Server failed to accept incoming connection from ADDRESS: " << inet_ntoa(m_socketAddress.sin_addr) << "; PORT: " << ntohs(m_socketAddress.sin_port);
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