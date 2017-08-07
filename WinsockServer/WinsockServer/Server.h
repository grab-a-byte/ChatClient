#pragma once
#undef UNICODE
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <stdio.h>

enum PACKET
{
	P_ChatMessage,
	P_DirectMessage
};

class Server
{
public:
	Server(int PORT);
	bool ListenForNewConnection();

	std::vector<std::string> usernames = {};

private:
	SOCKET Connections[100];
	std::thread connectionThreads[100];

	addrinfo* result;
	addrinfo hints;
	SOCKET ListenSocket; 
	int iResult;

	int ConnectionCounter = 0;

	void Getusername(int id);

	bool GetInt(int id, int & value);
	bool SendInt(int id, int value);
	bool SendBool(int id, bool value);
	bool GetBool(int id, bool& value);
	bool SendPacketType(int id, const PACKET & packetType);
	bool GetPacketType(int id, PACKET & packetType);
	bool SendString(int id, const std::string & value);
	bool GetString(int id, std::string & value);
	bool ProcessPacket(int index, PACKET packetType);
	bool CloseConnection(int index);

	static void ClientHandler(int index);
};

static Server* serverPtr; 