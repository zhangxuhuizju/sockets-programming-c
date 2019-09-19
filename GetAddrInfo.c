#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include "DieWithMessage.c"
#include "PrintSocketAddr.c"
#include <sys/types.h>
#include <sys/socket.h>

int main(int argc, char const *argv[])
{
    if (argc != 3)
        DieWithUserMessage("Parameter(s)", "<Address/Name> <Port/Service>");
    
    char *addrString = argv[1];
    char *portString = argv[2];

    //tell the system which kind of the info we want
    struct addrinfo addrCritical;
    memset(&addrCritical, 0, sizeof(addrCritical));
    addrCritical.ai_family = AF_UNSPEC;   //any address family
    addrCritical.ai_socktype = 0;  //only stream sockets
    addrCritical.ai_protocol = 0;

    //get address associated with the specified name/service
    struct addrinfo *addrList;   //hold for the of addresses returned
    int rtnVal = getaddrinfo(addrString, portString, &addrCritical, &addrList);
    if (rtnVal != 0)
        DieWithUserMessage("getaddrinfo() failed", gai_strerror(rtnVal));
    
    //Display returned addresses
    for (struct addrinfo *addr = addrList; addr != NULL; addr = addr->ai_next) {
        PrintSocketAddress(addr->ai_addr, stdout);
        fputc('\n', stdout);
    }
    freeaddrinfo(addrList);
    return 0;
}
