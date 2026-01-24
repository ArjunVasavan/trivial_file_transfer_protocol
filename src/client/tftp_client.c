#include "../../include/tftp.h"
#include "tftp_client.h"

status connect_should_be_first = FAILURE;

int main() {
    tftp_client_t client;
    memset(&client, 0, sizeof(client));  // Initialize client structure

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

    client->server_len = sizeof(client->server_addr);

    if ( client->sockfd < 0 ) {
        perror("socket failed\n");
        return;
    }

    client->server_addr.sin_family = AF_INET;
    client->server_addr.sin_port = htons(port);
    client->server_addr.sin_addr.s_addr = inet_addr(ip);
    strcpy(client->server_ip,ip);

}

void put_file(tftp_client_t *client, char *filename) {

    // Send WRQ request and send file
    
    printf("Enter the name of file to send: ");
    scanf("%s",filename);

    send_request(client->sockfd,client->server_addr,filename,WRQ);

    char location_of_file[69];

    sprintf(location_of_file,"src/client/%s",filename);

    send_file(client->sockfd,client->server_addr,client->server_len,location_of_file);

}

void get_file(tftp_client_t *client, char *filename) {
    // Send RRQ and recive file 

    printf("Enter the name of file to recieve: ");
    scanf("%s",filename);


}

void disconnect(tftp_client_t *client) {
    // close fd

}


void send_request(int sockfd,struct sockaddr_in server_addr, char *filename, int opcode) {

    tftp_packet send_packet;

    send_packet.opcode = htons(opcode);

    strcpy(send_packet.body.request.filename,filename);

    // TODO: mode is needed here
    
    sendto(sockfd,&send_packet,sizeof(send_packet),0,(struct sockaddr*)&server_addr,sizeof(server_addr));


}
