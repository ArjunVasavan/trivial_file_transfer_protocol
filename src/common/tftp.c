#include "../../include/tftp.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void send_file(int sockfd, struct sockaddr_in address, socklen_t len, char *filename, tftp_mode mode) {

    // Implement file sending logic here
    char buf[100];
    printf("[send_file]:opening %s\n",filename);
    int fd = open(filename,O_RDONLY);
    printf("[send_file]:opening successfull\n");

    if ( fd < 0 ) {
        perror("open failed");
        return;
    }

    uint16_t block_number = 1;
    int read_count = 0;
    tftp_packet packet;
    packet.opcode = htons(DATA);
    int last_was_full = 0;
    int chunk_size;

    // These are for net ascii operation
    char read_buffer[512];
    char converted_buffer[1024]; // it is for worst case scenario of \n -> \r\n

    if ( mode == MODE_DEFAULT ) {
        chunk_size = 512;
    } else if ( mode == MODE_OCTET ) {
        chunk_size = 1;
    } else if ( mode == MODE_NETASCII ) {
        chunk_size = 512;
    }

    while ( true ) { 


        if ( mode == MODE_NETASCII ) { // for netascii im using  an tmp buffer
            read_count = read(fd,read_buffer,chunk_size);
        } else {
            read_count = read(fd, packet.body.data_packet.data,chunk_size);
        }

        if ( read_count <= 0 ) break;

        if ( mode == MODE_NETASCII ) {
            int converted_len = convert_to_netascii(read_buffer,read_count,converted_buffer,1024);

            memcpy(packet.body.data_packet.data,converted_buffer,converted_len);
            read_count = converted_len;
            printf("[send_file]: NETASCII Converted %d bytes\n",converted_len);
        }


        printf("[send_file]: read ""%s""  \n",packet.body.data_packet.data);
        packet.body.data_packet.block_number = htons(block_number);
        packet.body.data_packet.data_size = read_count;
        unsigned long int packet_length = 2 + 2 + read_count;
        // PACKET LENGTH => 2 bytes(opcode) + 2 bytes (block_number) + read_count

    once_more_send:

        printf("[send_file]: sending packet block %d  , size %d bytes (mode=%d)\n",
               block_number,read_count,mode);
        sendto(sockfd,&packet,packet_length,0,(struct sockaddr*)&address,len);

        /*
         * NOTE: How sendto sends data from start of address to packet_length
         * data is sended from [start of address: &packet -> packet_length]  
         * when i fill the data in union of this tftp_packet type
         * here memory become like this =>> [ opcode ][ block ][ data bytes ][ rest garbage ]
         *                                  start address -->
         * in union all the unnecessery type we doesnt used will become garbage and will be only 
         * after the essential data we added
         * * Union members do not have their own particular region in memory  
         * * they overlap at the same address i.e. which type is used will be at first
         *   rest garbage will be after the essential types
         * * so sending start address plus exact length sends only the intended fields.
        */

        tftp_packet ack_packet;
        int ack_pack_length = 2 + 2 ;
        // ack_pack_length = 2 (opcode) + 2(block_number) 

        struct sockaddr_in from_addr;
        socklen_t from_len = sizeof(from_addr);

        recvfrom(sockfd,&ack_packet,ack_pack_length,0,(struct sockaddr*)&from_addr,&from_len);
        int ack_block_number = ntohs(ack_packet.body.ack_packet.block_number);

        if ( ntohs(ack_packet.opcode) !=  ACK ) {
            goto once_more_send;
        } else if ( ack_block_number != block_number ) {

            // FIXME: modify goto section logic
            // fix: wrong op code
            // recvfrom failure

            goto once_more_send;
        }

        if ( read_count == chunk_size ) {

            last_was_full  = 1;

        } else {

            last_was_full = 0;

        }

        block_number+=1;

        if ( read_count < chunk_size ) {

            break;
        }

        printf("[send_file]: Comeplete data sended\n");

    }

    printf("[send_file]: exited\n");

    // NOTE: EDGE CASE => if last data size is mutliple of 512
    // the sender must send one extra data packet with 0 bytes

    if (last_was_full) {
        // send final DATA with 0 bytes
        memset(&packet,0,sizeof(packet));
        packet.opcode = htons(DATA);
        packet.body.data_packet.block_number = htons(block_number);
        unsigned long packet_length = 2 + 2;

    edge_case:

        sendto(sockfd, &packet, packet_length, 0,
               (struct sockaddr*)&address, len);

        // wait for ACK of this block

        tftp_packet ack_packet;

        int ack_pack_length = 2 + 2 ;
        // ack_pack_length = 2 (opcode) + 2(block_number) 

        struct sockaddr_in from_addr;
        socklen_t from_len = sizeof(from_addr);


        recvfrom(sockfd,&ack_packet,ack_pack_length,0,(struct sockaddr*)&from_addr,&from_len);

        int ack_block_number = ntohs(ack_packet.body.ack_packet.block_number);

        if ( ntohs(ack_packet.opcode) !=  ACK ) {

            goto edge_case;


        } else if ( ack_block_number != block_number ) {

            goto edge_case;

        }

        printf("Completed last_was_full case\n");

    }

    printf("Completed Sending (mode %d)\n",mode);

    close(fd);
}

