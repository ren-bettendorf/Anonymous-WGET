#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <csignal>
#include <thread>
#include <boost/algorithm/string.hpp>
#include <vector>

using namespace std;
using namespace boost;

#define DEFAULTPORT 9000


void cleanExit(int exitCode, string message)
{
	cout << message << endl;
	exit(exitCode);
}

int handleNextStone(char* chainlistChar, char* url)
{
	string chainlist = string(chainlistChar);

	vector<string> splitChain;
	trim(chainlist);
	split(splitChain, chainlist, is_any_of(" "), token_compress_on);

	srand(time(NULL));

	int randIndex = rand() % splitChain.size();

	string nextStone = splitChain.at(randIndex);
	splitChain.erase(splitChain.begin() + randIndex);

	vector<string> splitIpAndPort;
	split(splitIpAndPort, nextStone, is_any_of(":"));

	struct sockaddr_in nextStoneAddr;

	nextStoneAddr.sin_family = AF_INET;
        nextStoneAddr.sin_addr.s_addr = inet_addr(splitIpAndPort.at(0).c_str());
       	nextStoneAddr.sin_port = htons(stoi(splitIpAndPort.at(1)));
	int nextStoneSock;

        if ( (nextStoneSock = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
        {
                string errorMessage("Error: Unable to open socket");
                cleanExit(1, errorMessage);
        }
        if ( (bind(nextStoneSock, (struct sockaddr*)&nextStoneAddr, sizeof(nextStoneAddr))) < 0 )
        {
                string errorMessage("Error: Unable to bind socket");
                cleanExit(1, errorMessage);
        }

	return nextStoneSock;
}

void grabFile(char* url)
{

}

void sendFileToPreviousStone(int sock)
{

}

void waitForFile(int sock)
{

}

void deserializePacket() 
{
    
}

void deserializeInitialPacket(char* message)
{
	
}

int displayHelpInfo() 
{
	cout << "Welcome to Stepping Stone!" << endl << endl;
	cout << "To start the stepping stone type ./ss" << endl;
	cout << "The flag '-p' can be used to define a port other than teh default 9000" << endl << endl;
	cleanExit(1, "");
}


void handleConnectionThread(int port, int previousStoneSock) 
{
	int nextStoneSock;
	char messageHeaderBuffer[4];

	if ( recv(previousStoneSock, messageHeaderBuffer, 4, 0) < 0 )
	{
		cleanExit(1, "Error: recv failed");
	}

	uint16_t sizeChainlist = -1;
	memcpy(&sizeChainlist, messageHeaderBuffer, 2);
	sizeChainlist = ntohs(sizeChainlist);

	uint16_t sizeUrl = -1;
	memcpy(&sizeUrl, messageHeaderBuffer + 2, 2);
	sizeUrl = ntohs(sizeUrl);

	char messageBuffer[sizeUrl + sizeChainlist];
	if ( recv(previousStoneSock, messageBuffer, sizeUrl + sizeChainlist, 0) < 0 )
	{
		cleanExit(1, "Error: recv failed");
	}

	char chainlist[sizeChainlist];
	memcpy(chainlist, messageBuffer, sizeChainlist);

	char url[sizeUrl];
	memcpy(url, messageBuffer + sizeChainlist, sizeUrl);

	if(sizeChainlist > 0)
	{
		nextStoneSock = handleNextStone(chainlist, url);
		waitForFile(nextStoneSock);
		
	}
	else
	{
		grabFile(url);
		sendFileToPreviousStone(previousStoneSock);
	}
}

void signalHandler(int signal)
{
	string message("Error: Handled signal interruption");
	cleanExit(signal, message);
}

// Expecting Call: ./ss [-p port]
int main(int argc, char* argv[]) 
{
	// handle signals
	signal(SIGINT/SIGTERM/SIGKILL, signalHandler);

	int serverSock;
	struct sockaddr_in sin;
    int port;

    if(argc == 1) 
	{
        port = DEFAULTPORT;
    } 
	else 
	{
        if(argc != 3 || strcmp(argv[1], "-p") != 0) 
		{
            displayHelpInfo();
        }
        port = atoi(argv[2]);
    }
	
	char ip[INET_ADDRSTRLEN];
	char hostname[256];
	
	if(gethostname(hostname, sizeof hostname) != 0) 
	{
		string errorMessage("Error: Could not find hostname");
		cleanExit(1, errorMessage);
	}
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = htons(INADDR_ANY);
	sin.sin_port = htons(port);
	
	//inet_ntop(AF_INET, &addr.sin_addr.s_addr, ip, INET_ADDRSTRLEN);
	
	cout << "Stepping Stone " << ip << ":" << port << endl;

	if ( (serverSock = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
	{
		string errorMessage("Error: Unable to open socket");
		cleanExit(1, errorMessage);
	}
	if ( (bind(serverSock, (struct sockaddr*)&sin, sizeof(sin))) < 0 )
	{
		string errorMessage("Error: Unable to bind socket");
		cleanExit(1, errorMessage);
	}
	
	listen(serverSock, 1);

	while(true)
	{
		int incomingSock;
		struct sockaddr_in clientAddr;
		socklen_t addrSize = sizeof clientAddr;
		if ( (incomingSock = accept(serverSock, (struct sockaddr*)&clientAddr, &addrSize)) < 0 )
		{
			string errorMessage("Error: Problem accepting client.");
			cleanExit(1, errorMessage);
		}

		thread ssSockThread(handleConnectionThread, port, incomingSock);
		ssSockThread.join();
	}
    
    return 0;
}
