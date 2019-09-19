#ifndef TCPCLIENTUTIITY
#define TCPCLIENTUTIITY

#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "DieWithMessage.c"
#include "PrintSocketAddr.c"

int SetupTCPClientSocket(const char *host, const char *service) {
    struct addrinfo addrCriteria;
    memset(&addrCriteria, 0, sizeof(addrCriteria));
    addrCriteria.ai_family = AF_UNSPEC;
    addrCriteria.ai_socktype = SOCK_STREAM;
    addrCriteria.ai_protocol = IPPROTO_TCP;

    struct addrinfo *servAddr;
    int rtnval = getaddrinfo(host, service, &addrCriteria, &servAddr);
    if (rtnval != 0)
        DieWithUserMessage("getaddrinfo() failed", gai_strerror(rtnval));

    int sock = -1;
    for (struct addrinfo *addr = servAddr; addr != NULL; addr = addr->ai_next) {
        sock = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
        if (sock < 0)
            continue;
        if (connect(sock, addr->ai_addr, addr->ai_addrlen) == 0)
            break;
        close(sock);
        sock = -1;
    }
    freeaddrinfo(servAddr);
    return sock;
}

static const int MAXPENDING = 5;

int SetupTCPServiceSocket(const char *service) {
    struct addrinfo addrCriteria;
    memset(&addrCriteria, 0, sizeof(addrCriteria));
    addrCriteria.ai_family = AF_UNSPEC;
    addrCriteria.ai_flags = AI_PASSIVE;  //accept on any address
    addrCriteria.ai_socktype = SOCK_STREAM;
    addrCriteria.ai_protocol = IPPROTO_TCP;

    struct addrinfo *servAddr;
    int rtnval = getaddrinfo(NULL, service, &addrCriteria, &servAddr);
    if (rtnval != 0)
        DieWithUserMessage("getaddrinfo() failed", gai_strerror(rtnval));
    
    int servSock = -1;
    for (struct addrinfo *addr = servAddr; addr != NULL; addr = addr->ai_next) {
        servSock = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
        if (servSock < 0)
            continue;
        if ((bind(servSock, addr->ai_addr, addr->ai_addrlen) == 0)
            && (listen(servSock, MAXPENDING) == 0) ) {
            struct sockaddr_storage localAddr;
            socklen_t addrSize = sizeof(localAddr);
            if (getsockname(servSock, (struct sockaddr *) &localAddr, &addrSize) < 0)
                DieWithSystemMessage("getsockname() failed");
            fputs("Binding to ", stdout);
            PrintSocketAddress((struct sockaddr *) &localAddr, stdout);
            fputc('\n', stdout);
            break;
        }
        
        close(servSock);
        servSock = -1;
    }
    freeaddrinfo(servAddr);
    return servSock;
}

#endif