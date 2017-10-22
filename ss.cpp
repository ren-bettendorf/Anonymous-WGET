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
#include "awget.h"

using namespace std;
using namespace boost;

#define DEFAULTPORT 9000


void cleanExit(int exitCode, string message)
{
	cout << message << endl;
	exit(exitCode);
}

void sendSystemWget(string url)
{
	cout << "wget " << url << endl;
	const char* wgetMessage = ("wget " + url).c_str();
	system(wgetMessage);
}

string createFinalRequestUrl(string url)
{
        cout << "Parsing: " << url << endl;
	string prefix("");
        if( contains(url, ("://")) )
        {
                cout << "String has beginning http(s). Stripping" << endl;
                cout << "Found at position " << url.find("://") << endl;
		prefix = url.substr(0, url.find("://") + 3);
                url = url.substr(url.find("://") + 3);
        }
        int countSlashes = count(url.begin(), url.end(), '/');
        if ( countSlashes < 2 )
        {
                if( countSlashes == 0 )
                {
                        cout << "Adding slash" << endl;
                        url = url + "/";
                }
		else if(url.find_last_of("/") != url.size() -1)
		{
			return prefix + url;
		}
                url = url + "index.html";
        }
	url = prefix + url;
        cout << "FINAL: " << url << endl;
	return url;
}

string parseFileName(string url)
{
	return url.substr(url.find_last_of("/") + 1);

}

vector<string> splitChainlistFromLastStone(char* chainlistChar)
{
	string chainlist = string(chainlistChar);

	vector<string> splitChainlist;
	trim(chainlist);
	split(splitChainlist, chainlist, is_any_of(" "), token_compress_on);

	return splitChainlist;
}

int selectRandomStoneIndex(int max)
{
	srand(time(NULL));

	return rand() % max;
}



