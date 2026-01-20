/* Common file for server & client */

#include "../../include/tftp.h"

void send_file(int sockfd, struct sockaddr_in client_addr, socklen_t client_len, char *filename) 
{
    // Implement file sending logic here
}

void receive_file(int sockfd, struct sockaddr_in client_addr, socklen_t client_len, char *filename) 
{
    // Implement file receiving logic here
}
