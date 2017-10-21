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

//----------COLLEEN---------------------------------------------------------
#define FILENAME "index.html"
#define MIN(a,b) (((a)(b))?(a):(b))

int minimum(int a, int b)
{
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
    //int sockfd;
    char *UrlBuf;
    //Retrieve URL information from the command line argument
    int urlSize;
    char *tok;
    size_t lineLen;
    char *line;
    size_t read;
    
    urlSize=sizeof(argv[1]);
    int urlBufSize;
    //Store the Url in a buffer for data packing later
    UrlBuf=malloc((sizeof(char)*urlSize)+1);
    if(NULL!=UrlBuf){
        //copy UrlInfo to Buff: Required later to pack data
        strcpy(UrlBuf,argv[1]);
    }
    //Debug
    // printf("\n %s", UrlBuf);
    
    //For second argument
    FILE *fp;
    
    //-----COLLEEN: STEPS 1, 2, 4, 5, 6 and 7 OF CS457Project2_2017.pdf (pages 7 and 8)-----
    //a chain file was passed in
    if(argc == 3)
    {
        fp = fopen(argv[2],"r");
    }
    //a chainfile has not been passed in
    else
    {
        fp=fopen("chaingang.txt", "r");
        
        if(fp == NULL)
        {
            printf("ERROR: awget failed to locate chainfile.txt");
            exit(0);
        }
    }
    //-------------------------------------------------------------------------------------
    
    //count the number of IP addresses and Ports in the text file
    int numOfAddr=0;
    char ch;
    while(!feof(fp)){
        ch=fgetc(fp);
        if(ch=='\n'){
            numOfAddr=numOfAddr+1;
        }//end if
    }//end of while line count
    //Debug
    //  printf("\n Num of IP addresses=%d", numOfAddr);
    fclose(fp);
    //Generate a random number between 0 and numOfAddr;
    int randIP;
    randIP=rand()%numOfAddr;
    //Debug
    // printf("\n Random IP address=%d", randIP);
    
    //Extarct the IP address and Port numner at randIP
    lineLen=256;
    int i;
    char tempLine[256];//required later
    line=malloc((sizeof(char)*lineLen)+1);
    fp=fopen(argv[2],"r");
    if(NULL!=fp) {
        for(i=0;i<randIP;i++) {
            read=getline(&line, &lineLen,fp);
            strcpy(tempLine, line);
        }
    }
    
    //Extract IP address and the port number from the line
    char *IP4, *port;
    unsigned int intPort;
    IP4=strtok(line," ");
    
    //-----COLLEEN: WHEN I ADD MY CODE TO YOURS THERE IS A SEG FAULT ON LINE 121)----
    
    port=strtok(NULL, " ");
    //printf("\n IP4=%s", IP4);
    //printf("\n Port=%s \n", port);
    intPort=atoi(port);
    
    //printf("\n IntPort=%d", intPort);
    fclose(fp);
    //Pack IP address in to string
    char *AddrBuf;
    int sizeAddrBuf;
    struct stat st;
    if(stat(argv[2],&st)==0) {
        ;
        //printf("\n File Size=%d",st.st_size);
    }
    sizeAddrBuf=(st.st_size)*2;  //Extra Space;
    //Allocate memory to
    AddrBuf=malloc((sizeof(char)*sizeAddrBuf)+1);
    if(NULL==AddrBuf) {
        printf("\n Failed to allocate memory to AddrBuf");
        exit(0);
    } //End If
    //clear memory
    
    //memset(&AddrBuf, 0, (sizeof(char)*sizeAddrBuf));
    //unpack IP and Port
    i=0;
    char*getLine;
    getLine=malloc((sizeof(char)*lineLen)+1);
    fp=fopen(argv[2],"r");
    while(i<numOfAddr){
        //read the line
        read=getline(&getLine, &lineLen,fp);
        //  printf("\n i=%d", i);
        //  printf("\n Line=%s: getline=%s", tempLine, getLine);
        //  NOT A CLEAN CODE: NEED TO CLEAN IT UP
        if(NULL!=getLine){
            if(strcmp(getLine, tempLine)!=0){
                //      printf("\n Not equal");
                //extract IP4 and port
                IP4=strtok(getLine, " ");
                port=strtok(NULL, " ");
                if(i==0){strcpy(AddrBuf,IP4);}
                else{strcat(AddrBuf, IP4);}
                strcat(AddrBuf, ":");
                strcat(AddrBuf, port);
                //    sprintf(AddrBuf, "%s, %s", IP4,port);
                
            }//end if*/
        }
        i++;
    }//END while */
    //Create the socket and connect with the client//
    int totalSize;
    char *sendBuf;
    totalSize=((sizeof(int)*2)+((strlen(AddrBuf))+strlen(UrlBuf))*sizeof(char));
    
    sendBuf=(char*)malloc(totalSize+1);
    if(NULL==sendBuf) {
        printf("\n Failed to allocate the memory");
        exit(0);
    }
    
    //----------COLLEEN: We need to send URL and chainfile to the server-----------
    
    //  //pack the data;
    //   sprintf(sendBuf, "%d, %s, %d, %s", strlen(AddrBuf),AddrBuf, strlen(UrlBuf), UrlBuf);
    //  puts(sendBuf);
    //  int sockfd;
    //   struct socketAddressIN serverAddr;
    //   socklen_t addr_size;
    //   //Create socket
    //   sockfd=socket(AF_INET, SOCK_STREAM, 0);
    //  if(sockfd<0) {
    //     printf("\n failed to create the socket");
    //     exit(0);
    //   }
    //   bzero((char *) &serverAddr, sizeof(serverAddr));
    //   //Configure server settings//
    //   serverAddr.sin_family = AF_INET;
    //   //set the port number//
    //   serverAddr.sin_port = htons(intPort);
    //   //set the IP address//
    //   serverAddr.sin_addr.s_addr = inet_addr(IP4);
    //   //Connet to the client
    //  /*if(connect(sockfd, (struct sockaddr *) &serverAddr, sizeof(serverAddr))<0){
    //      printf("\n Error connecting to server");
    //      exit(0);
    //   }*/
    //
    //  //read the content of file
    
    //parse out the filename from the URL and if it is NULL, then the filename is == index.html
    
    //write(sockfd,sendBuf,sizeof(sendBuf));
    
    
    //---COLLEEN-STEPS 12, 13, 14 and 15 OF CS457Project2_2017.pdf (pages 7 and 8)---------
    char filename[256];
    int clientSocket;
    ssize_t length;
    struct socketAddressIN remoteAddress;
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
    inet_pton(AF_INET, IP4, &(remoteAddress.sin_addr));
    remoteAddress.sin_port = htons(intPort);
    
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
