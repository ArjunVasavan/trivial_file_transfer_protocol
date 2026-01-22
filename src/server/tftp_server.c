#include "../../include/tftp.h"
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>


void handle_client(int sockfd, struct sockaddr_in client_addr, socklen_t client_len, tftp_packet *packet);

int main() {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    tftp_packet packet;

    sockfd = socket(AF_INET,SOCK_DGRAM,0);

    if ( sockfd < 0 ) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    /*
     * NOTE: INADDR_ANY
     * INADDR_ANY: it allows server to listen to any interface like local or LAN
     * loop back address 127.0.0.1 also be used here but it will limit the ---
     * system to local interfaces only like only on same machine
    */

  
    if(bind(sockfd,(struct sockaddr*)&server_addr,sizeof(server_addr)) < 0 ) {

        perror("bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);

    }


    printf("TFTP Server listening on port %d...\n", PORT);

    char dummy_test[25];
    recvfrom(sockfd,dummy_test,sizeof(dummy_test),0,(struct sockaddr*)&client_addr,&client_len);

    if(strcmp(dummy_test,"DUMMY_SIGNAL") == 0 ) {
        printf("connected to client recieved dummy signal\n");
    }

    // Main loop to handle incoming requests
    while (1) {
        int n = recvfrom(sockfd, &packet, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &client_len);
        if (n < 0) {
            perror("Receive failed or timeout occurred");
            continue;
        }

         handle_client(sockfd, client_addr, client_len, &packet);
    }

    close(sockfd);
    return 0;
}

void handle_client(int sockfd, struct sockaddr_in client_addr, socklen_t client_len, tftp_packet *packet) 
{
    // Extract the TFTP operation (read or write) from the received packet
    // and call send_file or receive_file accordingly
    
    tftp_opcode tftp_operation = packet->opcode;

    printf("[CHECK]: recieved %d from %s\n",packet->opcode,packet->body.request.filename);

    if ( tftp_operation == WRQ ) {

    } else if ( tftp_operation == RRQ ) {

    } else if ( tftp_operation == DATA ) {
    
    } else if ( tftp_operation == ACK ) {

    } else if ( tftp_operation == ERROR ) {

    } else {

    }
}




