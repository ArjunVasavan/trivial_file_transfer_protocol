#include "../../include/tftp.h"
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

static tftp_mode current_mode = MODE_DEFAULT;

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

    while (true) {

        printf("[main]: trying to recieved ack\n");

        int n = recvfrom(sockfd, &packet, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &client_len);

        printf("[main]: recieved ack\n");

        if (n < 0) {
            perror("Receive failed");
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
    
    char* filename = packet->body.request.filename;
    char* mode = filename+ strlen(filename) + 1;

    if ( tftp_operation == WRQ || tftp_operation == RRQ ) {

        if ( strcmp(mode,"default") == 0 ) {
            current_mode = MODE_DEFAULT;
        } else if ( strcmp(mode,"octet") == 0 ) {
            current_mode = MODE_OCTET;
        } else if ( strcmp(mode,"netascii") == 0 ) {
            current_mode = MODE_NETASCII;
        } else {

            printf("Transfer Mode: %s\n",mode);
            fprintf(stderr,"[ERROR]: illegal mode recieved\n");
            return;
        }

        printf("Transfer Mode: %s\n",mode);
    }


    if ( tftp_operation == WRQ ) {

        tftp_packet ack_packet;
        ack_packet.opcode = htons(ACK);
        ack_packet.body.ack_packet.block_number = htons(0);

        sendto(sockfd,&ack_packet,4,0,(struct sockaddr*)&client_addr,client_len);
        char fullpath[300];
        snprintf(fullpath,sizeof(fullpath),"src/server/%s",packet->body.request.filename);
        receive_file(sockfd,client_addr,client_len,fullpath);


    }  else if (tftp_operation == RRQ) {

        tftp_packet ack_packet;
        ack_packet.opcode = htons(ACK);
        ack_packet.body.ack_packet.block_number = htons(0);



        char fullpath[300];
        snprintf(fullpath,sizeof(fullpath),"src/server/%s",packet->body.request.filename);
        sendto(sockfd,&ack_packet,4,0,(struct sockaddr*)&client_addr,client_len);
        send_file(sockfd,client_addr,client_len,fullpath);


    } else {



    }
}




