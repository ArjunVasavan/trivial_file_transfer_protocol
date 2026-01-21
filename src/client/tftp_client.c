#include "../../include/tftp.h"
#include <stdio.h>
#include "tftp_client.h"



int main() {
    tftp_client_t client;
    memset(&client, 0, sizeof(client));  // Initialize client structure

    // Main loop for command-line interface
    while (1) {

    error_choice:
        printf("Client Menu\n\n");
        printf("1) Connect\n");
        printf("2) Put\n");
        printf("3) Get\n");
        printf("4) Mode\n");
        printf("5) Exit\n");
        int choice;
        scanf("%d",&choice);

        switch (choice) {

            case 1:{

                char ip_address[69];
                int port_number;

                    

                break;
            }
            case 2:{

                break;
            }
            case 3:{

                break;
            }
            case 4:{

                break;
            }
            case 5:{

                break;
            }
            default:{

                fprintf(stderr,"[ERROR] Enter any choice in (1-5) range\n");
                goto error_choice;
            }
        }

        return 0;
    }

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
/*

 FIXME: here some structure data type bug is here commented for fixing later

void send_request(int sockfd, sockaddr_in server_addr, char *filename, int opcode)
{

}


void receive_request(int sockfd, sockaddr_in server_addr, char *filename, int opcode)
{
}

*/
