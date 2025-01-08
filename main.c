#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>

#include "constHeader.h"

int main(int argc, char ** argv) {
    printf("TFTP Client \n");

    if (argc != 4){
        printf("Wrong command \n");
        exit(EXIT_FAILURE);
    }

    struct addrinfo hints;
    struct addrinfo * result;
    memset(&hints,0,sizeof(struct addrinfo));

    hints.ai_family = AF_INET;
    hints.ai_protocol = IPPROTO_UDP;

    int status = getaddrinfo(argv[1],argv[2],&hints,&result);

    char ip_string[128]={0};
    char port_string[128]={0};

    getnameinfo(result->ai_addr,result->ai_addrlen,
                ip_string,128,port_string,128,
                NI_NUMERICHOST|NI_NUMERICSERV);

    printf("%s is resolved at : %s:%s \n",argv[1],ip_string,port_string);

    int sd = socket(result->ai_family,result->ai_socktype,
                    result->ai_protocol);

    char rrq[128]={0};
    rrq[1]=1;
    sprintf(rrq+2,"%s",argv[3]);
    sprintf(rrq+3+strlen(argv[3]),"octet");

    sendto(sd,rrq,strlen(argv[3])+9,NULL,
           result->ai_addr,result->ai_addrlen);

    
    
    
    char buffer[BUFFER_SIZE];
    struct sockaddr_in server_addr;
    socklen_t addr_len = sizeof(server_addr);

    ssize_t received_bytes = recvfrom(sd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&server_addr, &addr_len);
    if (received_bytes < 0) { 
        perror("Error receiving data");
        freeaddrinfo(result);
        close(sd);
        exit(EXIT_FAILURE);
    }
   
    char server_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(server_addr.sin_addr), server_ip, INET_ADDRSTRLEN);
    printf("Received packet from %s:%d\n", server_ip, ntohs(server_addr.sin_port));

    uint16_t opcode = ntohs(*(uint16_t *)buffer);
    if (opcode == 3) {
        uint16_t block_number = ntohs(*(uint16_t *)(buffer + 2));
        printf("Received DATA packet: Block %d, Size %zd bytes\n", block_number, received_bytes - 4);
        FILE *file = fopen("received_file", "wb");
        if (!file) {
            perror("Error opening file for writing");
            freeaddrinfo(result);
            close(sd);
            exit(EXIT_FAILURE);
        }
        fwrite(buffer + 4, 1, received_bytes - 4, file);
        fclose(file);
        printf("Data written to 'received_file'.\n");

    } else if (opcode == 5) {
        printf("Received ERROR packet: %s\n", buffer + 4);
    } else {
        printf("Unknown opcode: %d\n", opcode);
    }

    freeaddrinfo(result);
    close(sd);

    return 0;
}
