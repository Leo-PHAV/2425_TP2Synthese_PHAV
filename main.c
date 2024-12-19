#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stddef.h>
#include <netdb.h>

#include "constHeader.h"

int main (int argc, char *argv[]){
    struct addrinfo hints;
    struct addrinfo *res,*r;

    char *host = argv[1];
    char *file = argv[2];

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET; 
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    int status = getaddrinfo(argv[1],NULL,&hints,&res);
    if (status != 0){
        fprintf(stderr,"%s\n",gai_strerror(status));
        exit(EXIT_FAILURE);  
    }

    for (r = res; r != NULL; r=r->ai_next){
        char buffer[128];  
        int text;
        text = snprintf(buffer, 100,"Family = %d \nSocketType = %d \nProtocol = %d\n",r->ai_family,r->ai_socktype,r-> ai_protocol);
        write(STDOUT_FILENO, buffer, text);
        break;
    }
    freeaddrinfo(res);
    exit(EXIT_SUCCESS);

   
    return 0;
}
