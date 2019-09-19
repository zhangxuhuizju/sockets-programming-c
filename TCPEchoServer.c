#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "DieWithMessage.c"
#include "PrintSocketAddr.c"
#include "TCPClientUtility.c"

//static const int MAXPENDING = 5;

void HandleTCPClient(int clntSocket) {
    char buffer[BUFSIZ];
    __ssize_t numBytesRcvd = recv(clntSocket, buffer, BUFSIZ, 0);

    while (numBytesRcvd > 0) {
        __ssize_t numBytesSent = send(clntSocket, buffer, numBytesRcvd, 0);

        numBytesRcvd = recv(clntSocket, buffer, BUFSIZ, 0);
    }
   
    close(clntSocket);
}

int AcceptTCPConnection(int servSock) {
    struct sockaddr_storage clntAddr;
    socklen_t clntAddrLen = sizeof(clntAddr);

    int clntSock = accept(servSock, (struct sockaddr *) &clntAddr, &clntAddrLen);
    if (clntSock < 0)
        DieWithSystemMessage("accept() failed!");

    fputs("Handling client ", stdout);
    PrintSocketAddress((struct sockaddr*) &clntAddr, stdout);
    fputc('\n', stdout);

    return clntSock;
}


int main(int argc, char const *argv[])
{
    if (argc != 2)
        DieWithUserMessage("Parameters(s)", "<Server Port>");
    
    const char* service = argv[1];

    //create socket for incoming connections
    int servSock = SetupTCPServiceSocket(service);
    if (servSock < 0)
         DieWithSystemMessage("Socket() failed");
    
    //construct local address structure
    // struct sockaddr_in servAddr;
    // memset(&servAddr, 0, sizeof(servAddr));
    // servAddr.sin_family = AF_INET;
    // servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    // servAddr.sin_port = htons(servPort);

    // if (bind(servSock, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0)
    //     DieWithSystemMessage("bind() failed");
    
    // if (listen(servSock, MAXPENDING) < 0)
    //     DieWithSystemMessage("listen() failed");
    
    for(;;) {
        // struct sockaddr_in clntAddr;
        // socklen_t clntAddrLen = sizeof(clntAddr);

        // int clntSock = accept(servSock, (struct sockaddr *) &clntAddr, &clntAddrLen);
        // if (clntSock < 0)
        //     DieWithSystemMessage("accept() failed!");
        
        // char clntName[INET_ADDRSTRLEN];
        // if (inet_ntop(AF_INET, &clntAddr.sin_addr.s_addr, clntName, sizeof(clntName)) != NULL)
        //     printf("Handling client %s %d\n", clntName, ntohs(clntAddr.sin_port));
        // else 
        //     puts("unable to get client address");
        int clntSock = AcceptTCPConnection(servSock);
        HandleTCPClient(clntSock);
        close(clntSock);
    }
    return 0;
}