int connectToNextStone(string ip, int port)
{
	struct sockaddr_in nextStoneAddr;

	nextStoneAddr.sin_family = AF_INET;
        nextStoneAddr.sin_addr.s_addr = inet_addr(ip.c_str());
       	nextStoneAddr.sin_port = htons(port);
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

int displayHelpInfo() 
{
	cout << "Welcome to Stepping Stone!" << endl << endl;
	cout << "To start the stepping stone type ./ss" << endl;
	cout << "The flag '-p' can be used to define a port other than teh default 9000" << endl << endl;
	cleanExit(1, "");
}


void handleConnectionThread(int previousStoneSock) 
{
	int nextStoneSock;
	char messageHeaderBuffer[6];
	chainlist_packet packetInfo;

	if ( recv(previousStoneSock, messageHeaderBuffer, 6, 0) < 0 )
	{
		cleanExit(1, "Error: recv failed");
	}

	unsigned short sizeChainlist = -1;
	memcpy(&sizeChainlist, messageHeaderBuffer, 2);
	packetInfo.chainlistLength = ntohs(sizeChainlist);
	cout << "Chainlist Length: " << packetInfo.chainlistLength << endl;
	unsigned short sizeUrl = -1;
	memcpy(&sizeUrl, messageHeaderBuffer + 2, 2);
	packetInfo.urlLength = ntohs(sizeUrl);
	cout << "Url Length: " << packetInfo.urlLength << endl;

	unsigned short numStonesLeft = -1;
	memcpy(&numStonesLeft, messageHeaderBuffer + 4, 2);
	numStonesLeft = ntohs(numStonesLeft);
	packetInfo.numberChainlist = numStonesLeft;
	cout << "Number Addresses: " << packetInfo.numberChainlist << endl;

	char messageBuffer[packetInfo.urlLength + packetInfo.chainlistLength];
	if ( recv(previousStoneSock, messageBuffer, packetInfo.urlLength + packetInfo.chainlistLength, 0) < 0 )
	{
		cleanExit(1, "Error: recv failed");
	}

	char chainlist[packetInfo.chainlistLength];
	memcpy(chainlist, messageBuffer, packetInfo.chainlistLength);

	char url[packetInfo.urlLength];
	memcpy(url, messageBuffer + packetInfo.urlLength, sizeUrl);

	if(packetInfo.numberChainlist > 0)
	{
		vector<string> chainlistSplit = splitChainlistFromLastStone(chainlist);
		cout << "Chainlist is" << endl;
		for(int i = 0; i < numStonesLeft; i++)
		{
			cout << "<" << chainlistSplit.at(i) << ">" << endl;
		}
		int nextStoneIndex = selectRandomStoneIndex(numStonesLeft);
		string nextStone = chainlistSplit.at(nextStoneIndex);
		chainlistSplit.erase(chainlistSplit.begin() + nextStoneIndex);
		cout << "Next SS is <" << nextStone << ">" << endl << "waiting for file..." << endl;

		vector<string> nextStoneIpAndPort;
		split(nextStoneIpAndPort, nextStone, is_any_of(":"));
		nextStoneSock = connectToNextStone(nextStoneIpAndPort[0], stoi(nextStoneIpAndPort[1]));

		string combineChainlist("");
		for(int i = 0; i < chainlistSplit.size(); i++)
		{
			combineChainlist = combineChainlist + " " + chainlistSplit.at(i) ;
		}
		char chainlistPacket[packetInfo.urlLength + combineChainlist.length() + 6];
		memset(chainlistPacket, 0, packetInfo.urlLength + combineChainlist.length() + 6);

		unsigned short wrapChainlistSize = htons(packetInfo.chainlistLength - 1);
		memcpy(chainlistPacket, &wrapChainlistSize, 2);
		memcpy(chainlistPacket + 2, &sizeUrl, 2);
		unsigned short wrapNumber = htons(chainlistSplit.size());
		memcpy(chainlistPacket + 4, &wrapNumber, 2);
		memcpy(chainlistPacket + 6, combineChainlist.c_str(), combineChainlist.length());
		memcpy(chainlistPacket + 6 + combineChainlist.length(), url, packetInfo.urlLength);

		send(nextStoneSock, chainlistPacket, packetInfo.urlLength + combineChainlist.length() + 6, 0);

		bool fileTransfer = false;
		unsigned long fileSize = -1;
		unsigned short fileNameLength = -1;
		char fileMessageBuffer[6];
		if ( (recv(nextStoneSock, fileMessageBuffer, 6, 0) < 0) )
		{
			cleanExit(1, "Error: Failed to read file header");
		}

		memcpy(&fileSize, fileMessageBuffer, 4);
		fileSize = ntohl(fileSize);

		send(previousStoneSock, fileMessageBuffer, fileNameLength + 6, 0);
		if(fileSize == 0)
		{
			cleanExit(1, "Error: Something failed at the wget");
		}

		bool allPacketsTransferred = false;
		unsigned long recFileSize = 0;

		while(!allPacketsTransferred)
		{
			unsigned long packetSize = -1;
			char dataSizeBuffer[4];
			if ( (recv(nextStoneSock, dataSizeBuffer, 4, 0) < 0) )
			{
				cleanExit(1, "Error: Failed to read data size");
			}

			memcpy(&packetSize, dataSizeBuffer, 4);
                	packetSize = ntohl(packetSize);

			char data[packetSize];
			if ( (recv(nextStoneSock, data, packetSize, 0) < 0) )
			{
				cleanExit(1, "Error: Data read went wrong");
			}
			char dataWrapped[packetSize + 4];
			memset(dataWrapped, 0, packetSize + 4);
			unsigned long wrap = htonl(packetSize);
			memcpy(dataWrapped, &wrap, 4);
			memcpy(dataWrapped + 4, data, packetSize); 
			send(previousStoneSock, dataWrapped, packetSize + 4, 0);

			recFileSize += packetSize;
			cout << "Rec Packet! packet size: " << packetSize << ". Total " << recFileSize << " out of " << fileSize << endl;
			if (recFileSize == fileSize)
				allPacketsTransferred = true;
		}
		close(nextStoneSock);
	}
	else
	{
		FILE* file;
		unsigned long size;
		string requestUrl = createFinalRequestUrl(url);
		string fileName = parseFileName(requestUrl);

		sendSystemWget(requestUrl);

		file = fopen(fileName.c_str(), "r");
		if(file == NULL)
		{
			cleanExit(1, "Error: Unable to open file");
		}

		fseek(file, 0, SEEK_END);
		size = ftell(file);
		rewind(file);

		// Send file information
		char fileHeader[fileName.length() + 6];
		memset(fileHeader, 0, fileName.length() + 6);

		unsigned long wrapSize = htonl(size);
		memcpy(fileHeader, &wrapSize, 4);
		unsigned short wrapFileNameSize = htons(fileName.length());
		memcpy(fileHeader + 4, &wrapFileNameSize, 2);
		memcpy(fileHeader + 6, fileName.c_str(), fileName.length());

		send(previousStoneSock, fileHeader, fileName.length() + 6, 0);


		int bufferSize = 10000;
		char dataRead[bufferSize];
		unsigned short bytesRead;
		while( (bytesRead = fread(dataRead, 1, bufferSize, file)) > 0)
		{
			// Wrap message size then send
			char fileWrapped[bytesRead + 2];
			memset(fileWrapped, 0, bytesRead + 2);
			unsigned short wrap = htons(bytesRead);
			memcpy(fileWrapped, &wrap, 2);
			memcpy(fileWrapped + 2, dataRead, bytesRead);

			send(previousStoneSock, fileWrapped, bytesRead + 2, 0);
		}

		fclose(file);
		cout << "Finished transmitting file" << endl;
		cout << "Removing " << fileName << endl;
		system( ("rm " + fileName).c_str() );
	}

	close(previousStoneSock);
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

	char hostname[256];

	if(gethostname(hostname, sizeof hostname) != 0)
	{
		string errorMessage("Error: Could not find hostname");
		cleanExit(1, errorMessage);
	}
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = htons(INADDR_ANY);
	sin.sin_port = htons(port);

	// I modified code found from beej guide that showed how to get ip from hostname
    	struct addrinfo hints, *res, *p;
    	char ipstr[INET_ADDRSTRLEN];

    	memset(&hints, 0, sizeof hints);
    	hints.ai_family = AF_INET; 
    	hints.ai_socktype = SOCK_STREAM;
    	getaddrinfo(hostname, NULL, &hints, &res);

    	for(p = res;p != NULL; p = p->ai_next) 
    	{
        	if (p->ai_family == AF_INET) 
		{
           	 	struct sockaddr_in *ip = (struct sockaddr_in *)p->ai_addr;
            		void* addr = &(ip->sin_addr);
            		inet_ntop(AF_INET, &(ip->sin_addr), ipstr, sizeof ipstr);
            		break;
		}
    	}
  	freeaddrinfo(res);

	cout << "Stepping Stone " << ipstr << ":" << port << endl;

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
		string ipString(ipstr);
		char remoteIp[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(clientAddr.sin_addr), remoteIp, INET_ADDRSTRLEN);
		cout << "Connection from: " << remoteIp << ":" << ntohs(clientAddr.sin_port) << endl;
		handleConnectionThread(incomingSock);
		//thread ssSockThread(handleConnectionThread, incomingSock);
		//ssSockThread.join();
	}

    	return 0;
}
