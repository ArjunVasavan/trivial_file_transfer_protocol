#include "../../include/tftp.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include "tftp_client.h"

status connect_should_be_first = FAILURE;

int main() {
    tftp_client_t client;
    memset(&client, 0, sizeof(client));  // Initialize client structure

    // Main loop for command-line interface
    while (1) {

    error_case:
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

                if( read_client(ip_address,&port_number) == FAILURE ) {

                    fprintf(stderr,"[ERROR]: Enter valid details\n");
                    goto error_case;

                }
                    
                connect_to_server(&client,ip_address,port_number);

                connect_should_be_first = SUCCESS;

                break;
            }
            case 2:{

                if ( connect_should_be_first == FAILURE ) {

                    fprintf(stderr,"[ERROR]: Before doing operations first connect with server\n");
                    goto error_case;

                }

                char filename[69];

                put_file(&client,filename);

                break;
            }
            case 3:{

                if ( connect_should_be_first == FAILURE ) {

                    fprintf(stderr,"[ERROR]: Before doing operations first connect with server\n");
                    goto error_case;

                }


                char filename[69];

                get_file(&client,filename);

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
                goto error_case;
            }
        }

    }

    return 0;
}

// This function is to initialize socket with given server IP, no packets sent to server in this function
void connect_to_server(tftp_client_t *client, char *ip, int port) {

    client->sockfd = socket(AF_INET,SOCK_DGRAM,0);

    if ( client->sockfd < 0 ) {
        perror("socket failed\n");
        return;
    }

    client->server_addr.sin_family = AF_INET;
    client->server_addr.sin_port = htons(port);
    client->server_addr.sin_addr.s_addr = inet_addr(ip);
    strcpy(client->server_ip,ip);

    char dummy_data[25] = "DUMMY_SIGNAL";
    sendto(client->sockfd,dummy_data,sizeof(dummy_data),0,(struct sockaddr*)&client->server_addr,sizeof(client->server_addr));

}

void put_file(tftp_client_t *client, char *filename) {

    // Send WRQ request and send file
    
    printf("Enter the name of file to send: ");
    scanf("%s",filename);

    tftp_packet put_packet;

    put_packet.opcode = WRQ;

    strcpy(put_packet.body.request.filename,filename);

    sendto(client->sockfd,&put_packet,sizeof(put_packet),0,(struct sockaddr*)&client->server_addr,sizeof(client->server_addr));



}

void get_file(tftp_client_t *client, char *filename) {
    // Send RRQ and recive file 

    printf("Enter the name of file to recieve: ");
    scanf("%s",filename);

    tftp_packet put_packet;

    put_packet.opcode = RRQ;

    strcpy(put_packet.body.request.filename,filename);

    sendto(client->sockfd,&put_packet,sizeof(put_packet),0,(struct sockaddr*)&client->server_addr,sizeof(client->server_addr));


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
