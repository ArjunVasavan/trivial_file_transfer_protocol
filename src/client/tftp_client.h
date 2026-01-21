#ifndef TFTP_CLIENT_H
#define TFTP_CLIENT_H

#include "../../include/tftp.h"

typedef struct {
    int sockfd;
    struct sockaddr_in server_addr;
    socklen_t server_len; 
    char server_ip[INET_ADDRSTRLEN]; // NOTE: we store ip address in this
    // TODO: declare one more for portnumber as server_port
} tftp_client_t;

// Function prototypes
void connect_to_server(tftp_client_t *client, char *ip, int port);
void put_file(tftp_client_t *client, char *filename);
void get_file(tftp_client_t *client, char *filename);
void disconnect(tftp_client_t *client);

// NOTE: belowth functions are for client only
// opcode -> operation code
void send_request(int sockfd, struct sockaddr_in server_addr, char *filename, int opcode);
void receive_request(int sockfd, struct sockaddr_in server_addr, char *filename, int opcode);
status read_client(char* ip_address,int* port_number);
status validate_port(int port_number);
status validate_ip(char* ip_address);

#endif
