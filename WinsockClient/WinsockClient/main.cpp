#include "Client.h"
#include "PACKETENUM.h"


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

Client* myClient;

#define DEFAULT_PORT 27015

void SecureUsername()
{
	// Make client have a username
	bool usernameAccepted = false;
	do
	{
		std::cout << "Please provide a username" << std::endl;
		std::string username;
		std::getline(std::cin, username);
		myClient->SendString(username);

		myClient->GetBool(usernameAccepted);

		if (!usernameAccepted)
			std::cout << "Username Taken, Try again" << std::endl;

	} while (!usernameAccepted);
}

int main(int argc, char **argv)
{
	myClient = new Client(argc, argv, DEFAULT_PORT);

	if (!myClient->Connect())
	{
		system("pause");
		return 1;
	}

	SecureUsername();

	myClient->StartSubRoutine();

	//Receive and create messages
	std::string buffer;
	while (true)
	{
		std::getline(std::cin, buffer); // get line from cmd console

		if (buffer[0] == '@')
		{
			buffer.erase(0, 1);
			if (!myClient->SendDirectMessage(buffer))
				break;
		}
		else
		{
			if (!myClient->SendString(buffer))
				break; // leave as server conn lost
		}
	}

	// cleanup
	system("pause");

	WSACleanup();

	system("pause");

	return 0;
}