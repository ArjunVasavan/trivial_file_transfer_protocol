#include "tftp.h"
#include "tftp_client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>



int main() {
    tftp_client_t client;
    memset(&client, 0, sizeof(client));  // Initialize client structure

    // Main loop for command-line interface
    while (1) {
 /* TODO: 
    print main menu and read choice from user and based on choice perform operation
 */
    }

    return 0;
}

// This function is to initialize socket with given server IP, no packets sent to server in this function
void connect_to_server(tftp_client_t *client, char *ip, int port) {
    // Create UDP socket
  
 // TODO: read the server address and port number after that validate 
    // bind


}

void put_file(tftp_client_t *client, char *filename) {
    /*
     NOTE: put_file
     read file name and validate
     if ( exist ) => send

    */

    // Send WRQ request and send file

}

void get_file(tftp_client_t *client, char *filename) {
    // Send RRQ and recive file 
  
}

void disconnect(tftp_client_t *client) {
    // close fd
   
}
void send_request(int sockfd, sockaddr_in server_addr, char *filename, int opcode)
{

}

void receive_request(int sockfd, sockaddr_in server_addr, char *filename, int opcode)
{
}
