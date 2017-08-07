#include "Client.h"

Client::Client(int argc, char **argv, int PORT)
{
	WSADATA wsaData;

	// Validate the parameters
	if (argc != 2) {
		printf("usage: %s server-name\n", argv[0]);
	}

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		exit(0);
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo(argv[1], std::to_string(PORT).c_str(), &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
	}

	clientPtr = this;
}

bool Client::Connect()
{
	bool connected = false;

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
	{

		// Create a SOCKET for connecting to server
		ServerConnection = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (ServerConnection == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return false;
		}

		// Connect to server.
		iResult = connect(ServerConnection, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR)
		{
			std::cout << "Could not connect to server, error: " << WSAGetLastError() << std::endl;
			closesocket(ServerConnection);
			ServerConnection = INVALID_SOCKET;
			continue;
		}
		else
		{
			connected = true;
			break;

		}
	}

	return connected;
}

bool Client::ProcessPacket(PACKET packetType)
{
	

	switch (packetType)
	{
	case P_ChatMessage:
	{
		std::string message;
		if (!GetString(message))
			return false;
		std::cout << message << std::endl;
		break;
	}

	case P_DirectMessage:
	{
		bool value;
		if (!GetBool(value))
		DM_Failed = value;
		break;
	}
	default:
		std::cout << "unrecognized packet: " << packetType << std::endl;
	}
	return true;
}

void Client::ClientHandler()
{
	PACKET packetType;
	while (true)
	{
		//get type
		if (!clientPtr->GetPacketType(packetType))
			break;
		//procees package type
		if (!clientPtr->ProcessPacket(packetType))
			break;
	}

	//lost connection or error occured

	std::cout << "Lost connection to the server" << std::endl;
	if (clientPtr->CloseConnection())
	{
		std::cout << "Socket to server was closed successfully" << std::endl;
	}
}

bool Client::SendString(const std::string& value)
{
	if (!SendPacketType(P_ChatMessage))
		return false;

	int bufferLength = value.size();
	if (!SendInt(bufferLength))
		return false;

	int returnCheck = send(ServerConnection, value.c_str(), bufferLength, NULL);
	if (returnCheck == SOCKET_ERROR)
		return false;

	return true;
}

bool Client::SendDirectMessage(const std::string& value)
{
	if (!SendPacketType(P_DirectMessage))
		return false;

	//send target user
	int bufferLength = value.size();
	if (!SendInt(bufferLength))
		return false;

	int returnCheck = send(ServerConnection, value.c_str(), bufferLength, NULL);
	if (returnCheck == SOCKET_ERROR)
		return false;

	return true;
}

bool Client::SendInt(const int& value) const
{
	int returnCheck = send(ServerConnection, (char*)&value, sizeof(int), NULL);
	if (returnCheck == SOCKET_ERROR)
		return false;

	return true;
}
bool Client::GetInt(int& value)
{
	int returnCheck = recv(ServerConnection, (char*)& value, sizeof(int), NULL);
	if (returnCheck == SOCKET_ERROR)
		return false;

	return true;
}
bool Client::SendPacketType(const PACKET& packetType)
{
	int returnCheck = send(ServerConnection, (char*)& packetType, sizeof(PACKET), NULL);
	if (returnCheck == SOCKET_ERROR)
		return false;

	return true;
}
bool Client::GetPacketType(PACKET& packetType)
{
	int returnCheck = recv(ServerConnection, (char*)& packetType, sizeof(PACKET), NULL);
	if (returnCheck == SOCKET_ERROR)
		return false;

	return true;
}
bool Client::GetString(std::string& value)
{
	int bufferLength = 0;
	if (!GetInt(bufferLength))
		return false;

	char* buffer = new char[bufferLength + 1]; // +1 to allow for terminating '/0'
	buffer[bufferLength] = '\0';

	int returnCheck = recv(ServerConnection, buffer, bufferLength, NULL);
	value = buffer;
	delete[] buffer;

	if (returnCheck == SOCKET_ERROR)
		return false;

	return true;
}
bool Client::SendBool(bool value)
{
	int returnCheck = send(ServerConnection, (char*)& value, sizeof(bool), NULL);
	if (returnCheck == SOCKET_ERROR)
		return false; 

	return true; 
}

bool Client::GetBool(bool& value)
{
	int returnCheck = recv(ServerConnection, (char*)& value, sizeof(bool), NULL);
	if (returnCheck == SOCKET_ERROR)
		return false;

	return true;
}

bool Client::CloseConnection()
{
	if (closesocket(ServerConnection == SOCKET_ERROR))
	{
		if (WSAGetLastError() == WSAENOTSOCK)
			return true;

		std::cout << "Failed to close the socket. Winsock Error: " << std::to_string (WSAGetLastError()) << std::endl;
		return false;
	}

	return true;
}
