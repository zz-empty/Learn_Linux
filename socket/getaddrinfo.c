#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <hostname>\n", argv[0]);
        return 1;

    }

    struct addrinfo hints, *res, *p;
    char ipstr[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;      
    hints.ai_socktype = SOCK_STREAM;  

    int status = getaddrinfo(argv[1], NULL, &hints, &res);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 2;

    }

    printf("IP addresses for %s:\n\n", argv[1]);
    for(p = res; p != NULL; p = p->ai_next) {
        void *addr;
        const char *ipver;

        if (p->ai_family == AF_INET) { 
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr = &ipv4->sin_addr;
            ipver = "IPv4";

        } else { 
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            addr = &ipv6->sin6_addr;
            ipver = "IPv6";

        }

        inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
        printf("  %s: %s\n", ipver, ipstr);

    }

    freeaddrinfo(res);
    return 0;

}
