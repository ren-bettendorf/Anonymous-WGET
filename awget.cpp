#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <errno.h> 
#include <string.h> 
#include <netdb.h> 
#include <sys/types.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <sys/stat.h> 
#include <arpa/inet.h> 
#include <ctype.h> 
#include "awget.h"
#include <vector>
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <fstream>

using namespace boost;
using namespace std;

//--------------------------------------------------------------------------- 
int main(int argc, char *argv[]) 
{
    	string url;

    	//-----COLLEEN: STEPS 1, 2, 4, 5, 6 and 7 OF CS457Project2_2017.pdf (pages 7 and 8)-----
    	//a chain file was passed in
	if(argc > 1)
		url = argv[1];
	string file;
    	if(argc == 4)
    	{
		if(strcmp(argv[2], "-c") != 0)
		{
			cout << "Error: Not correct startup." << endl;
			cout << "Expected: ./awget <URL> [-c chainfile]" << endl;
			exit(0);
		}
		file = argv[3];
    	}
    	//a chainfile has not been passed in
    	else
    	{
		file = "chaingang.txt";
    	}

    	//-------------------------------------------------------------------------------------
    	ifstream read(file);
	if ( read.bad() )
	{
		cout << "ERROR: awget failed to located chainfile" << endl;
		exit(0);
	}
	cout << "Request: " << url << endl;
    	//count the number of IP addresses and Ports in the text file
	string chainLine;
	vector<string> chainLines;
	getline(read, chainLine);
	unsigned short numOfAddr = stoi(chainLine);
	cout << "Chainlist is " << endl;
    	while(getline(read, chainLine)){
		cout << "<" << chainLine << ">" << endl;
		chainLine = chainLine.replace(chainLine.find(" "), 1, ":");
		chainLines.push_back(chainLine);
    	}
	cout << numOfAddr << endl;
    	read.close();

	cout << numOfAddr << endl;
	for(int i = 0; i < chainLines.size(); i++)
	{
		cout << chainLines.at(i) << endl;
	}
   	//Generate a random number between 0 and numOfAddr;
    	srand(time(NULL));
	int randIP = rand()%numOfAddr;
    	//Debug
    	// printf("\n Random IP address=%d", randIP);

    	//Extract IP address and the port number from the line

	vector<string> ipAndPort;
	string nextStone = chainLines.at(randIP);
	cout << "Next SS is <" << nextStone << ">" << endl;

	chainLines.erase(chainLines.begin() + randIP);
	split(ipAndPort, nextStone, is_any_of(":"));
	string IP4 = ipAndPort[0];
	int port =  stoi(ipAndPort[1]);
    	string chainlistStr("");
	for(int i = 0; i < chainLines.size(); i++)
	{
		chainlistStr = chainlistStr + " " + chainLines.at(i);
	}
	chainlistStr += " ";
	url += " ";
	//-----COLLEEN: WHEN I ADD MY CODE TO YOURS THERE IS A SEG FAULT ON LINE 121)--
    	//Create the socket and connect with the client//
    	char sendHeader[6];
    	char sendBuf[chainlistStr.length() + url.length() + 2];
	memset(sendHeader, 0, 6);
	memset(sendBuf, 0, chainlistStr.length() + url.length() + 2);

	unsigned short wrap = htons(chainlistStr.length() + 1);
    	memcpy(sendHeader, &wrap, 2);
	unsigned short wrapAgain = htons(url.length() + 1);
    	memcpy(sendHeader + 2, &wrapAgain, 2);
   	unsigned short wrapThrice = htons(numOfAddr - 1); 
   	memcpy(sendHeader + 4, &wrapThrice, 2);

   	memcpy(sendBuf, chainlistStr.c_str() + '\0', chainlistStr.length() + 1);
   	memcpy(sendBuf + chainlistStr.length() + 1, url.c_str() + '\0', url.length() + 1);
    	//----------COLLEEN: We need to send URL and chainfile to the server-----------

    	//pack the data;
      	// printf("PACKET: %hu, %hu, %hu, %s, %s",strlen(AddrBuf),strlen(UrlBuf),(numOfAddr-1),AddrBuf, UrlBuf);
	//      puts(sendBuf);
    	// int sockfd;
    	// struct socketAddressIN serverAddr;
    	// socklen_t addr_size;
    	// //Create socket
    	// sockfd=socket(AF_INET, SOCK_STREAM, 0);
    	// if(sockfd<0) {
    	// printf("\n failed to create the socket");
    	// exit(0);
    	// }
    	// bzero((char *) &serverAddr, sizeof(serverAddr));
    	// //Configure server settings//
    	// serverAddr.sin_family = AF_INET;
    	// //set the port number//
    	// serverAddr.sin_port = htons(intPort);
    	// //set the IP address//
    	// serverAddr.sin_addr.s_addr = inet_addr(IP4);
    	// //Connet to the client
    	// /*if(connect(sockfd, (struct sockaddr *) &serverAddr, sizeof(serverAddr))<0){
    	// printf("\n Error connecting to server");
    	// exit(0);
    	// }*/
   	//
    	// //read the content of file

    	//parse out the filename from the URL and if it is NULL, then the filename is == index.html

    	//write(sockfd,sendBuf,sizeof(sendBuf));


    	//---COLLEEN-STEPS 12, 13, 14 and 15 OF CS457Project2_2017.pdf (pages 7 and 8)---------
    	char filename[256];
    	int clientSocket;
    	struct sockaddr_in remoteAddress;
   	FILE *recFile;

    	//remoteAddress
    	memset(&remoteAddress, 0, sizeof(remoteAddress));

    	remoteAddress.sin_family = AF_INET;
    	remoteAddress.sin_addr.s_addr=inet_addr(IP4.c_str());
    	remoteAddress.sin_port = htons(port);

    	//create socket
    	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    	if (clientSocket == -1)
    	{
        	cout << "ERROR: CREATING THE SOCKET" << endl;
        	exit(0);
    	}
    	//connect
    	if (connect(clientSocket, (struct sockaddr *)&remoteAddress, sizeof(struct sockaddr)) == -1)
    	{
        	cout << "ERROR: CONNECTING TO THE SOCKET" << endl;
        	exit(0);
    	}

	if ( send(clientSocket, sendHeader, 6, 0) < 0)
	{
		cout << "ERROR: BAD SEND" << endl;
		exit(0);
	}

   	if( send(clientSocket, sendBuf, url.length() + chainlistStr.length() + 2, 0) < 0 )
	{
		cout << "ERROR: BAD SEND" << endl;
		exit(0);
	}

    	//receive file size
	bool fileTransfer = false;
	unsigned long fileSize = -1;
	unsigned short fileNameLength = -1;
	char fileHeader[6];
	cout << "Waiting for file..." << endl;
	if ( (recv(clientSocket, fileHeader, 6, 0) < 0) )
	{
		exit(1);
	}

	memcpy(&fileSize, fileHeader, 4);
	fileSize = ntohl(fileSize);
	memcpy(&fileNameLength, fileHeader + 4, 2);
	fileNameLength = ntohs(fileNameLength);

	char fileNameRecv[fileNameLength];
	recv(clientSocket, fileNameRecv, fileNameLength, 0);

	cout << "File Name: <" << fileNameRecv << ">" << endl;
	string fileName(fileNameRecv);
	trim(fileName);
	cout << "File Name: <" << fileName << ">" << endl;
	if(fileSize == 0)
	{
		exit(1);
	}

	bool allPacketsTransferred = false;
	unsigned long recFileSize = 0;
	recFile = fopen(fileName.c_str(), "wb");
	cout << "Begin transmission from last stone" << endl;
	while(!allPacketsTransferred)
	{
		unsigned short packetSize = -1;
		char dataSizeBuffer[2];
		if ( (recv(clientSocket, dataSizeBuffer, 2, 0) < 0) )
		{
			exit(1);
		}

		memcpy(&packetSize, dataSizeBuffer, 2);
               	packetSize = ntohs(packetSize);

		char data[packetSize];
		if ( (recv(clientSocket, data, packetSize, MSG_WAITALL) < 0) )
		{
			exit(1);
		}
		fwrite(data, packetSize, 1, recFile);
		recFileSize += packetSize;
		cout << "Rec Packet! packet size: " << packetSize << ". Total " << recFileSize << " out of " << fileSize << endl;
		if (recFileSize == fileSize)
			allPacketsTransferred = true;
	}
    	fclose(recFile);
    	close(clientSocket);
	cout << "Received file: " << fileName << endl;
	cout << "Goodbye!" << endl;
    	return 0;

    	//------------------------------------------------------------------------------------
}

