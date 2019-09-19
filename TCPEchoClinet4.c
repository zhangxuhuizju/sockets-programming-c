#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "DieWithMessage.c"

int main(int argc, char const *argv[])
{
    if (argc < 3 || argc > 4)
        DieWithUserMessage("Parameter(s)", 
        "<Server Address> <Echo Word> [<Server Port>]");
    
    char *servIP = argv[1];
    char *echoString = argv[2];

    //third arg is optional, 7 is well-known echo port
    in_port_t servPort = (argc == 4) ? atoi(argv[3]) : 7;

    //creat a reliable, stream socket using TCP
    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0)
        DieWithSystemMessage("socket() failed");
    
    //construct the server address struct
    struct sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;  //ipv4 address family
    //convert address
    int rthVal = inet_pton(AF_INET, servIP, &servAddr.sin_addr.s_addr);
    if (rthVal == 0)
        DieWithUserMessage("inet_pton() failed","invalid address string");
    else if (rthVal < 0)
        DieWithSystemMessage("inet_pton() failed");
    servAddr.sin_port = htons(servPort);

    //establish the connection to the echo server
    if (connect(sock, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0)
        DieWithSystemMessage("connect failed");
    
    size_t echoStringLen = strlen(echoString);

    //send string to the server
    ssize_t numBytes = send(sock, echoString, echoStringLen, 0);
    if (numBytes < 0)
        DieWithSystemMessage("send() failed");
    else if (numBytes != echoStringLen)
        DieWithUserMessage("send()","send unexpected number of bytes");
    
    //receive the same string back from the server
    unsigned int totalBytesRcvd = 0;
    fputs("Received: ", stdout);
    while (totalBytesRcvd < echoStringLen){
        char buffer[BUFSIZ];
        numBytes = recv(sock, buffer, BUFSIZ - 1, 0);
        if (numBytes < 0)
            DieWithSystemMessage("receive() failed");
        else if (numBytes == 0)
            DieWithUserMessage("recv()", "connection closed prematurely");
        totalBytesRcvd += numBytes;
        buffer[numBytes] = '\0';
        fputs(buffer, stdout);
    }
    
    fputc('\n', stdout);

    close(sock);
    return 0;
}