void receive_file(int sockfd, struct sockaddr_in address, socklen_t len, char *filename, tftp_mode mode) {

    printf("[receive_file]: opening %s\n",filename);
    int fd = open(filename,O_WRONLY|O_TRUNC|O_CREAT,0666); 
    printf("[receive_file]: opening successfull\n");
    unsigned long int expected_block = 1;
    tftp_packet data_packet;
    long int recieved_bytes;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    tftp_packet ack_packet;

    int expected_chunk_size = (mode == MODE_OCTET) ? 1 : 512; // NetAscii and Default are 512
    
    if ( mode == MODE_OCTET ) {
        expected_chunk_size = 1;
    } else {
        expected_chunk_size = 512;
    }

    char converted_buffer[1024];

    while ( true ) {

    once_more_read:

        recieved_bytes = recvfrom(sockfd,&data_packet,BUFFER_SIZE,
                                  0,(struct sockaddr*)&client_addr,&client_len);

        printf("[BYTES RECIEVED]: %lu\n",recieved_bytes);
        uint16_t opcode = ntohs(data_packet.opcode);
        uint16_t block_number = ntohs(data_packet.body.data_packet.block_number);

        if ( opcode == DATA ) {

            if ( block_number != expected_block ) {

                ack_packet.opcode = htons(ACK);

                ack_packet.body.ack_packet.block_number = htons(expected_block - 1);

                // 4 byets => opcode + block_number

                sendto(sockfd,&ack_packet,4,0,(struct sockaddr*)&client_addr,client_len);

                goto once_more_read;

                // FIXME: modify goto later 

            }

            int data_length = recieved_bytes - 4 ; // 4 => opcode + block_number

            if ( data_length > 0 ) {
                if ( mode == MODE_NETASCII ) {
                    int converted_len = convert_from_netascii(data_packet.body.data_packet.data,data_length,converted_buffer,1024);

                    write(fd,converted_buffer,converted_len);
                    printf("[receive_file]: NetASCII wrote %d bytes\n",converted_len);
                } else {
                    write(fd,data_packet.body.data_packet.data,data_length);
                }
            }

            ack_packet.opcode = htons(ACK);
            ack_packet.body.ack_packet.block_number = htons(expected_block);
            sendto(sockfd,&ack_packet,4,0,(struct sockaddr*)&client_addr,client_len);
            expected_block+=1;
            if ( data_length < expected_chunk_size ) break;
        }

    }

    printf("Completed Recieving (mode %d)\n",mode);

    close(fd);
}

//Converts \n to \r\n
int convert_to_netascii(char* input, int input_len, char* output, int max_output ) {

    int output_idx = 0;

    for ( int i = 0 ; i < input_len && output_idx < max_output -1 ; i++ ) {
        if ( input[i] == '\n' ) {
            //checking if its already \n\r to prevent double convert 
            if ( i > 0 && input[i-1] == '\r' ) {
                output[output_idx++] = input[i];
            } else {
                //converting \n to \r\n 
                output[output_idx++] = '\r';
                output[output_idx++] = '\n';
            }

        } else {
            output[output_idx++] = input[i];
        }
    } 

    return output_idx;
}

//convert \r\n to \n 

int convert_from_netascii(char* input, int input_len, char* output, int max_output) {
    int output_idx = 0;

    for ( int i = 0 ; i < input_len && output_idx < max_output - 1 ; i++ ) {
        if ( i < input_len - 1 &&  input[i] == '\r' && input[i+1] == '\n' ) {
            //convert \r\n to \n 
            output[output_idx++] = '\n';
            i+=1; // skipping the next \n [\r\n -> \n]
        } else {
            output[output_idx++] = input[i];
        }
    }

    return output_idx;
}
