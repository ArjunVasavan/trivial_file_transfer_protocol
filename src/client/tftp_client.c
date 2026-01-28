#include "../../include/tftp.h" // contain common defination  
#include <stdio.h>
#include <string.h>
#include "tftp_client.h" // client only defination and function declar

status connect_should_be_first = FAILURE;
// This is used to check connection should happen first


static tftp_mode current_mode = MODE_DEFAULT;

int main() {

    tftp_client_t client;
    memset(&client, 0, sizeof(client)); 

    while (true) { // an infinite loop till user presses exit

    error_case: // this is an goto when user enters invalid options

        printf("Client Menu\n");
        printf("1) Connect\n");
        printf("2) Put\n");
        printf("3) Get\n");
        printf("4) Mode\n");
        printf("5) Exit\n");
        int choice;
        printf("Enter an Choice: ");
        scanf("%d",&choice);

        switch (choice) {

            case 1:{
                char ip_address[256];
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
                char filename[256];
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
                if ( connect_should_be_first == FAILURE ) {
                    fprintf(stderr,"[ERROR]: Before doing operations first connect with server\n");
                    goto error_case;
                }

                set_mode();

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
    tftp_packet ack_packet;
    recvfrom(client->sockfd,&ack_packet,4,0,NULL,NULL);

    if ( ntohs(ack_packet.opcode) != ACK || ntohs(ack_packet.body.ack_packet.block_number) != 0 ) {
        printf("WRQ rejected\n");
        return;
    }

    char location_of_file[69];
    sprintf(location_of_file,"src/client/%s",filename);
    send_file(client->sockfd,client->server_addr,client->server_len,location_of_file);

}

void get_file(tftp_client_t *client, char *filename) {
    // Send RRQ and recive file 

    printf("Enter the name of file to recieve: ");
    scanf("%s",filename);
    printf("[get_file]: sending request\n");
    send_request(client->sockfd,client->server_addr,filename,RRQ);
    tftp_packet ack_packet;
    recvfrom(client->sockfd,&ack_packet,4,0,NULL,NULL);
    printf("[get_file]: recieved ACK\n");


    if ( ntohs(ack_packet.opcode) != ACK || ntohs(ack_packet.body.ack_packet.block_number) != 0 ) {
        printf("RRQ rejected\n");
        return;
    }

    char fullpath[300];
    snprintf(fullpath,sizeof(fullpath),"src/client/%s",filename);
    receive_file(client->sockfd,client->server_addr,client->server_len,fullpath);
    
}

void disconnect(tftp_client_t *client) {
    // close fd

}


void send_request(int sockfd,struct sockaddr_in server_addr, char *filename, int opcode) {

    tftp_packet send_packet;
    memset(&send_packet,0,sizeof(send_packet));
    send_packet.opcode = htons(opcode);
    strcpy(send_packet.body.request.filename,filename);
    strcpy(send_packet.body.request.mode,mode_to_string(current_mode));
    printf("[send_request]: Sending %s mode\n",send_packet.body.request.mode);
    int size_of_send_packet = 2 + strlen(filename) + 1 + strlen(send_packet.body.request.mode) + 1 ;
    // size_of_send_packet = opcode (2 bytes) + filename + '\0' + mode + 1
    sendto(sockfd,&send_packet,size_of_send_packet,0,(struct sockaddr*)&server_addr,sizeof(server_addr));
}

void set_mode() {

error_set_mode:
    printf("Select Mode: \n");
    printf("1) Default [512] ");
    printf("2) Octet [1] ");
    printf("3) NetASCII [\\n<->\\r\\n][1]");
    printf("4) Exit");
    int choice;
    printf("Enter choice: ");
    scanf("%d",&choice);

    switch (choice) {
    
        case 1: {

            current_mode = MODE_DEFAULT;
            break;
        }
        case 2: {

            current_mode = MODE_OCTET;
            break;
        }
        case 3: {

            current_mode = MODE_NETASCII;
            break;
        }
        case 4: {
            printf("Exiting from Mode...\n");
            return;
        }
        default:{
            fprintf(stderr,"[ERROR]: use above options as choice only");
            goto error_set_mode;
        }
    }

    printf("Current Mode Set to : %s\n",mode_to_string(current_mode));

}

const char* mode_to_string (tftp_mode mode ) {
    switch (mode) {

        case MODE_DEFAULT: {

            return "default";
        }
        case MODE_OCTET: {

            return "octet";
        }
        case MODE_NETASCII: {

            return "netascii";
        }
        default: return "unknown";
    }
}
