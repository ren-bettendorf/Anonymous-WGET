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

//----------COLLEEN---------------------------------------------------------
#define FILENAME "index.html" 
#define MIN(a,b) (((a)(b))?(a):(b))


int minimum(int a, int b) {
    if(a > b)
    {
        return b;
    }
    else
    {
        return a;
    }
}
//--------------------------------------------------------------------------- 
int main(int argc, char *argv[]) 
{
    string url;
    //For second argument
    
    //-----COLLEEN: STEPS 1, 2, 4, 5, 6 and 7 OF CS457Project2_2017.pdf (pages 7 and 8)-----
    //a chain file was passed in
	url = argv[1];
	string file;
    if(argc == 3)
    {
	file = argv[2];
    }
    //a chainfile has not been passed in
    else
    {
	file = "chaingang.txt";
        if(read == NULL)
        {
            printf("ERROR: awget failed to locate chainfile.txt");
            exit(0);
        }
    }
    //-------------------------------------------------------------------------------------
    ifstream read(file);
    //count the number of IP addresses and Ports in the text file
    	unsigned short numOfAddr=0;
	string chainLine;
	vector<string> chainLines;
    	while(getline(read, chainLine)){
		chainLine = chainLine.replace(chainLine.find(" "), 1, ":");
		chainLines.push_back(chainLine);
		cout << "Adding " << chainLine << endl;
    	}//end of while line count
	numOfAddr = chainLines.size();
    //Debug
    // printf("\n Num of IP addresses=%d", numOfAddr);
    	read.close();
	cout << numOfAddr << endl;
	for(int i = 0; i < chainLines.size(); i++)
	{
		cout << chainLines.at(i) << endl;
	}
    //Generate a random number between 0 and numOfAddr;
    int randIP = rand()%numOfAddr;
    //Debug
    // printf("\n Random IP address=%d", randIP);

    //Extract IP address and the port number from the line

	vector<string> ipAndPort;
	cout << "Random Ip: " << randIP << endl;
	string nextStone = chainLines.at(randIP);
	cout << "First " << nextStone << endl;
	chainLines.erase(chainLines.begin() + randIP);
	cout << "Erased" << endl;
	split(ipAndPort, nextStone, is_any_of(":"));
	string IP4 = ipAndPort[0];
	int port =  stoi(ipAndPort[1]);
    	string chainlistStr("");
	for(int i = 0; i < chainLines.size(); i++)
	{
		chainlistStr = chainlistStr + " " + chainLines.at(i);
	}
	cout << "Should be 1 address" << endl;
	cout << "Chainlist: " << chainlistStr << endl;
	trim(chainlistStr);
	cout << "Trimmed: " << chainlistStr << endl;
	//-----COLLEEN: WHEN I ADD MY CODE TO YOURS THERE IS A SEG FAULT ON LINE 121)----
    //printf("\n IP4=%s", IP4);
    //printf("\n Port=%s \n", port)
    //printf("\n IntPort=%d", intPort);
 
    //Create the socket and connect with the client//
	cout << "Size: " << 6 + chainlistStr.length() + url.length() << endl;
    char sendHeader[6];
    char sendBuf[chainlistStr.length() + url.length()];
	memset(sendHeader, 0, 6);
	memset(sendBuf, 0, chainlistStr.length() + url.length());

	unsigned short wrap = htons(chainlistStr.length());
    memcpy(sendHeader, &wrap, 2);
	unsigned short wrapAgain = htons(url.length());
    memcpy(sendHeader + 2, &wrapAgain, 2);
   	unsigned short wrapThrice = htons(numOfAddr - 1); 
   memcpy(sendHeader + 4, &wrapThrice, 2);
   memcpy(sendBuf, chainlistStr.c_str(), chainlistStr.length());
   memcpy(sendBuf + chainlistStr.length(), url.c_str(), url.length());
    //----------COLLEEN: We need to send URL and chainfile to the server-----------
    
    // //pack the data;
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
    ssize_t length;
    struct sockaddr_in remoteAddress;
    char buffer[BUFSIZ];
    int fileSize;
    FILE *receivedFile;
    int remaining = 0;
    
    //for testing purposes
    strcpy(filename, FILENAME);
    
    memset(buffer, 0, BUFSIZ);
    
    //remoteAddress
    memset(&remoteAddress, 0, sizeof(remoteAddress));
    
    remoteAddress.sin_family = AF_INET;
    remoteAddress.sin_addr.s_addr=inet_addr(IP4.c_str());
    //inet_pton(AF_INET, IP4, &(remoteAddress.sin_addr));
    remoteAddress.sin_port = htons(port);
    
    //create socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1)
    {
        printf("ERROR: CREATING THE SOCKET");
        exit(0);
    }
    
    //connect
    if (connect(clientSocket, (struct sockaddr *)&remoteAddress, sizeof(struct sockaddr)) == -1)
    {
        printf("ERROR: CONNECTING TO THE SOCKET");
        exit(0);
   
    }
    //send packet to
   int sendBytes;
	sendBytes = send(clientSocket, sendHeader, 6, 0);
	cout << "Sendbytes: " << sendBytes << endl;
   sendBytes=send(clientSocket, sendBuf, url.length() + chainlistStr.length(), 0);
   cout << "sendBytes: " << sendBytes << endl;;
    //receive file size
    recv(clientSocket, buffer, BUFSIZ, 0);
    fileSize = atoi(buffer);
    
    receivedFile = fopen(FILENAME, "w");
    if (receivedFile == NULL)
    {
        printf("ERROR: CANNOT OPEN THE FILE");
        exit(0);
    }
    
    remaining = fileSize;
    
    while(((length = recv(clientSocket, buffer, BUFSIZ, 0)) > 0) && (remaining > 0))
    {
        fwrite(buffer, sizeof(char), minimum(remaining, BUFSIZ), receivedFile);
        remaining -= length;
    }
    fclose(receivedFile);
    close(clientSocket);
    return 0;
    
    //------------------------------------------------------------------------------------
}

