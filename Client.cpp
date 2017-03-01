#include <WinSock2.h>
#include <iostream>
using namespace std;

#pragma comment(lib, "Ws2_32.lib")

const int STRLEN = 256;

// Closes the socket and performs the WSACleanup


int main()
{
	WSADATA		wsaData;
	SOCKET		mySocket;
	SOCKET		acceptSocket;
	SOCKADDR_IN	serverAddr;
	char		ipAddress[20];
	int			port;
	char		sendMessage[STRLEN];
	char		recvMessage[STRLEN];
	bool		done = false;

	cout << "Enter server IP address: ";
	cin >> ipAddress;

	cout << "Enter server port number: ";
	cin >> port;
	cin.ignore();

	// Loads Windows DLL (Winsock version 2.2) used in network programming
	if ( WSAStartup( MAKEWORD(2, 2), &wsaData ) != NO_ERROR )
	{
		cerr << "ERROR: Problem with WSAStartup\n";
		return 1;
	}

	// Create a new socket for communication
	mySocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if ( mySocket == INVALID_SOCKET )
	{
		cerr << "ERROR: Cannot create socket\n";
		WSACleanup();
		return 1;
	}

	cout << "\nAttempting to connect...\n";

	// Setup a SOCKADDR_IN structure which will be used to hold address
	// and port information for the server. Notice that the port must be converted
	// from host byte order to network byte order.
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons( port );
	serverAddr.sin_addr.s_addr = inet_addr( ipAddress );

	// Try to connect
	if ( connect( mySocket, (SOCKADDR*)&serverAddr, sizeof( serverAddr ) ) == SOCKET_ERROR )
	{
		cerr << "ERROR: Failed to connect\n";
		cleanup(mySocket);
		return 1;
	}

	cout << "Connected...\n\n";
	
	while (!done)
	{
		// Get a message from the user and attempt to send it to the remote computer
		cout << "Send > ";
		cin.get( sendMessage, STRLEN );
		cin.ignore();
		int iSend = send( mySocket, sendMessage, strlen( sendMessage ), 0 );
		if (iSend == SOCKET_ERROR) 
		{
			cerr << "ERROR: Failed to send message\n";
			cleanup(mySocket);
			return 1;
		}

		// Wait to receive a reply message back from the remote computer
		cout << "\n\t--WAIT--\n\n";
		int iRecv = recv( mySocket, recvMessage, STRLEN, 0 );
		if (iRecv > 0)
		{
			recvMessage[iRecv] = '\0';
			cout << "Recv > " << recvMessage << "\n";
		}
		else if (iRecv == 0)
		{
			cout << "Connection closed\n";
			cleanup(mySocket);
			return 0;
		}
		else
		{
			cerr << "ERROR: Failed to receive message\n";
			cleanup(mySocket);
			return 1;
		}

		

		// Communication ends when client sends an "end" message,
        // and the server responds with an "end" message
        if ( strcmp( recvMessage, "end" ) == 0 ||
                strcmp( sendMessage, "end" ) == 0 )
        {
            done = true;
        }
	}
	
	cleanup(mySocket);

	return 0;
}

void cleanup(SOCKET socket)
{
	closesocket(socket);
	WSACleanup();
}