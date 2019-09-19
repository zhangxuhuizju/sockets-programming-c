#ifndef PRINTSOCKETADDR
#define PRINTSOCKETADDR

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
void PrintSocketAddress(const struct sockaddr *address, FILE *stream) {
    if (address == NULL || stream == NULL)
        return;
    
    void *numericAddress;  //pointer to binary address
    char addrBuffer[INET6_ADDRSTRLEN];
    in_port_t port;  //port to print;
    switch (address->sa_family)
    {
    case AF_INET:
        numericAddress = &((struct sockaddr_in *)address)->sin_addr;
        port = ntohs(((struct sockaddr_in *) address)->sin_port);
        break;
    case AF_INET6:
        numericAddress = &((struct sockaddr_in6 *)address)->sin6_addr;
        port = ntohs(((struct sockaddr_in6 *) address)->sin6_port);
        break;
    default:
        fputs("[invalid address]", stream);
        break;
    }
    if (inet_ntop(address->sa_family, numericAddress, addrBuffer, sizeof(addrBuffer)) == NULL)
        fputs("[invalid address]", stream);
    else {
        fprintf(stream, "%s", addrBuffer);
        if (port != 0)
            fprintf(stream, "-%u", port);
    }
}

#endif