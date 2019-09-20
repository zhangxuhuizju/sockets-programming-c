#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "../DieWithMessage.c"
#include "../PrintSocketAddr.c"

int main(int argc, char const *argv[])
{
    if (argc != 2)
        DieWithUserMessage("Parameter(s)", "<Server Port/Service>");
    
    const char *service = argv[1];
    
    struct addrinfo addrCriteria;
    memset(&addrCriteria, 0, sizeof(addrCriteria));
    addrCriteria.ai_family = AF_UNSPEC;
    addrCriteria.ai_flags = AI_PASSIVE;  //accept on any address
    addrCriteria.ai_socktype = SOCK_DGRAM;
    addrCriteria.ai_protocol = IPPROTO_UDP;

    struct addrinfo *servAddr;
    int rtnval = getaddrinfo(NULL, service, &addrCriteria, &servAddr);
    if (rtnval != 0)
        DieWithUserMessage("getaddrinfo() failed", gai_strerror(rtnval));
    
    int sock = socket(servAddr->ai_family, servAddr->ai_socktype, servAddr->ai_protocol);
    if (sock < 0)
        DieWithSystemMessage("socket() failed");
    
    if (bind(sock, servAddr->ai_addr, servAddr->ai_addrlen) < 0)
        DieWithSystemMessage("bind() failed");
    
    freeaddrinfo(servAddr);
    for(;;) {
        struct sockaddr_storage clntAddr;
        socklen_t clntAddrLen = sizeof(clntAddr);

        char buffer[100000];
        ssize_t numBytesRcvd = recvfrom(sock, buffer, 100000, 0, (struct sockaddr*) &clntAddr, &clntAddrLen);
        if (numBytesRcvd < 0)
            DieWithSystemMessage("recvfrom() failed");
        
        fputs("Handling client: ", stdout);
        PrintSocketAddress((struct sockaddr*) &clntAddr, stdout);
        fputc('\n', stdout);

        ssize_t numbytesSend = sendto(sock, buffer, numBytesRcvd, 0, (struct sockaddr*) &clntAddr, sizeof(clntAddr));
        if (numbytesSend < 0)
            DieWithSystemMessage("sendto() failed");
        else if (numBytesRcvd != numbytesSend)
            DieWithUserMessage("sendto()", "sent unexpected number of bytes");
    }
    return 0;
}
