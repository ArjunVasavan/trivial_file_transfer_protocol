#include "../../include/tftp.h"
#include <netinet/in.h>
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

void handle_client(int sockfd, struct sockaddr_in client_addr, socklen_t client_len, tftp_packet *packet) {
    // Extract the TFTP operation (read or write) from the received packet
    // and call send_file or receive_file accordingly

    tftp_opcode tftp_operation = ntohs(packet->opcode);

    if ( tftp_operation == WRQ ) {

        tftp_packet ack_packet;
        ack_packet.opcode = htons(ACK);
        ack_packet.body.ack_packet.block_number = htons(0);

        sendto(sockfd,&ack_packet,4,0,(struct sockaddr*)&client_addr,client_len);

        receive_file(sockfd,client_addr,client_len,packet->body.request.filename);



    }  else if (tftp_operation == RRQ) {



    } else {



    }
}




